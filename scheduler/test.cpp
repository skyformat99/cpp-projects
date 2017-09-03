//#define _SMALL_DATA_
#define _DISPLAY_
#define _OUT_REDIRECT_

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

#include "Constraint.h"
#include "ResidentPool.h"
#include "BlockPool.h"
#include "Rotation.h"
#include "RotationPool.h"
#include "RotationBlockPool.h"
#include "ScheduleState.h"
#include "Utilities.h"

ScheduleState* recurseRequired (RotationPool*, ScheduleState*);

int main(int argc, char* argv[])
{

#ifdef _OUT_REDIRECT_
  std::ofstream out("out.txt");
  std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
  std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
#endif

#ifdef _SMALL_DATA_
  const char* DATA_RES   = "data/res_3.txt";
  const char* DATA_CONST = "data/const_3.txt";
  const char* DATA_BLOCK = "data/blocks_3.txt";
  const char* DATA_ROT   = "data/rots_3.txt";
  const char* DATA_RB    = "data/rb_3.txt";
  const char* DATA_OUT   = "output.html";
#else
  const char* DATA_RES   = "data/res.txt";
  const char* DATA_CONST = "data/const.txt";
  const char* DATA_BLOCK = "data/blocks.txt";
  const char* DATA_ROT   = "data/rots.txt";
  const char* DATA_RB    = "data/rb.txt";
  const char* DATA_OUT   = "output.html";
#endif

  ResidentPool* rp = new ResidentPool(DATA_RES);
  Constraint::loadFromFile(DATA_CONST,rp);
#ifdef _DISPLAY_  
  rp->print();
#endif

  // rp->getResident(3)->testConstraints();

  BlockPool* bp = new BlockPool(DATA_BLOCK);
#ifdef _DISPLAY_
  bp->print();
#endif

  RotationPool* op = new RotationPool(DATA_ROT);
#ifdef _DISPLAY_
  op->print();
#endif

  RotationBlockPool* rbp = new RotationBlockPool(DATA_RB,op,bp);
  if (! rbp->areMinMaxPossible(rp->length)) {
    cout << "Min / Max are not possible with RotationBlockPool given" << endl;
    throw exception();
  }
#ifdef _DISPLAY_
  rbp->print();
#endif

  ScheduleState* state = new ScheduleState(rp,bp,op,rbp);

  static int numRecurseCalls = 0;
  ScheduleState* result;
  double duration;  
  clock_t start = clock();
  
  result = recurseRequired(op,state);

  duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;

  cerr << "Duration of search: " << duration << " sec" << endl;

#ifdef _DISPLAY_
  if (state != NULL)
    state->print();
  else
    cout << "Resulting State is NULL!" << endl;
#endif

  state->printByBlockRotation ();

  // Write web table
  Utilities::toResidentWebTable(state, rp, bp, op, "output.html");

  delete state;
  delete rbp;
  delete op;
  delete bp;
  delete rp;

#ifdef _OUT_REDIRECT_
  std::cout.rdbuf(coutbuf); //reset to standard output again
#endif
}

/**
 * Recursive function to find the optimal state given the constraints.
 **/
ScheduleState* recurseRequired (RotationPool* op, ScheduleState* state)
{
  static int numRecurseCalls;
  numRecurseCalls++; 

#if defined DEBUG1 || DEBUG4
  if (numRecurseCalls % 100000 == 0) {
    state->print();
    state->printByBlockRotation ();
  }
#endif

  Resident* minRes = state->getNextRequiredResident();
  bool areRequiredFinished = false;
  // If no more required rotations ... get the next "required" resident
  if (minRes == NULL) {
#ifdef DEBUG1
    cout << "-- getNextReqResident returned NULL - call getNextResident" << endl;
#endif
    
    areRequiredFinished = true;
    minRes = state->getNextResident();

    if (minRes == NULL) {
#ifdef DEBUG1
      cout << "-- getNextResident returned NULL - return state!" << endl;
#endif

      // must check this first -- first check of minimums!
      if (state->areRotationBlocksWithinLimits())
        return state;
      else
        return NULL;
    }
  }

#ifdef DEBUG1  
  cout << "-- minResident: " << *minRes 
       << " requiredFinished? " << ((areRequiredFinished) ? "yes" : "no") << endl;
#endif

  Block* minBlock = state->getResidentEmptyBlock(minRes);
  if (minBlock == NULL) {
#ifdef DEBUG1
    cout << "Error: block is empty but resident was picked ... return null!" << endl;
#endif
    return NULL;

  } else {
#ifdef DEBUG1
  cout << "-- minBlock: " << *minBlock << endl;
#endif
  }

  // Get required rotations, numRequired. If we already determined no more 
  // were required for this resident (above, if we needed to move to getNextResident())
  // then we force this to '0' to skip the first 'for' loop
  int* reqRotations = minRes->getRequiredRotations();
  int numReq = (!areRequiredFinished) ? minRes->getNumRequired() : 0;
#ifdef DEBUG1
  cout << "-- numRequiredRotations: " << numReq << endl;
#endif
  
  int i; Rotation* o; ScheduleState* result; 
  for (i=0; i<numReq; i++) {

    // See if we've done it - if we have, continue
    if (state->isResidentAssigned(minRes->getID(), reqRotations[i]))
      continue;

    // Reference the rotation once we know we're playing with it
    o = op->getRotation(reqRotations[i]);

#ifdef DEBUG1
    cout << "-- rotation (" << reqRotations[i] << "): " << *o << endl;
#endif

    // If it fits this resident's constraints and rotation-block min-max ...
    // [ state->areRotationBlocksWithinLimits() - are we within min-max limits ]
    if (minRes->meetsConstraints(minBlock, o, state) &&
        state->areRotationBlocksWithinLimits(minRes, minBlock, o, 1)) {

#ifdef DEBUG1
      cout << "--- meets constraints ... assigning" << endl;
#endif

      // then assign it!
      state->assign(minRes, minBlock, o);

#ifdef DEBUG1
      cout << "--- assigned ... and recursing ..." << endl;
#endif

      // and recurse
      result = recurseRequired(op, state);

      // if recurse result is not null
      if (result != NULL) {

        //  return the recurse result
        return result;
      }

#ifdef DEBUG1
      cout << "--- NULL ... unassigned ... " << endl;
#endif

      // else unassign  
      state->unassign(minRes, minBlock, o);

    } else {

      // if it doesn't fit constraints, go to next rotation for this resident
#ifdef DEBUG1
      cout << "--- does not meet constraints ... moving on" << endl;
#endif
    }
  }

#if defined DEBUG1 || DEBUG2
  cout << "-- Gone through all required rotations - now try required sets!" << endl;
#endif

  // gone through all required rotations - now try the required sets!
  int numRequiredSets = minRes->getNumRequiredSets();

#if defined DEBUG1 || DEBUG2
  cout << "-- numRequiredSets: " << numRequiredSets << endl;
#endif

  int numInRotation, j;
  for (i=0; i<numRequiredSets; i++) {
    
    // Have we finished this required set?
    if (state->isResidentRequiredSetDone(minRes, i))
      continue;

    // Get the rotations in this set, and the number of them
    reqRotations = minRes->getRequiredSetRotations(i, &numInRotation);

#ifdef DEBUG2
    cout << "--- (2) numInRotation: " << numInRotation << endl;
#endif

    for (j=0; j<numInRotation; j++) {

      // Reference the rotation once we know we're playing with it
      o = op->getRotation(reqRotations[j]);

#ifdef DEBUG2
      cout << "---- rotation (" << reqRotations[j] << "): " << *o << endl;
#endif

      // Does it meet constraints?
      if (minRes->meetsConstraints(minBlock, o, state) &&
          state->areRotationBlocksWithinLimits(minRes, minBlock, o, 1)) {

#ifdef DEBUG2
      cout << "---- meets constraints (2) ... assigning" << endl;
#endif

        // then assign it!
        state->assign(minRes, minBlock, o);
        state->setResidentRequiredSetDone(minRes, i, true);

#ifdef DEBUG2
        cout << "---- assigned (2) ... and recursing ..." << endl;
#endif

        // and recurse
        result = recurseRequired(op, state);

        // if recurse result is not null
        if (result != NULL) {

          //  return the recurse result
          return result;
        }

#ifdef DEBUG2
        cout << "---- NULL (2) ... unassigning ..." << endl;
#endif

        // else unassign  
        state->unassign(minRes, minBlock, o);
        state->setResidentRequiredSetDone(minRes, i, false);

      } else { // did not meet constraints - go to next
#ifdef DEBUG2
        cout << "---- does not meet constraints (2) ... moving on" << endl;
#endif
      }

    } 

    // finished with all rotations in this set and failed - must return NULL!
#ifdef DEBUG2
    cout << "---- finished all rotations in set and failed (2) - return NULL" << endl;
#endif
    return NULL;

  }

  // if we've tried them all, return NULL
#if defined DEBUG1 || DEBUG2
  cout << "---- attempted all rotation sets with no luck (2) - return NULL" << endl;
#endif
  return NULL;
}