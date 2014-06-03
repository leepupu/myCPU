#include "register.h"

Register::Register()
{
    init();
}

Register::Register(const int* arr)
{
  for(int i=0;i<=8;i++)
        data[i] = arr[i];
}

void Register::init()
{
    for(int i=0;i<=8;i++)
        data[i] = i;
    data[5] = 1;
}

bool Register::compare(const int* arr)
{
  for(int i=0;i<=8;i++)
    if(arr[i] != data[i])
      return false;
  return true;
}

int Register::read(int idx)
{
  return data[idx];
}

void Register::write(int idx, int v)
{
  if(idx == 0)
    return;
  data[idx] = v;
}

void Register::dump()
{
  printf("\nRegisters:\n");
  for(int i=0;i<=8;i++)
  {
    printf("$%d: %d", i, data[i]);
    if(i%3==2)
      printf("\n");
    else
      printf("\t");
  }
}
