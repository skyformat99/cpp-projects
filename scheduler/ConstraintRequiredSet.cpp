#define SEPARATOR "|"

#include <iostream>
#include <vector>

#include "Block.h"
#include "ConstraintRequiredSet.h"
#include "Resident.h"
#include "Rotation.h"
#include "ScheduleState.h"
#include "Utilities.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

ConstraintRequiredSet::ConstraintRequiredSet (ConstraintType type, 
    std::vector<std::string>& params) : Constraint (type)
{
  loadRequiredRotationSets(params);
}

ConstraintRequiredSet::~ConstraintRequiredSet ()
{
  int i;
  for (i=0; i<rotations->size(); i++)
    delete rotations->at(i);

  delete rotations;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

int ConstraintRequiredSet::getSetRotationCount(int i)
{
  return numChoices[i];
}

bool ConstraintRequiredSet::isValid (Resident* r, Block* b, Rotation* o, ScheduleState* state)
{
  return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void ConstraintRequiredSet::loadRequiredRotationSets (std::vector<std::string>& params)
{
  rotations = new vector<int*>;
  numChoices = new int[params.size()-2];

  vector<int> v; int* choice; int i, j;
  for (i=2; i<params.size(); i++) {  // start at 2 because 0, 1 were used in 'Constraint'
  
    v = Utilities::tokenizeToIntVector(params.at(i), SEPARATOR);

    numChoices[i-2] = v.size();

#ifdef DEBUG3 
    cout << "loadRotationSets: i: " << (i-2) << " numChoices: " << numChoices[i-2] << " str: " << params.at(i) << endl;
#endif

    choice = new int[ numChoices[i-2] ];

    for (j=0; j<numChoices[i-2]; j++)
      choice[j] = v[j];

    rotations->push_back(choice);
  }
}