#include "stdio.h"
#ifndef register_h
#define register_h
class Register
{
public:
    Register();
    Register(const int*);
    void init();
    int read(int);
    void write(int, int);
    void dump();
    bool compare(const int*);
private:
    int data[9];
};
#endif // register_h
