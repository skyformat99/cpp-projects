#include "RotationBlockPool.h"
#include "PropertyFile.h"

#include <iostream>
#include <string>
#include <vector>
#include <climits>

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

RotationBlockPool::RotationBlockPool (string fn, RotationPool* rp, BlockPool* bp)
{
  this->fileName = fn;

  this->rotationPool = rp;
  this->blockPool = bp;

  loadFromFile();
}

RotationBlockPool::~RotationBlockPool()
{
  delete[] rbs;
  
  int i;
  for (i=0;i<rotationPool->length;i++)
    delete[] rb2d[i];
  delete[] rb2d;

  delete[] blockMinimums;
  delete[] countRotationsWithMins;

  for (i=0; i<blockPool->length; i++)
    delete[] rotationIndicesWithMinimums[i];
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

bool RotationBlockPool::areMinMaxPossible (int numResidents)
{
  bool r = true;
  int i, j, min, max, minSum, maxSum;

  // across each block ... are there too many residents for each max? 
  // (this is not true as long as there is a max == -1)
  for (i=0; i<blockPool->length; i++) {
    maxSum = 0;

    for (j=1; j<rotationPool->length; j++) { // start from 1 for "OFF" block
      max = rb2d[j][i]->getMax();

      if (max == RotationBlock::NO_MAX) {
        maxSum = INT_MAX;
        break;
      }

      maxSum += max;
    }

    if (numResidents > maxSum) {
      cout << "Block " << i << " numResidents (" << numResidents 
           << ") exceed max spaces (" << maxSum << ")!" << endl;
      r = false;
    }
  }

  // across each block ... are there too many residents for each min?
  for (i=0; i<blockPool->length; i++) {
    minSum = 0; 

    for (j=1; j<rotationPool->length; j++) { // start from 1 for "OFF" block
      min = rb2d[j][i]->getMin();

      if (min != RotationBlock::NO_MIN)
        minSum += min;
    }

    if (numResidents < minSum) {
      cout << "Block " << i << " minSum (" << minSum
           << ") exceeds numResidents (" << numResidents << ")!" << endl;
      r = false;
    }
  }

  return r;
}

/** Called when running, requires only a lookup 
RotationBlock* RotationBlockPool::getByIDs (int rID, int bID)
{
  return rb2d[rID][bID];
} **/

void RotationBlockPool::print ()
{
  cout << "RotationBlockPool (count: " << length << "):" << endl;

  for (int i=0; i < length; i++)
    cout << "  " << rbs[i] << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

/** Called when initializing, requires O(n) search **/
RotationBlock* RotationBlockPool::findMatchByID (int rID, int bID)
{
  int i;
  for (i=0; i<length; i++) {
    if (rbs[i].matchesIDs(rID,bID)) return rbs+i;
  }

  return NULL;
}

void RotationBlockPool::loadFromFile ()
{
  vector<RotationBlock>* r = new vector<RotationBlock>;
  PropertyFile* p = new PropertyFile(fileName,",");

  int i, j;
  vector<string> v; 
  for (i=0; i<p->length(); i++) {
    v = p->tokenizeToVector(i);

    r->push_back(RotationBlock(i,
         rotationPool->getRotation(atoi(v[1].c_str())),
         blockPool->getBlock(atoi(v[0].c_str())),
         atoi(v[2].c_str()),atoi(v[3].c_str())));
  }

  length = r->size();
  rbs = new RotationBlock[length];

  i = 0;
  for (vector<RotationBlock>::iterator it = r->begin() ; it != r->end(); ++it)
    rbs[i++] = *it;

  delete p;
  delete r;

  /** Now fill 2D array **/
  rb2d = new RotationBlock**[rotationPool->length];
  for (i=0;i<rotationPool->length;i++)
    rb2d[i] = new RotationBlock*[blockPool->length]; 

  for (i=0; i<rotationPool->length; i++)
    for (j=0; j<blockPool->length; j++)
      rb2d[i][j] = findMatchByID(i,j);

  /** Lastly deal with block Minimums **/
  blockMinimums = new int[blockPool->length]; 
  countRotationsWithMins = new int[blockPool->length];
  rotationIndicesWithMinimums = new int*[blockPool->length];
  int numWithMins, index, bMin;

  for (i=0; i<blockPool->length; i++) {
    countRotationsWithMins[i] = 0;
    blockMinimums[i] = 0;

    for (j=1; j<rotationPool->length; j++) {
      if ((bMin = rb2d[j][i]->getMin()) != RotationBlock::NO_MIN) {
        countRotationsWithMins[i]++;
        blockMinimums[i] += bMin;
      }
    }

    rotationIndicesWithMinimums[i] = new int[countRotationsWithMins[i]];
    index = 0;

    for (j=1; j<rotationPool->length; j++) {
      if (rb2d[j][i]->getMin() != RotationBlock::NO_MIN)
        rotationIndicesWithMinimums[i][index++] = j;
    }
  }
}