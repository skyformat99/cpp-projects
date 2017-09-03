#ifndef __BlockPool_h__
#define __BlockPool_h__

#include "Block.h"
#include "PoolParent.h"

using namespace std;

class BlockPool : public PoolParent
{

public:
  BlockPool (string);
  ~BlockPool ();

  Block* getBlock (int);
  void print ();

private:

  Block* blocks;

  void loadFromFile ();

};

#endif