#include "mycpu.h"
#include <stdio.h>
#ifndef control_signal_h
#define control_signal_h

class ControlSignal
{
public:
  // static const int RType = 0;
  // static const int lwType = 1;
  // static const int brType = 2;
  // static const int swType = 3;
  // static const int brneType = 4;
  // static const int UnknowType = 5;

  static const int Mem2Reg = 0;
  static const int RegWrite = 1;
  static const int MemWrite = 2;
  static const int MemRead = 3;
  static const int Branch = 4;
  static const int ALUSrc = 5;
  static const int ALUOp0 = 6;
  static const int ALUOp1 = 7;
  static const int RegDst = 8;
  static const int BranchNE = 9;

  ControlSignal() : csignal(0) {};
  ~ControlSignal() {};
  int mask(int);
  int getSignal();
  void setSignal(int);
  void setType(int);
  bool getBitSignal(int);
  void clear();
  void copy(ControlSignal*);
  void dump(int);
private:
  int csignal;

  /* data */
};
#endif
