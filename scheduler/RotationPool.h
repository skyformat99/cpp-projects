#ifndef __RotationPool_h__
#define __RotationPool_h__

#include "Rotation.h"
#include "PoolParent.h"

using namespace std;

class RotationPool : public PoolParent
{

public:
  RotationPool (string);
  ~RotationPool ();

  Rotation* getRotation (int);
  void print ();

  static const int EMPTY_INDEX = -1;

private:

  Rotation* emptyRotation;
  Rotation* rotations;

  void loadFromFile ();

};

#endif