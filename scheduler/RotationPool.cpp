#include "Rotation.h"
#include "RotationPool.h"
#include "PropertyFile.h"

#include <iostream>
#include <string>
#include <vector>

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

RotationPool::RotationPool (string fn)
{
  this->fileName = fn;
  this->emptyRotation = new Rotation(this->EMPTY_INDEX,"EMPTY",true);

  loadFromFile();
}

RotationPool::~RotationPool ()
{
  delete[] this->rotations;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Rotation* RotationPool::getRotation (int i)
{
  if (i == EMPTY_INDEX) return this->emptyRotation;

  if ((i < 0) || (i >= this->length))
    throw exception("Index out of bounds exception");

  return this->rotations+i;
}

void RotationPool::print ()
{
  cout << "RotationPool (count: " << this->length << "):" << endl;

  for (int i=0; i < this->length; i++)
    cout << "  " << this->rotations[i] << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void RotationPool::loadFromFile ()
{
  vector<Rotation>* r = new vector<Rotation>;
  PropertyFile* p = new PropertyFile(this->fileName,",");

//  r->push_back(Rotation(RotationPool::EMPTY_INDEX,"EMPTY",true));
  r->push_back(Rotation(0,"OFF",false));

  vector<string> v; int i;
  for (i=0; i<p->length(); i++) {
    v = p->tokenizeToVector(i);

    r->push_back(Rotation(atoi(v[0].c_str()),v[1],false));
  }

  this->length = r->size();
  this->rotations = new Rotation[this->length];

  i = 0;
  for (vector<Rotation>::iterator it = r->begin() ; it != r->end(); ++it)
    this->rotations[i++] = *it;

  delete p;
  delete r;
}

