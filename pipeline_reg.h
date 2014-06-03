#ifndef pipeline_reg_h
#define pipeline_reg_h
#include <stdio.h>
#include "mycpu.h"
#include "control_signal.h"
#include "stdio.h"
#include "string.h"


//new 0 pre 1
//every pipeline register need to remember data before and after stages

/**
 * Base class of pipeline registers
 */
class PipelineReg
{
public:
  PipelineReg(int);
  ~PipelineReg();
  int getPre(int type);
  int getNew(int type);
  void setNew(int type, int);
  void setPre(int type, int);
  void update();
  ControlSignal* csPre;
  ControlSignal* csNew;
  void dump();
  bool isClear();
protected:
    // pipeline register dump helper
    class DumpHelper
    {
      public:
        const char* name;
        bool isDumpCs;
        const char **names;
        const int *indexs;
        int csWid;
        void setup(int args, const char**, const int*, int);
        void dump(PipelineReg*);
      private:
        int count;
    };
    virtual int getLast(){};
    int regLen;
    int** getData();
    int **data;
    int ignoreIdx;
    DumpHelper* pDumpHelper;
};

class IF_ID : public PipelineReg
{
public:
  IF_ID() : PipelineReg(last+1)
  {
    ignoreIdx = PC;
    pDumpHelper->name = "IF/ID";
    pDumpHelper->isDumpCs = false;
    static const char * const names[] = {"PC", "Instruction"};
    static const int indexs[] = {PC, INSTRUCTION};
    pDumpHelper->setup(2, (const char**)names, indexs, 0);
  };
  int stepPC();
  static const int INSTRUCTION = 0;
  static const int PC = 1;
  static const int last = PC;
  void stall();
};

class ID_EX : public PipelineReg
{
public:
  ID_EX() : PipelineReg(last+1)
  {
    ignoreIdx = PC;
    pDumpHelper->name = "ID/EX";
    pDumpHelper->isDumpCs = true;
    static const char * const names[] = {"ReadData1", "ReadData2", "sign_ext", "Rs", "Rt", "Rd"};
    static const int indexs[] = {Rd0, Rd1, signEXT, Rs, Rt, Rd};
    pDumpHelper->setup(6, 
      (const char**)names,
      indexs,
      9
    );
  };
  ~ID_EX() {};
  static const int Rd0 = 0;
  static const int Rd1 = 1;
  static const int signEXT = 2;
  static const int Rs = 3;
  static const int Rt = 4;
  static const int Rd = 5;
  static const int PC = 6;
  static const int INSTRUCTION = 7;
  static const int last = INSTRUCTION;
};

class EX_MEM : public PipelineReg
{
public:
  EX_MEM() : PipelineReg(last+1)
  {
    ignoreIdx = PC;
    pDumpHelper->name = "EX/MEM";
    pDumpHelper->isDumpCs = true;
    static const char *names[] = {"ALUout", "WriteData", "Rt"};
    static const int indexs[] = {ALUout, WriteData, Rt};
    pDumpHelper->setup(3, 
      (const char**)names,
      indexs,
      5
    );
  };
  ~EX_MEM() {};
  static const int ALUout = 0;
  static const int WriteData = 1;
  static const int Rt = 2;
  static const int PC = 3;
  static const int BranchToken = 4;
  static const int INSTRUCTION = 5;
  static const int last = INSTRUCTION;
};

class MEM_WB : public PipelineReg
{
public:
  MEM_WB() : PipelineReg(last+1)
  {
    pDumpHelper->name = "MEM/WB";
    pDumpHelper->isDumpCs = true;
    static const char *names[] = {"ReadData", "ALUout"};
    static const int indexs[] = {ReadData, ALUout};
    pDumpHelper->setup(2, 
      (const char**)names,
      indexs,
      2
    );
  };
  ~MEM_WB() {};
  static const int ReadData = 0;
  static const int ALUout = 1;
  static const int Dst = 2;
  static const int last = Dst;
};



#endif
