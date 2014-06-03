#include "stdio.h"
#ifndef data_memory_h
#define data_memory_h
class DataMemory
{
public:
  DataMemory();
  DataMemory(const int*);
  ~DataMemory();
  void init();
  int read(int);
  void write(int, int);
  void dump();
  bool compare(const int*);

private:
    int data[20];
};
#endif
