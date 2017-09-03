#define RANDOMIZE_RESIDENT

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <climits>

#include "BlockPool.h"
#include "Resident.h"
#include "RotationBlockPool.h"
#include "ResidentPool.h"
#include "Rotation.h"
#include "RotationPool.h"
#include "ScheduleState.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

ScheduleState::ScheduleState (ResidentPool* rp, BlockPool* bp, 
    RotationPool* op, RotationBlockPool* rbp)
{
  this->rp = rp;
  this->bp = bp;
  this->op = op;
  this->rbp = rbp;

  // Initiate the random number generator
  srand (time(NULL));

  int i;
  state = new int*[rp->length];
  for (i=0;i<rp->length;i++)
    state[i] = new int[bp->length];
  
  rotationBlockCount = new int*[op->length];
  for (i=0;i<op->length;i++)
    rotationBlockCount[i] = new int[bp->length];

  residentFilled = new int[rp->length]; // for each resident, # of filled blocks
  residentRequiredFilled = new int[rp->length]; // for each resident, # of required rotations filled

  residentRequiredSetDone = new bool*[rp->length];
  for (i=0; i<rp->length; i++)
    residentRequiredSetDone[i] = new bool[rp->getResident(i)->getNumRequiredSets()];

  reset();
}

ScheduleState::~ScheduleState ()
{
  int i;
  for (i=0;i<rp->length;i++)
    delete[] state[i];
  delete[] state;

  for (i=0;i<op->length;i++)
    delete[] rotationBlockCount[i];
  delete[] rotationBlockCount;

  delete[] residentFilled;
  delete[] residentRequiredFilled;

  for (i=0; i<rp->length; i++)
    delete[] residentRequiredSetDone[i];
  delete[] residentRequiredSetDone;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ScheduleState::areAllFilled ()
{
  int i, j;
  for (i=0;i<rp->length;i++) {
    for (j=0;j<bp->length;j++) {
      if (state[i][j] == RotationPool::EMPTY_INDEX) return false;
    }
  }

  return true;
}

bool ScheduleState::areRotationBlocksWithinLimits ()
{
  // Start rotation loop from 1, because all "OFF" rotations 
  // (at index == 0) are not limit-checked
  int i, j; bool r = true;
  for (i=1;i<op->length;i++) 
    for (j=0;j<bp->length;j++) 
      r &= rbp->getByIDs(i,j)->isWithinLimits(rotationBlockCount[i][j],true); 

  return r;
}

/** Adds x to the current rotationBlockCount and determines if it will
 *  be within limits **/
bool ScheduleState::areRotationBlocksWithinLimits (Resident* r, Block* b, Rotation* o, 
         int add, bool careAboutMin)
{
  int oID = o->getID();
  int bID = b->getID();

#ifdef DEBUG1
//  cout << "---- oID: " << oID << " bID: " << bID << endl;
#endif

  RotationBlock* rb = rbp->getByIDs(oID,bID);
//  if (rb == NULL) return false;

#ifdef DEBUG1
//  cout << "---- rbp->get(...): " << *rb << endl;

//  cout << "---- rbCount[" << oID << "][" << bID << "]: " 
//       << rotationBlockCount[oID][bID] << endl;
#endif
  
  bool e = rbp->getByIDs(o->getID(), b->getID())->isWithinLimits(
             rotationBlockCount[o->getID()][b->getID()]+add, careAboutMin);

#ifdef DEBUG1
  cout << "---- RotationBlockConstraints: " << ((e) ? "yes" : "no") << endl;
#endif

  return e;
}

void ScheduleState::assign (Resident* r, Block* b, Rotation* o)
{
  int rID = r->getID(), bID = b->getID(), oID = o->getID();
  int oldOID = state[rID][bID];

  state[rID][bID] = oID;

  rotationBlockCount[oID][bID]++;

  if (oldOID == RotationPool::EMPTY_INDEX) residentFilled[rID]++;
  
  if (r->isRequired(oID)) residentRequiredFilled[rID]++;
}

Rotation* ScheduleState::getFilledRotation (int rID, int bID)
{
  if (state[rID][bID] == RotationPool::EMPTY_INDEX)
    return NULL;

  return op->getRotation(state[rID][bID]);
}

/** 
 * Finds an empty block associated with the given resident. This method
 * does not care about required versus other blocks - it simpley randomly
 * grabs a free block for the given resident.
 **/
Block* ScheduleState::getResidentEmptyBlock (Resident* r)
{
  // If no free blocks are here, return NULL
  int numFree = bp->length - residentFilled[r->getID()];
  if (numFree == 0) return NULL;

  // Get a random index into the free blocks
  int blockInd = rand() % numFree;

#ifdef DEBUG1
  cout << "--- getResidentEmptyBlock: blockInd: " << blockInd << endl;
#endif

  int i, j = 0;
  for (i=0;i<bp->length;i++) {
    if (state[r->getID()][i] == RotationPool::EMPTY_INDEX) {
      if (j == blockInd) {
        return bp->getBlock(i);
      }

      j++;
    }
  }

  cout << "Error: could not find random block!\n" 
       << " blockInd: " << blockInd 
       << " residentFilled: " << residentFilled[r->getID()] << endl;

  throw exception("Could not find random block to fill");

}

/**
 * Finds an empty block associated with the given resident. Does this
 * using the method described inside - looking at minimum number
 * of required spaces and returning block with least # available.
 **/ 
Block* ScheduleState::getResidentEmptyBlockSpecial (Resident* r)
{
  // If no free blocks are here, return NULL
  int numFree = bp->length - residentFilled[r->getID()];
  if (numFree == 0) return NULL;
   
  // Here we determine, of the free blocks, how many have space in the
  // minimum. An example:
  //   50 residents total
  //   5 req rotations + WBN + ICN + E rotation = min of 32 residents
  //     Remainder of residents are on: PCR, Adv, ED, Elective
  //
  // As we are picking a block, we look for emptyu blocks with the least amount
  // of free space on their rotations-with-minimums first - so these fail first:
  //   
  //   spaceInReqBlocks = blockMinimum - sum across RotationsWithMinimums ( # filled )
  // 
  // This could be done by randomizing across all blocks with least amount of space,
  // or we could just pick one.
  int i, j, k;
  
  int* rotsWithMins;
  int* sumRotsWithMinFilled = new int[bp->length]; 
  for (i=0; i<bp->length; i++) {
    sumRotsWithMinFilled[i] = 0;

    rotsWithMins = rbp->getRotationIndicesWithMinimums(i);
    for (j=0; j<rbp->getCountRotationsWithMinimums(i); j++) {
      for (k=0; k<rp->length; k++) {
        if (state[k][i] == rotsWithMins[j]) sumRotsWithMinFilled[i]++;
      }
    }

    cout << "---- i: " << i << " blockMin: " << rbp->getBlockMinimum(i) 
        << " sumRotsWithMinFilled[i]: " << sumRotsWithMinFilled[i] << endl;
  }

  int val, minVal = INT_MAX, minInd = -1, countAtMin = 0;
  for (i=0; i<bp->length; i++) {
    if (state[r->getID()][i] != RotationPool::EMPTY_INDEX) continue;

    val = rbp->getBlockMinimum(i) - sumRotsWithMinFilled[i];    

    if (val == minVal) countAtMin++;

    if (val < minVal) {
      minVal = val;
      minInd = i;
      countAtMin = 1;
    }
  }

#ifdef DEBUG1
  cout << "--- minBlock: minVal: " << minVal << " countAtMin: " << countAtMin << endl;
#endif

  // No empty blocks
  if (minInd == -1) { 
    delete sumRotsWithMinFilled;
    
    return NULL;
  }

  // Just 1 empty block that meets criteria ...
  if (countAtMin == 1) {
    delete sumRotsWithMinFilled;

    return bp->getBlock(minInd);
  }

  int indexIntoMinVals = rand() % countAtMin + 1;
  int blockToReturn = -1;

  minVal = INT_MAX; countAtMin = 0;
  for (i=0; i<bp->length; i++) {
    if (state[r->getID()][i] != RotationPool::EMPTY_INDEX) continue;

    val = rbp->getBlockMinimum(i) - sumRotsWithMinFilled[i];    

    if (val == minVal) countAtMin++;

    if (indexIntoMinVals == countAtMin) blockToReturn = i;;

    if (val < minVal) {
      minVal = val;
      countAtMin = 1;
      if (indexIntoMinVals == countAtMin) blockToReturn = i;
    }
  }

#ifdef DEBUG1
  cout << "--- minBlock: blockToReturn: " << blockToReturn << endl;
#endif

  delete sumRotsWithMinFilled;

  if (blockToReturn != -1)
    return bp->getBlock(blockToReturn);
  else
    return NULL;
}

/** 
  * Finds the resident with most # req'd blocks, or NULL if none req'd 
  *
  * PHP version: counted # of empty spaces per resident into an array. 
  * If only 1 resident with the min # of empty spaces, returned this. 
  * Otherwise returned a random resident in that set (with min spaces).
  *
  * C++ version: find first resident with most # req blocks remainning. 
  * Return NULL if none have blocks remaining
  **/
Resident* ScheduleState::getNextRequiredResident ()
{
  // If we've already determined no more required, skip the test!
  if (noMoreRequired) return NULL;

  // Loop through residents and find numReq - numReqFilled
  int i, val, minVal = INT_MAX, minInd = -1, numAtMin = -1;
  int numReq, numFilled;
  for (i=0; i<rp->length; i++) {

    numReq = rp->getResident(i)->getNumRequired();
    numFilled = residentRequiredFilled[i];
    val = numReq - numFilled;

#ifdef DEBUG3
    cout << "---- numReq: " << numReq << " numFilled: " << numFilled 
         << " val: " << val << " numAtMin: " << numAtMin << endl;
#endif

    if (val == minVal && val > 0)
      numAtMin++;

    if (val < minVal && val > 0) {
      numAtMin = 1;
      minVal = val;
      minInd = i;
    }
  }

#ifdef DEBUG3
  cout << "--- getNextReqRes: minVal: " << minVal << " minInd: " << minInd << endl;
#endif

  // If we didn't find any, set the var to pass this method in the future
  // and return
  if (minInd < 0) {
    noMoreRequired = true;
    return NULL;
  }

#ifdef RANDOMIZE_RESIDENT

  if (numAtMin == 1)
    return rp->getResident(minInd);

  // Get index into list of all residents at minimum, as
  // this will be the resident we grab
  int randomResIndex = (rand() % numAtMin) + 1;

  minVal = INT_MAX; minInd = -1; numAtMin = -1;
  for (i=0; i<rp->length; i++) {

    val = rp->getResident(i)->getNumRequired() - residentRequiredFilled[i];

    if (val == minVal && val > 0) {
      numAtMin++;
      
      if (numAtMin == randomResIndex) 
        return rp->getResident(i);
    }

    if (val < minVal && val > 0) {
      numAtMin = 1;
      minVal = val;
      minInd = i;

      if (randomResIndex == numAtMin) 
        return rp->getResident(i);
    }
  }

  cout << "--- randomResIndex: " << randomResIndex 
       << " numAtMin: " << numAtMin
       << " minInd: " << minInd << " minVal: " << minVal << endl;

  throw exception("Error randomizing resident list");

#else

  return rp->getResident(minInd);

#endif

}

/** 
 * Finds the resident with the most # of blocks empty.
 *
 * Runs in 2*O(N) time - requires a (partial) second look if
 * RANDOMIZE_RESIDENT is defined, as this will randomize
 * the resident with the least number of space.
 **/
Resident* ScheduleState::getNextResident ()
{
  int i, val, minVal = INT_MAX, minInd = -1, numAtMin = -1;
  for (i=0;i<rp->length; i++) {
    val = bp->length - residentFilled[i];

    if (val == minVal && val > 0)
      numAtMin++;

    if (val < minVal && val > 0) {
      numAtMin = 1;
      minVal = val;
      minInd = i;
    }
  }

  // If we didn't find any, return NULL
  if (minInd < 0)
    return NULL;

#ifdef RANDOMIZE_RESIDENT

  if (numAtMin == 1)
    return rp->getResident(minInd);

  // Get index into list of all residents at minimum, as
  // this will be the resident we grab
  int randomResIndex = (rand() % numAtMin) + 1;

  minVal = INT_MAX; minInd = -1; numAtMin = -1;
  for (i=0; i<rp->length; i++) {
    val = bp->length - residentFilled[i];

    if (val == minVal && val > 0) {
      numAtMin++;
      
      if (numAtMin == randomResIndex) 
        return rp->getResident(i);
    }

    if (val < minVal && val > 0) {
      numAtMin = 1;
      minVal = val;
      minInd = i;

      if (randomResIndex == numAtMin) 
        return rp->getResident(i);
    }
  }

  cout << "--- randomResIndex: " << randomResIndex 
       << " numAtMin: " << numAtMin
       << " minInd: " << minInd << " minVal: " << minVal << endl;

  throw exception("Error randomizing resident list");

#else

  return rp->getResident(minInd);

#endif

}

/** Determine if a resident (rID) is alredy assigned a 
 *  particular rotation **/
bool ScheduleState::isResidentAssigned (int rID, int oID) 
{
  int j;
  for (j=0;j<bp->length;j++) {
    if (state[rID][j] == oID) return true;
  }

  return false;
}

bool ScheduleState::isResidentRequiredSetDone (Resident* r, int rs) 
{ 
  return residentRequiredSetDone[r->getID()][rs];
}

bool ScheduleState::isRotationSurroundingBlock (int rID, int bID, int oID)
{
  if (bID > 0) {
    if (state[rID][bID-1] == oID) return false;
  }

  if (bID < bp->length - 1) {
    if (state[rID][bID+1] == oID) return false;
  }

  return true;
}

void ScheduleState::print ()
{
  cout << "ScheduleState" << endl;

  int i, j;
  for (i=0; i<rp->length; i++) {
    cout << "  " << rp->getResident(i)->shortString() << ": ";

    for (j=0; j<bp->length; j++) {
      cout << "[" << j << "] " << op->getRotation(state[i][j])->getName();

      if (j < bp->length-1) cout << ",";
    }

    cout << endl;
  }
}

void ScheduleState::printByBlockRotation ()
{
  cout << "ScheduleState" << endl;

  int i, j, unAssigned;  
  Block* b; Rotation* o; RotationBlock* rb;
  for (i=0; i<bp->length; i++) {
    b = bp->getBlock(i);
    cout << "Block " << i << " (" << b->getStartDate() 
         << " - " << b->getEndDate() << ")";

    unAssigned = 0;
    for (j=0; j<rp->length; j++)
      if (state[j][i] == RotationPool::EMPTY_INDEX) unAssigned++;

    cout << " unAssigned: " << unAssigned << endl;

    for (j=0; j<op->length; j++) {
      o = op->getRotation(j);
      cout << "  [" << j << "] " << o->getName();

      rb = rbp->getByIDs(j,i);
      if (rb == NULL)
        cout << " (no min / no max)";
      else
        cout << " (min: " << rb->getMin() << ", max: " << rb->getMax() << ")";
      
      cout << " # Residents: " << rotationBlockCount[j][i] << endl;
    }
  }
}    

void ScheduleState::randomFill ()
{
  // Fill the state randomly
  int i, j;
  for (i=0;i<rp->length;i++) {
    for (j=0;j<bp->length;j++) {
      state[i][j] = rand() % (op->length-1) + 1;
      rotationBlockCount[state[i][j]][j]++;
    }
  }
} 

void ScheduleState::reset ()
{
  int i, j;
  for (j=0;j<bp->length;j++) {
    for (i=0;i<rp->length;i++)
      state[i][j] = RotationPool::EMPTY_INDEX;

    for (i=0;i<op->length;i++)
      rotationBlockCount[i][j] = 0;
  }

  for (i=0;i<rp->length;i++) {
    residentFilled[i] = 0;
    residentRequiredFilled[i] = 0;
  }

  noMoreRequired = false;

  int numReqSet;
  for (i=0; i<rp->length; i++) {
    numReqSet = rp->getResident(i)->getNumRequiredSets();

    for (j=0; j<numReqSet; j++) {
      residentRequiredSetDone[i][j] = false;
    }
  }
}

void ScheduleState::setResidentRequiredSetDone (Resident* r, int rs, bool b)
{
  residentRequiredSetDone[r->getID()][rs] = b;
}

void ScheduleState::unassign (Resident* r, Block* b, Rotation* o)
{
  int rID = r->getID(), bID = b->getID(), oID = o->getID();

  state[rID][bID] = RotationPool::EMPTY_INDEX;

  rotationBlockCount[oID][bID]--;

  residentFilled[rID]--;
  
  if (r->isRequired(oID)) {
    noMoreRequired = false;  // now there's one required again!
    residentRequiredFilled[rID]--;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

