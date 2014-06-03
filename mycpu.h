#define DEBUG 0
#ifndef mycpu_h
#define mycpu_h

namespace InstType
{
  static const int RType = 0;
  static const int lwType = 1;
  static const int brType = 2;
  static const int swType = 3;
  static const int brneType = 4;
  static const int UnknowType = 5;
  static const char *decodeName[] = {"RType", "lwType", "swType", "branchType", "brneType", "UnknowType"};
  const char* decodeType(int);
}

#endif
