#ifndef __RotationBlockPool_h__
#define __RotationBlockPool_h__

#include "RotationBlock.h"
#include "RotationPool.h"
#include "BlockPool.h"
#include "PoolParent.h"

using namespace std;

class RotationBlockPool : public PoolParent
{

public:
  RotationBlockPool (string, RotationPool*, BlockPool*);
  ~RotationBlockPool ();

  bool areMinMaxPossible (int);
  inline RotationBlock* getByIDs (int rID, int bID) { return rb2d[rID][bID]; }

  inline int getBlockMinimum (int i) { return blockMinimums[i]; }
  inline int getCountRotationsWithMinimums (int i) { return countRotationsWithMins[i]; }
  inline int* getRotationIndicesWithMinimums (int i) { return rotationIndicesWithMinimums[i]; }

  void print ();

private:

  RotationBlock* rbs;
  RotationBlock*** rb2d;

  RotationPool* rotationPool;
  BlockPool* blockPool;

  RotationBlock* findMatchByID (int rID, int bID);
  void loadFromFile ();

  /** Minimum number of residents required for each block **/
  int* blockMinimums;
  /** 2D array - blocks x rotations with minimums - index of rotations with mins **/
  int** rotationIndicesWithMinimums;
  int* countRotationsWithMins;

};

#endif