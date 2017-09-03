#ifndef __RotationBlock_h__
#define __RotationBlock_h__

#include "Rotation.h"
#include "Block.h"

#include <string>
#include <climits>

using namespace std;

class RotationBlock
{

public:

  static const int NO_MIN = -1;
  static const int NO_MAX = -1;

  RotationBlock ();
  RotationBlock (int, Rotation*, Block*, int, int);
  ~RotationBlock ();

  friend std::ostream & operator<< (std::ostream & str, RotationBlock const & v) { 

    str << "[" << v.id << "]: Rotation: (" << *(v.r) << ") Block: (" 
        << *(v.b) << ") (" << v.min << ", " << v.max << ")";
    return str;
  }

  inline int getMin () { return (min == INT_MIN) ? NO_MIN : min; }
  inline int getMax () { return (max == INT_MAX) ? NO_MAX : max; }

  RotationBlock& RotationBlock::operator= (RotationBlock const&);

  bool matchesIDs (int rID, int bID);

  bool isWithinLimits (int, bool careAboutMin = true);

private:

  int id;

  Rotation* r;
  Block* b;

  int min;
  int max;

};

#endif