#ifndef __ConstraintRequired_h__
#define __ConstraintRequired_h__

#include <string>

#include "Constraint.h"

class ConstraintRequired : Constraint
{

public:

  ConstraintRequired (ConstraintType, std::string);
  ~ConstraintRequired ();

  bool isValid (Resident*, Block*, Rotation*, ScheduleState*);

//  ConstraintRequired& operator= (ConstraintRequired const&);

  inline int getNumRequired () { return numRequired; }
  int* getRequiredRotations ();

private:

  int* rotations;
  int numRequired;

  void loadRequiredRotations (std::string);

};

#endif