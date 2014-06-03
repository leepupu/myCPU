#include "mycpu.h"
#include "stdio.h"
#ifndef instruction_h
#define instruction_h

using namespace InstType;

class Instruction
{
public:
    Instruction(int);
    ~Instruction() {};
    // TODO: using namespace
    // static const int RType = 0;
    // static const int lwType = 1;
    // static const int brType = 2;
    // static const int swType = 3;
    // static const int brneType = 4;
    // static const int UnknowType = 5;

    int getType();
    int getOp();
    int getRs();
    int getRt();
    int getRd();
    int getShamt();
    int getFunct();
    int getITypeImmediate();
    int getJTypeImmediate();
    int getRaw();
    void clear();
private:
    int inst;
};

#endif
