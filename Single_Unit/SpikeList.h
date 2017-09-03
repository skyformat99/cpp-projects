#ifndef __spikeList
#define __spikeList

#include <string>
#include <sstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include "mat.h"
#include "matrix.h"

#include "UploadUtil.h"

using namespace std;

/** 
 * If defined, input-checking is performed on class values in the 
 * remapClasses function to verify that no class value is outside
 * the range of the sorts. This might slow down the code though.
 */
#define CLASS_VALUE_CHECK

/** Maximum number of blocks allowed in the computeBlockOffsets routine **/
#define MAX_NUM_BLOCKS 128

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
(maxarg1) : (maxarg2))

static double minarg1,minarg2;
#define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ?\
(minarg1) : (minarg2))

/**
 * Handles all spike-list operations.
 */
class SpikeList {

private:

  int channel;
  string *fileName;

  int numSpikes;
  double *spikeTimes;
  int *oldClasses;
  int *newClasses;
  int *blocks;

  int numBlocks;
  int *blockOffsets;
  int *blockNumbers;

  bool loaded;

  void freeMemory ();
  void computeBlockOffsets ();
  void loadFiles ();
  void remapClasses (int*, int);
  
public:

  SpikeList ();
  SpikeList (int, string, int*, int);
  ~SpikeList ();

  int* getBlockNumbers (int*);
  int* getBlockOffsets (int *);
  int getBlockOffset (int);
  int getBlockNextOffset (int);
  int getBlockPrevOffset (int);
  int getChannel ();
  int getNumBlocks ();
  int getNumSpikes ();
  int getNumSpikes (int, int);
  double* getSpikeTimes ();
  int* getSpikeClasses ();
  void getSpikesPerSort (int*);
  bool isLoaded ();
  bool isTimeMonotonic ();

  void loadFile (int, string, int*, int);

//  bool serialize (ByteBuffer*);
//  bool unserialize (ByteBuffer*);
};

#endif