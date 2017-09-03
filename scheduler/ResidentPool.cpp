#include <iostream>
#include <string>

#include "ResidentPool.h"
#include "PropertyFile.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

ResidentPool::ResidentPool (string fn)
{
  this->fileName = fn;

  loadFromFile();
}

ResidentPool::~ResidentPool ()
{
  delete[] this->residents;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Resident* ResidentPool::getResident (int i)
{
  if ((i < 0) || (i >= this->length))
    throw exception("Index out of bounds exception");

  return residents+i;
}

Resident& ResidentPool::operator[] (int i)
{
  return *(getResident(i));
}

void ResidentPool::print ()
{
  cout << "ResidentPool (count: " << this->length << "):" << endl;

  for (int i=0; i < this->length; i++)
    cout << "  " << this->residents[i] << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void ResidentPool::loadFromFile ()
{
  vector<Resident>* r = new vector<Resident>;
  PropertyFile* p = new PropertyFile(this->fileName,",");

  vector<string> v; int i;
  for (i=0; i<p->length(); i++) {
    v = p->tokenizeToVector(i);

    r->push_back(Resident(atoi(v[0].c_str()),v[1],v[2]));
  }

  this->length = r->size();
  this->residents = new Resident[this->length];

  i = 0;
  for (vector<Resident>::iterator it = r->begin() ; it != r->end(); ++it)
    this->residents[i++] = *it;

  delete p;
  delete r;
}

