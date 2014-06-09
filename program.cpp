#include "program.h"
#include "mycpu.h"

using namespace std;

Program::Program(const char *file_name)
{
  ifstream fin(file_name);
  char ins[50] ={0};
  while(fin >> ins)
  {
    unsigned int new_ins = 0;
    for(int i=0;i<32;i++)
      new_ins |= (1 & (ins[i] - '0')) << (31 - i);
    if(DEBUG)
      printf("decode instruction: %u, at line: %lu\n", new_ins, instructions.size());
    instructions.push_back(new_ins);
  }
  pInstruction = instructions.begin();
  istCount = instructions.size();
  fin.close();
}

Program::~Program()
{

}

int Program::getInstructionsCount()
{
    return istCount;
}

Instruction* Program::next()
{
  Instruction *pI;
  if(pInstruction == instructions.end())
    pI = new Instruction(0);
  pI = new Instruction(*(pInstruction++));
  return pI;
}

Instruction* Program::nextAt(int n)
{
  Instruction *pI;
  if(n >= istCount)
    pI = new Instruction(0); // NOP
  else
    pI = new Instruction(*(instructions.begin()+n));
  return pI;
}

void Program::reset(void)
{

}
