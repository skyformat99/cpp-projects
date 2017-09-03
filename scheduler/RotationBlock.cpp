#include "RotationBlock.h"

#include <string>
#include <climits>

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

RotationBlock::RotationBlock () 
{
  // Do nothing in default constructor
}

RotationBlock::RotationBlock (int id, Rotation* r, Block* b, int min, int max)
{
  this->id = id;

  this->r = r;
  this->b = b;

  this->min = (min == RotationBlock::NO_MIN) ? INT_MIN : min;
  this->max = (max == RotationBlock::NO_MAX) ? INT_MAX : max;
}

RotationBlock::~RotationBlock ()
{
  // Nothing to destruct
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

RotationBlock& RotationBlock::operator= (RotationBlock const& r) 
{
  this->id = r.id;

  this->r = r.r;
  this->b = r.b;

  this->min = r.min;
  this->max = r.max;

  return *this;
}

bool RotationBlock::isWithinLimits (int count, bool careAboutMin)
{
  return (careAboutMin) ? (count >= min && count <= max) : (count <= max);
}

bool RotationBlock::matchesIDs (int rID, int bID)
{
  return (r->getID() == rID && b->getID() == bID);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *