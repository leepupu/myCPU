#include "instruction.h"

Instruction::Instruction(int byte_code)
{
    inst = byte_code;
}

int Instruction::getType()
{

    int op = getOp();
    if(DEBUG)
    {
        printf("Inst::getType, op = %d\n", op);
    };
    if(op == 0)
        return InstType::RType;
    else if(op == 35)
        return InstType::lwType;
    else if(op == 4) // beq, bne
        return InstType::brType;
    else if(op == 5)
        return InstType::brneType;
    else if(op == 43)
        return InstType::swType;
    else
        return InstType::UnknowType;
}

int Instruction::getRaw()
{
    return inst;
}

int Instruction::getOp()
{
    if(DEBUG)
    {
        printf("getOp\n");
        int num = (unsigned)(inst & (-1 - ((1 << 26)-1))) >> 26;
        for(int i=31;i>=0;i--)
          if(num & (1 << i))
            printf("1");
          else
            printf("0");
        printf("\n");
    }
    return (unsigned)(inst & (-1 - ((1 << 26)-1))) >> 26;
}

int Instruction::getRs()
{
    if(DEBUG)
    {
        printf("getRs\n");
        int num = (unsigned)(  inst & ( ( (1 << 26) - 1 ) - ( (1 << 21) - 1 ) )  ) >> 21;
        for(int i=4;i>=0;i--)
          if(num & (1 << i))
            printf("1");
          else
            printf("0");
        printf("\n");
    }
    return (unsigned)(  inst & ( ( (1 << 26) - 1 ) - ( (1 << 21) - 1 ) )  ) >> 21;
}

int Instruction::getRt()
{
    if(DEBUG)
    {
        printf("getRt\n");
        int num = (unsigned)(  inst & ( ( (1 << 21) - 1 ) - ( (1 << 16 ) - 1 ) )  ) >> 16;
        for(int i=4;i>=0;i--)
          if(num & (1 << i))
            printf("1");
          else
            printf("0");
        printf("\n");
    }
    return (unsigned)(  inst & ( ( (1 << 21) - 1 ) - ( (1 << 16 ) - 1 ) )  ) >> 16;
}

int Instruction::getRd()
{
    if(DEBUG)
    {
        printf("getRd\n");
        int num = (unsigned)(  inst & ( ( (1 << 16) - 1 ) - ( (1 << 11 ) - 1 ) )  ) >> 11;
        for(int i=4;i>=0;i--)
          if(num & (1 << i))
            printf("1");
          else
            printf("0");
        printf("\n");
    }
    return (  inst & (unsigned)( ( (1 << 16) - 1 ) - ( (1 << 11 ) - 1 ) )  ) >> 11;
}

int Instruction::getFunct()
{
    return (  inst & ( (1 << 6) - 1 ) );
}

int Instruction::getITypeImmediate()
{
    return ( inst & ( (1 << 16) - 1 ) );
}

void Instruction::clear()
{
    inst = 0;
}





