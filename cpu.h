#include <iostream>
#include "program.h"
#include "pipeline_reg.h"
#include "data_memory.h"
#include "register.h"
#include "instruction.h"
#ifndef cpu_h
#define cpu_h

class CPU
{
public:
  CPU();
  CPU(DataMemory*, Register*);
  ~CPU();
  void setProgram(Program*);
  void reset();
  void execute();
  bool checkStatus(const int*, const int*);
private:
  void IF();
  void ID();
  void EX();
  void MEM();
  void WB();
  void cycle();
  int ALUCtrl(int, int, int);
  bool isFinish();

  IF_ID* if_id;
  ID_EX* id_ex;
  EX_MEM* ex_mem;
  MEM_WB* mem_wb;

  int PC;
  int prePC;

  Program *pProgram;
  DataMemory *pDataMemory;
  Register *pRegister;
  /* data */
};
#endif
