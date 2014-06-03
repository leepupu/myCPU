#include <iostream>
#include "mycpu.h"
#include "cpu.h"
#include "program.h"
#include "pipeline_reg.h"
#include "control_signal.h"
#ifdef DEBUG
#include <assert.h>
#endif

using namespace std;

void unitTestPipelineReg()
{
    IF_ID if_id;
    printf("IF_ID::PC = %u\n", IF_ID::PC);
    if_id.setNew(IF_ID::INSTRUCTION, 843215);
    if_id.setNew(IF_ID::PC, 4);
    printf("get old pc: %u\n", if_id.getPre(IF_ID::PC));
    assert( if_id.getPre(IF_ID::PC) == 0 );
    if_id.update();
    if_id.setNew(IF_ID::PC, 8);
    printf("get old pc: %u\n", if_id.getPre(IF_ID::PC));
    assert( if_id.getPre(IF_ID::PC) == 4 );
    if_id.update();
    if_id.setNew(IF_ID::PC, 12);
    printf("get old pc: %u\n", if_id.getPre(IF_ID::PC));
    assert( if_id.getPre(IF_ID::PC) == 8 );
}

void unitTestControlSignal()
{
  ControlSignal cs;
  cs.setType(InstType::RType);
  printf("cs.signal = %u\n", cs.getSignal());
  assert( cs.getBitSignal(ControlSignal::RegWrite) );
  assert( cs.getBitSignal(ControlSignal::RegDst) );
  assert( cs.getBitSignal(ControlSignal::ALUOp1) );
}

int main(int argsNum, char** args)
{
  freopen("myoutput.txt", "w", stdout);
  if(DEBUG && 0)
  {
    int num = (-1 - ((1 << 26)-1));
    for(int i=31;i>=0;i--)
      if(num & (1 << i))
        printf("1");
      else
        printf("0");
    printf("\n");
    unitTestPipelineReg();
    unitTestControlSignal();
  }

  const int arrInstrInReg[] = {0, 3, 2, 2, 4, 1, 6, 7, 8};
  const int arrInstrInMem[] = {1, 2, 3, 4, 5};

  const int arrGeneralReg[] = {0, 5, -1, 0, 4, 1, 6, 7, 8};
  const int arrGeneralMem[] = {1, 2, 3, 4, 5};

  const int arrDatahazardReg[] = {0, 1, -2, 3, -2, 1, 6, 7, 8};
  const int arrDatahazardMem[] = {1, 2, 3, 4, 5};

  const int arrLwhazardReg[] = {0, 1, 4, 3, 4, 1, 6, 7, 8};
  const int arrLwhazardMem[] = {1, 2, 3, 4, 5};

  const int arrBranchhazardReg[] = {0, 1, 2, 3, 4, 1, 6, 7, 8};
  const int arrBranchhazardMem[] = {1, 2, 3, 4, 5};

  const int arrBranchbonusReg[] = {0, 1, 1, 3, 4, 1, 6, 7, 8};
  const int arrBranchbonusMem[] = {1, 2, 3, 4, 5};

  const char *inputFileNames[] = {"BranchHazard.txt", "InstrIn.txt", "General.txt", "Datahazard.txt", "Lwhazard.txt", "Branchbonus.txt"};
  const char *outputFileNames[] = {"BranchhazardOut.txt", "InstrInOut.txt", "GeneralOut.txt", "DatahazardOut.txt", "LwhazardOut.txt", "BranchbonusOut.txt"};
  const int *arrRegs[] = {(int*)arrBranchhazardReg, (int*)arrInstrInReg, (int*)arrGeneralReg, (int*)arrDatahazardReg, (int*)arrLwhazardReg, (int*)arrBranchbonusReg};
  const int *arrMems[] = {(int*)arrBranchhazardMem, (int*)arrInstrInMem, (int*)arrGeneralMem, (int*)arrDatahazardMem, (int*)arrLwhazardMem, (int*)arrBranchhazardMem};

  const int programNum = 6;

  // build env
  DataMemory mem;
  Register reg;
  CPU myCPU(&mem, &reg);
  Program *pP;

  for(int i = 0 ; i < programNum ; i++)
  {
    freopen(outputFileNames[i], "w", stdout);
    pP = new Program(inputFileNames[i]);
    myCPU.setProgram(pP);
    myCPU.execute();
    if(!myCPU.checkStatus(arrRegs[i], arrMems[i]))
      fprintf(stderr, "ERROR AT CHECK %s\n", outputFileNames[i]);
    //assert(myCPU.checkStatus(arrRegs[i], arrMems[i]));
    myCPU.reset();
  }
  // pP = new Program("InstrIn.txt");
  // pP2 = new Program("General.txt");
  // pP3 = new Program("Datahazard.txt");
  // pP = new Program("Datahazard.txt");
  // pP = new Program("Lwhazard.txt");


}


