#ifndef __ResidentPool_h__
#define __ResidentPool_h__

#include "Resident.h"
#include "PoolParent.h"

using namespace std;

class ResidentPool : public PoolParent
{

public:
  ResidentPool (string);
  ~ResidentPool ();

  Resident* getResident (int);
  void print ();

  Resident& operator[] (int);

private:

  Resident* residents;

  void loadFromFile ();

};

#endif