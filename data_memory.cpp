#include "data_memory.h"

DataMemory::DataMemory()
{
    init();
}

DataMemory::DataMemory(const int* arr)
{
  for(int i=1;i<=5;i++)
    data[(i-1)*4] = arr[i-1];
}

DataMemory::~DataMemory()
{

}

void DataMemory::init()
{
    for(int i=1;i<=5;i++)
        data[(i-1)*4] = i;
}

bool DataMemory::compare(const int* arr)
{
  for(int i=1;i<=5;i++)
    if(data[(i-1)*4] != arr[i-1])
      return false;
  return true;
}

int DataMemory::read(int idx)
{
  return data[idx];
}

void DataMemory::write(int idx, int v)
{
  if(idx == 0)
    return;
  data[idx] = v;
}

void DataMemory::dump()
{
  printf("\nData memory:\n");
  for(int i=1;i<=5;i++)
  {
    printf("%02d:\t%d\n", (i-1)*4, data[(i-1)*4]);
  }
}
