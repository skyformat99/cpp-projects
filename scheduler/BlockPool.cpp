#include "Block.h"
#include "BlockPool.h"
#include "PropertyFile.h"

#include <iostream>
#include <string>
#include <vector>

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

BlockPool::BlockPool (string fn)
{
  this->fileName = fn;

  loadFromFile();
}

BlockPool::~BlockPool ()
{
  delete[] this->blocks;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Block* BlockPool::getBlock (int i)
{
  if ((i < 0) || (i >= this->length))
    throw exception("Index out of bounds exception");

  return this->blocks+i;
}

void BlockPool::print ()
{
  cout << "BlockPool (count: " << this->length << "):" << endl;

  for (int i=0; i < this->length; i++)
    cout << "  " << this->blocks[i] << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void BlockPool::loadFromFile ()
{
  vector<Block>* b = new vector<Block>;
  PropertyFile* p = new PropertyFile(this->fileName,",");

  vector<string> v; int i;
  for (i=0; i<p->length(); i++) {
    v = p->tokenizeToVector(i);

    b->push_back(Block(atoi(v[0].c_str()),v[1],v[2]));
  }

  this->length = b->size();
  this->blocks = new Block[this->length];

  i = 0;
  for (vector<Block>::iterator it = b->begin() ; it != b->end(); ++it)
    this->blocks[i++] = *it;

  delete p;
  delete b;
}