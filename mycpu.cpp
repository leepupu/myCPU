#include "mycpu.h"

const char* InstType::decodeType(int type)
{
  return *(decodeName+type);
}
