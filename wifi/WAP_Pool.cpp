#include "WAP_Pool.h"

#include <iostream>

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

WAP_Pool::WAP_Pool () 
{
  this->pool = new vector<WAP>();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void WAP_Pool::addWAP (WAP w)
{
  for (vector<WAP>::iterator it = this->pool->begin(); it != this->pool->end(); ++it) {
    if (w.equals(*it)) {
      (*it).update(w);
      return;
    }
  }

  this->pool->push_back(w);
}

bool WAP_Pool::contains (WAP& w)
{
  for (vector<WAP>::iterator it = this->pool->begin(); it != this->pool->end(); ++it)
    if (w.equals(*it)) return true;

  return false;
}

void WAP_Pool::print ()
{
  cout << "WAP Pool (N=" << this->pool->size() << ")" << endl;

  for (vector<WAP>::iterator it = this->pool->begin(); it != this->pool->end(); ++it) {
    cout << "  ";
    it->print();
    cout << endl;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *