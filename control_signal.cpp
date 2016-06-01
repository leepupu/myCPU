#include "control_signal.h"

void ControlSignal::setType(int type, int op)
{
  switch(type)
  {
    case InstType::RType:
      csignal = (mask(RegWrite) | mask(ALUOp1) | mask(RegDst));
      break;
    case InstType::lwType:
      csignal = (mask(Mem2Reg) | mask(RegWrite) | mask(MemRead) | mask(ALUSrc));
      break;
    case InstType::swType:
      csignal = (mask(MemWrite) | mask(ALUSrc));
      break;
    case InstType::brType:
      csignal = (mask(Branch) | mask(ALUOp0));
      break;
    case InstType::brneType:
      csignal = (mask(BranchNE) | mask(ALUOp0));
      break;
    case InstType::IType:
      csignal = (mask(ALUSrc) | mask(RegWrite) );
      if (op == 13)
        csignal |= (mask(ALUOp0) | mask(ALUOp1));

  }
}

int ControlSignal::mask(int offset)
{
  return 1 << offset;
}

void ControlSignal::setSignal(int cs)
{
  csignal = cs;
}

int ControlSignal::getSignal()
{
  return csignal;
}

bool ControlSignal::getBitSignal(int offset)
{
  return csignal & mask(offset);
}

void ControlSignal::clear()
{
  csignal = 0;
}

void ControlSignal::copy(ControlSignal* cs)
{
  this->csignal = cs->getSignal();
}

void ControlSignal::dump(int wid)
{
  printf("Control signals\t");
  for(int i=wid-1;i>=0;i--)
  {
    if(getBitSignal(i))
      printf("1");
    else if(i == Branch && getBitSignal(BranchNE)) // I add other signal to do bne
      printf("1");
    else
      printf("0");
  }
  printf("\n");
}
