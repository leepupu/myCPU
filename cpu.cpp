#include "program.h"
#include "cpu.h"

CPU::CPU()
{
  this->pProgram = NULL;
}

CPU::CPU(DataMemory* pm, Register* pr)
{
  this->pProgram = NULL;
  this->pDataMemory = pm;
  this->pRegister = pr;
  if_id = new IF_ID();
  id_ex = new ID_EX();
  ex_mem= new EX_MEM();
  mem_wb = new MEM_WB();
  PC = 0;
}

CPU::~CPU()
{
  if(this->pProgram != NULL)
    delete this->pProgram;
  delete if_id;
  delete id_ex;
  delete ex_mem;
  delete mem_wb;
}

/**
 * reset all pipeline register
 */
void CPU::reset()
{
  PC = prePC = 0;
  if(this->pProgram != NULL)
  {
    delete this->pProgram;
    this->pProgram = NULL;
  }
  delete if_id;
  delete id_ex;
  delete ex_mem;
  delete mem_wb;
  if_id = new IF_ID();
  id_ex = new ID_EX();
  ex_mem= new EX_MEM();
  mem_wb = new MEM_WB();
  pDataMemory->init();
  pRegister->init();
}

bool CPU::checkStatus(const int* reg, const int* mem)
{
  return (pRegister->compare(reg) && pDataMemory->compare(mem));
}

void CPU::setProgram(Program *pP)
{
  this->pProgram = pP;
}

void CPU::execute()
{
  if(this->pProgram == NULL)
    return;
  for(int clock_cycle = 1 ; (clock_cycle == 1) || !isFinish() ; clock_cycle++)
  {
    if(DEBUG && clock_cycle > 10) // preserve inf loop
      break;

    // 5 pipeline stages
    IF();
    ID();
    EX();
    MEM();
    WB();

    // dump data
    printf("CC %d:\n", clock_cycle);
    pRegister->dump();
    pDataMemory->dump();
    if_id->dump();
    id_ex->dump();
    ex_mem->dump();
    mem_wb->dump();
    printf("=================================================================\n");
  }

}

void CPU::IF()
{
  if(DEBUG)
    printf("processing IF\n");
  Instruction *pInst = pProgram->nextAt(PC / 4); // fetch
  prePC = PC;
  PC += 4; // immedate add 4 to next inst
  
  // do branch in wb
  if_id->setNew(IF_ID::INSTRUCTION, pInst->getRaw());
  if_id->setNew(IF_ID::PC, PC);
}

void CPU::ID()
{
    if(DEBUG)
    {
      printf("processing ID\n");
    }
    Instruction *pInst;
    pInst = new Instruction(if_id->getPre(IF_ID::INSTRUCTION));
    int type = pInst->getType();

    if(pInst->getRaw() != 0) // handle NOP
      id_ex->csNew->setType(type);
    else
      id_ex->csNew->clear();

    int rs = 0, rt = 0, rd = 0, ReadData0 = 0, ReadData1 = 0, sign_ext = 0;

    // WriteBack Process
    if(mem_wb->csPre->getBitSignal(ControlSignal::RegWrite))
    {
      int data = 0;
      if(mem_wb->csPre->getBitSignal(ControlSignal::Mem2Reg))
        data = mem_wb->getPre(MEM_WB::ReadData);
      else
        data = mem_wb->getPre(MEM_WB::ALUout);
      if(DEBUG)
        printf("write data: %d, to: %d\n", data, mem_wb->getPre(MEM_WB::Dst));
      pRegister->write(mem_wb->getPre(MEM_WB::Dst), data);
    }

    // Decode Inst, ReadReg Process
    rs = pInst->getRs();
    rt = pInst->getRt();
    ReadData0 = pRegister->read( rs );
    ReadData1 = pRegister->read( rt );
    if(DEBUG)
    {
        printf("type: %s(%d)\n", InstType::decodeType(type), type);
        printf("decode rs: %d, rt: %d\n", rs, rt);
    }

    switch(type)
    {
      case(InstType::RType):
        rd = pInst->getRd();
        break;
      case(InstType::swType):
      case(InstType::lwType):
      case(InstType::brType):
      case(InstType::brneType):
        sign_ext = pInst->getITypeImmediate();
        break;
      default:
        printf("unknow type\n");
    }

    int preCalPC = if_id->getPre(IF_ID::PC) + (sign_ext * 4);
    if(DEBUG)
    {
      printf("set pre cal PC: %d\n", preCalPC);
    }

    // Control Hazard
    if(DEBUG)
    {
      printf("check control hazard\n");
      printf("type: %d, ReadData0: %d, ReadData1: %d\n", type, ReadData0, ReadData1);
    }
    bool branchStall = false;
    if((type == InstType::brType) || (type == InstType::brneType))
    { // not cal yet, need to stall here
      // Bonus, stall first, than internal forwarding
      Instruction pExInst(id_ex->getPre(ID_EX::INSTRUCTION));
      if(pExInst.getType() == InstType::RType && (rs == id_ex->getPre(ID_EX::Rd) || rt == id_ex->getPre(ID_EX::Rd)))
      {
        if(DEBUG)
        {
          printf("branch stalling\n");
        }
        printf("Control hazard detection, stalling...\n");
        if_id->setNew(IF_ID::INSTRUCTION, pInst->getRaw());
        if_id->setNew(IF_ID::PC, if_id->getPre(IF_ID::PC));
        pInst->clear();
        PC = prePC;
        //ReadData0 = ReadData1 = sign_ext = rs = rt = rd = 0;
        id_ex->csNew->clear();
        branchStall = true;
      }
      else
      { // do internal forward here, too
        if(mem_wb->csPre->getBitSignal(ControlSignal::RegWrite) &&
           mem_wb->getPre(MEM_WB::Dst) != 0)
        {
          if(DEBUG)
          {
            printf("Control hazard detection, enter detect mem hazard detection\n");
          }
          if(mem_wb->getPre(MEM_WB::Dst) == rs)
          {
            ReadData0 = mem_wb->getPre(MEM_WB::ALUout);
            fprintf(stderr, "Control hazard's data hazard! forward mem_wb.ALUout to ReadData0!\n");
          }
          else if(mem_wb->getPre(MEM_WB::Dst) == rt)
          {
            ReadData1 = mem_wb->getPre(MEM_WB::ALUout);
            fprintf(stderr, "Control hazard's data hazard! forward mem_wb.ALUout to ReadData1!\n");
          }
        }

        if(ex_mem->csPre->getBitSignal(ControlSignal::RegWrite) &&
           ex_mem->getPre(EX_MEM::Rt) != 0 )
        {
          if(DEBUG)
          {
            printf("Control hazard detection, enter detect ex data hazard\n");
          }
          if(ex_mem->getPre(EX_MEM::Rt) == rs)
          {
            ReadData0 = ex_mem->getPre(EX_MEM::ALUout);
            fprintf(stderr, "Control hazard's data hazard! forward ex_mem.ALUout to ReadData0!\n");
          }
          else if(ex_mem->getPre(EX_MEM::Rt) == rt)
          {
            ReadData1 = ex_mem->getPre(EX_MEM::ALUout);
            fprintf(stderr, "Control hazard's data hazard! forward ex_mem.ALUout to ReadData1!\n");
          }
        }
      }
    }
    if(!branchStall && (type == InstType::brType && ReadData0 == ReadData1) || (type == InstType::brneType && ReadData0 != ReadData1))
    {
      
      if_id->setNew(IF_ID::INSTRUCTION, 0);
      PC = preCalPC;
    }

    id_ex->setNew(ID_EX::PC, preCalPC);
    id_ex->setNew(ID_EX::Rd0, ReadData0);
    id_ex->setNew(ID_EX::Rd1, ReadData1);
    id_ex->setNew(ID_EX::signEXT, sign_ext);
    id_ex->setNew(ID_EX::Rs, rs);
    id_ex->setNew(ID_EX::Rt, rt);
    id_ex->setNew(ID_EX::Rd, rd);
    id_ex->setNew(ID_EX::INSTRUCTION, pInst->getRaw());

    if_id->update();
    delete pInst;
}

void CPU::EX()
{
  if(DEBUG)
    printf("processing EX\n");
  int rt = 0;
  int ALUout = 0;
  Instruction *pInst;
  pInst = new Instruction(id_ex->getPre(ID_EX::INSTRUCTION));
  int pc = id_ex->getPre(ID_EX::PC);
  int sign_ext = id_ex->getPre(ID_EX::signEXT);
  int pre_rs = id_ex->getPre(ID_EX::Rs);
  int pre_rt = id_ex->getPre(ID_EX::Rt);
  int pre_rd = id_ex->getPre(ID_EX::Rd);
  int rd0 = id_ex->getPre(ID_EX::Rd0);
  int rd1 = id_ex->getPre(ID_EX::Rd1);
  int write_data = 0;
  ControlSignal* cs = id_ex->csPre;
  int funct = pInst->getFunct();
  int ALUOp0 = cs->getBitSignal(ControlSignal::ALUOp0), ALUOp1 = cs->getBitSignal(ControlSignal::ALUOp1);
  int realALUOp = ALUCtrl(ALUOp0, ALUOp1, funct);
  bool zero_flag = (rd0 == rd1);
  bool stall = false;

  // detect internal data hazard 
  // detect mem hazard first
  // override by ex hazard
  if(DEBUG)
  {
    printf("try detect mem hazard\n");
    printf("RegWrite: %d, Dst: %d\n", mem_wb->csPre->getBitSignal(ControlSignal::RegWrite), mem_wb->getPre(MEM_WB::Dst));
  }
  if(mem_wb->csPre->getBitSignal(ControlSignal::RegWrite) &&
           mem_wb->getPre(MEM_WB::Dst) != 0)
  {
    if(DEBUG)
    {
      printf("enter detect mem hazard detection\n");
    }
    if(mem_wb->getPre(MEM_WB::Dst) == pre_rs)
    {
      rd0 = mem_wb->getPre(MEM_WB::ALUout);
      fprintf(stderr, "data hazard! forward mem_wb.ALUout to rd0!\n");
    }
    else if(mem_wb->getPre(MEM_WB::Dst) == pre_rt)
    {
      rd1 = mem_wb->getPre(MEM_WB::ALUout);
      fprintf(stderr, "data hazard! forward mem_wb.ALUout to rd1!\n");
    }
    if(DEBUG)
      printf("rd0: %d, rd1: %d\n", rd0, rd1);
  }

  if(ex_mem->csPre->getBitSignal(ControlSignal::RegWrite) &&
     ex_mem->getPre(EX_MEM::Rt) != 0 )
  {
    if(DEBUG)
    {
      printf("enter detect ex data hazard\n");
    }
    if(ex_mem->getPre(EX_MEM::Rt) == pre_rs)
    {
      rd0 = ex_mem->getPre(EX_MEM::ALUout);
      fprintf(stderr, "data hazard! forward ex_mem.ALUout to rd0!\n");
    }
    else if(ex_mem->getPre(EX_MEM::Rt) == pre_rt)
    {
      rd1 = ex_mem->getPre(EX_MEM::ALUout);
      fprintf(stderr, "data hazard! forward ex_mem.ALUout to rd1!\n");
    }
  }

  Instruction preInst = Instruction(if_id->getPre(IF_ID::INSTRUCTION));
  if(cs->getBitSignal(ControlSignal::MemRead) &&
         (pre_rt == preInst.getRs() || pre_rt == preInst.getRt())) // stall
  {
    fprintf(stderr, "data hazard! load hazard! Stalling...\n");
    if(DEBUG)
      printf("stalling!\n");
    stall = true;
    id_ex->csNew->clear();
    if_id->setPre(IF_ID::INSTRUCTION, id_ex->getNew(ID_EX::INSTRUCTION));
    if_id->setPre(IF_ID::PC, id_ex->getNew(ID_EX::PC));
    //if_id->stall();
    PC = prePC;
  }

  if(DEBUG)
  {
      printf("rd0: %d, rd1: %d\n", rd0, rd1);
      printf("ALUOp0: %d, ALUOp1: %d, sign_ext: %d\n", ALUOp0, ALUOp1, sign_ext);
      printf("CS.ALUSrc: %d, funct: %d, realALUOp: %d\n", id_ex->csPre->getBitSignal(ControlSignal::ALUSrc), funct, realALUOp);
  }
  if(cs->getBitSignal(ControlSignal::ALUSrc)) // I-type MUX
  {
    rd1 = sign_ext;
  }
  if(cs->getBitSignal(ControlSignal::MemWrite))
  {
      write_data = rd1;
  }

  // i am ALU~
  if(!cs->getBitSignal(ControlSignal::Branch) && !cs->getBitSignal(ControlSignal::BranchNE))
  {
    switch(realALUOp)
    {
      case(0): // 000
        ALUout = rd0 & rd1;
        break;
      case(1): // 001
        ALUout = rd0 | rd1;
        break;
      case(2): // 010
        ALUout = rd0 + rd1;
        break;
      case(6): // 110
        ALUout = rd0 - rd1;
        break;
      // slt?
      default:
        break;
    }
  }
  if(DEBUG)
  {
    printf("ALUout: %d\n", ALUout);
  }

  /*
  // Control Hazard Detection Unit
  if(zero_flag && ( cs->getBitSignal(ControlSignal::Branch) || cs->getBitSignal(ControlSignal::BranchNE) ))
  { // branch
    if(DEBUG)
    {
      printf("doing branch, new pc = %d\n", pc);
    }
    if_id->setPre(IF_ID::PC, pc);
    if_id->setPre(IF_ID::INSTRUCTION, 0);// clear wrong inst
    // clear cs?
  }*/

  // PC adder, branch pre cal
  if(cs->getBitSignal(ControlSignal::RegDst))
  {
    rt = id_ex->getPre(ID_EX::Rd);
  }
  else
  {
    rt = id_ex->getPre(ID_EX::Rt);
  }
  if(DEBUG)
  {
    printf("Rt: %d\n", rt);
  }

  if((cs->getBitSignal(ControlSignal::Branch) || cs->getBitSignal(ControlSignal::BranchNE)))
  {
    ex_mem->csNew->clear();
    rt = 0;
    ALUout = 0;
    write_data = 0;
  }
  else
  {
    ex_mem->csNew->copy(cs);
  }

  //if(id_ex->csPre->getSignal()) // not NOP// no need to clean when stall
  
  ex_mem->setNew(EX_MEM::Rt, rt);
  ex_mem->setNew(EX_MEM::ALUout, ALUout);
  ex_mem->setNew(EX_MEM::WriteData, write_data);

  
  if(DEBUG)
  {
    printf("set new pc: %d\n", pc + (sign_ext << 2));
  }
  ex_mem->setNew(EX_MEM::PC, pc + (sign_ext << 2));// shift left 2 to alig instruction : 4 bytes/inst
  id_ex->update();
}

void CPU::MEM()
{
  if(DEBUG)
    printf("processing MEM\n");
  int ALUout = ex_mem->getPre(EX_MEM::ALUout);
  int write_data = ex_mem->getPre(EX_MEM::WriteData);
  int dst = ex_mem->getPre(EX_MEM::Rt);
  int pc = ex_mem->getPre(EX_MEM::PC);
  if(DEBUG)
  {
      printf("dst: %d\n", dst);
  }

  int read_data = 0;

  ControlSignal* cs = ex_mem->csPre;

  if(DEBUG)
    cs->dump(32);

  if(cs->getBitSignal(ControlSignal::MemWrite))
  {
    pDataMemory->write(ALUout, write_data);
  }
  else if(cs->getBitSignal(ControlSignal::MemRead))
  {
    read_data = pDataMemory->read(ALUout);
  }

  
  

  mem_wb->csNew->copy(cs);
  mem_wb->setNew(MEM_WB::ALUout, ALUout);
  mem_wb->setNew(MEM_WB::ReadData, read_data);
  mem_wb->setNew(MEM_WB::Dst, dst);
  ex_mem->update();
}

void CPU::WB()
{
  if(DEBUG)
    printf("processing WB\n");
  
  mem_wb->update();
}

/**
 * Chap4_1 p36 ALUCtrl hardware simulation
 * @param  ALUop0 [description]
 * @param  ALUop1 [description]
 * @param  funct  [description]
 * @return        [Real ALU operation]
 */
int CPU::ALUCtrl(int ALUop0, int ALUop1, int funct)
{
  if(DEBUG)
  {
    printf("ALUCtrl\n");
    for(int i=5;i>=0;i--)
      if( (1 << i) & funct )
        printf("1");
      else
        printf("0");
    printf("\n");
  }
  int f0 = (funct & 1);
  int f1 = (funct & 2) >> 1;
  int f2 = (funct & 4) >> 2;
  int f3 = (funct & 8) >> 3;
  if(DEBUG)
  {
    printf("f0: %d, f1: %d, f2: %d, f3: %d\n", f0, f1, f2, f3);  
  }
  
  int rtn = 0;
  rtn |= (f0 | f3) & ALUop1;
  rtn |= (!f2 | !ALUop1) << 1;
  rtn |= ( (f1 & ALUop1) | ALUop0 ) << 2;
  return rtn;
}

bool CPU::isFinish()
{
  if(DEBUG)
    printf("isClear: %d, %d, %d, %d\n", if_id->isClear(), id_ex->isClear(), ex_mem->isClear(), mem_wb->isClear());
  if(if_id->isClear() && id_ex->isClear() && ex_mem->isClear() && mem_wb->isClear())
    return true;
  return false;
}



