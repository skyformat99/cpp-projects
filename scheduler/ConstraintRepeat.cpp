#include "Block.h"
#include "ConstraintRepeat.h"
#include "Resident.h"
#include "Rotation.h"
#include "ScheduleState.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

ConstraintRepeat::ConstraintRepeat (ConstraintType type) : Constraint (type)
{
  // Construct
}

ConstraintRepeat::~ConstraintRepeat()
{
  // Destruct
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * Ensure that for this resident, rotationID does not fall before
 * or after the current blockID
 */
bool ConstraintRepeat::isValid (Resident* r, Block* b, Rotation* o, ScheduleState* state)
{
  return state->isRotationSurroundingBlock (r->getID(), o->getID(), b->getID());
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *