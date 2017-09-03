#ifndef __Utilities_h__
#define __Utilities_h__

#include <string>
#include <vector>

class ScheduleState;
class ResidentPool;
class BlockPool;
class RotationPool;

using namespace std;

class Utilities
{
public:

  static vector<string> tokenizeToStringVector (string, string);
  static vector<int> tokenizeToIntVector (string, string);

  static void toResidentWebTable (ScheduleState*, ResidentPool*, BlockPool*, 
                  RotationPool*, string, bool append = false);

};

#endif