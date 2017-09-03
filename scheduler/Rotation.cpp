#include "Rotation.h"

#include <string>

Rotation::Rotation () 
{

}

Rotation::Rotation (int id, string n, bool b)
{
  this->name = n;
  this->isEmpty = b;
  this->id = id;
}

Rotation::~Rotation ()
{

}

Rotation& Rotation::operator= (Rotation const& r) 
{
  this->name = r.name;
  this->isEmpty = r.isEmpty;
  this->id = r.id;

  return *this;
}