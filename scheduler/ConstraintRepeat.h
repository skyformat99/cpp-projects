#ifndef __ConstraintRepeat_h__
#define __ConstraintRepeat_h__

#include <string>

#include "Constraint.h"

class ConstraintRepeat : Constraint
{

public:

  ConstraintRepeat (ConstraintType);
  ~ConstraintRepeat ();

  bool isValid (Resident*, Block*, Rotation*, ScheduleState*);

private:

};

#endif