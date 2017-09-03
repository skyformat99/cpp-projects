#ifndef __ConstraintRequiredSet_h__
#define __ConstraintRequiredSet_h__

#include <string>
#include <vector>

#include "Constraint.h"

class ConstraintRequiredSet : Constraint
{

public:

  ConstraintRequiredSet (ConstraintType, std::vector<std::string>&);
  ~ConstraintRequiredSet ();

  inline int getNumRequiredSets () { return rotations->size(); }

  inline int* getSetRotations (int i) { return rotations->at(i); }
  int getSetRotationCount (int);

  bool isValid (Resident*, Block*, Rotation*, ScheduleState*);

private:

  std::vector<int*>* rotations;
  int* numChoices;

  void loadRequiredRotationSets (std::vector<std::string>&);

};

#endif