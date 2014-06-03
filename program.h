#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include "instruction.h"
#ifndef program_h
#define program_h
/**
 *  Handle File Input and transform to int32 instructions
 */

class Program
{
public:
  Program();
  Program(const char*);
  ~Program();
  int getInstructionsCount();
  void reset();
  Instruction* next();
  Instruction* nextAt(int);

private:
  std::vector<int> instructions;
  std::vector<int>::iterator pInstruction; // alwarys point to inst which not exec yet
  int istCount;
  /* data */
};
#endif
