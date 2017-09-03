#define SEPARATOR "&"

#include <iostream>

#include "ConstraintRequired.h"
#include "Utilities.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

ConstraintRequired::ConstraintRequired (ConstraintType type, string val) : Constraint (type)
{
  loadRequiredRotations (val);
}

ConstraintRequired::~ConstraintRequired ()
{
  delete[] rotations;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ConstraintRequired::isValid (Resident* r, Block* b, Rotation* o, ScheduleState* state)
{
  return true;
}

int* ConstraintRequired::getRequiredRotations () 
{
  return rotations;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void ConstraintRequired::loadRequiredRotations (string s)
{
  vector<int> v = Utilities::tokenizeToIntVector (s, SEPARATOR);

  numRequired = v.size();

  rotations = new int[numRequired];
  for (int i=0; i<numRequired; i++) 
    rotations[i] = v[i];
}