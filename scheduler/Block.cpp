#include "Block.h"

#include <string>

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Block::Block () 
{
  // Default constructor
}

Block::Block (int id, string sd, string ed)
{
  this->startDate = sd;
  this->endDate = ed;
  this->id = id;
}

Block::~Block ()
{

}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Block& Block::operator= (Block const& r) 
{
  this->startDate = r.startDate;
  this->endDate = r.endDate;
  this->id = r.id;

  return *this;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *