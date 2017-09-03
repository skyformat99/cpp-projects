#include "SpikeList.h"

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

/**
 * Given the channel # and base filename (no extension),
 * loads all of the file data into the object.
 */
SpikeList::SpikeList (int channel, string fn, int *sorts, int numSorts) 
{
  loaded = false;

  loadFile(channel,fn,sorts,numSorts);
}

/**
 * Instanciates a new, empty SpikeList.
 */
SpikeList::SpikeList () 
{
  loaded = false;

  channel = -1;
  numSpikes = -1;

  fileName = NULL;

  spikeTimes = NULL;
  oldClasses = NULL;
  newClasses = NULL;
  blocks = NULL;

  numBlocks = -1;
  blockOffsets = NULL;
  blockNumbers = NULL;
}

/**
 * Destructs the spikeList.
 */
SpikeList::~SpikeList () 
{
  freeMemory();
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

/**
 * Returns the pointer to the blockNumbers block.
 * Stores in the passed argument the number of blocks, if not null.
 */
int* SpikeList::getBlockNumbers (int* nb) 
{
  if (!isLoaded())
    return NULL;

  if (nb != NULL)
    *nb = this->numBlocks;

  return this->blockNumbers;
}

/**
 * Returns the pointer to the blockOffsets block.
 * Stores in the passed argument the number of blocks, if not null.
 */
int* SpikeList::getBlockOffsets (int* nb) 
{
  if (!isLoaded())
    return NULL;

  if (nb != NULL)
    *nb = this->numBlocks;

  return this->blockOffsets;
}

/**
 * Returns the block offset (if present), or -1 if it is
 * not present.
 */
int SpikeList::getBlockOffset (int blockNumber) 
{
  if (!isLoaded())
    return NULL;

  int i;
  for (i=0;i<this->numBlocks;i++) {
    if (blockNumbers[i] == blockNumber) return blockOffsets[i];
  }

  return -1;
}

/**
 * Returns the offset for the next block, numSpikes if this
 * is the last block in the offset list, or -1 if this block
 * is not in the offset list.
 */
int SpikeList::getBlockNextOffset (int blockNumber) 
{
  if (!isLoaded())
    return NULL;

  int i;
  for (i=0;i<this->numBlocks;i++) {
    if (blockNumbers[i] == blockNumber) {
      if (i == this->numBlocks - 1) 
        return this->numSpikes;
      else
        return blockOffsets[i+1];
    }
  }

  return -1;
}

/**
 * Returns the offset for the previous block, 0 if this is
 * the first block in the offset list, or -1 if this block
 * does not exist in the offset list.
 */
int SpikeList::getBlockPrevOffset (int blockNumber) 
{
  if (!isLoaded())
    return NULL;

  int i;
  for (i=0;i<this->numBlocks;i++) {
    if (blockNumbers[i] == blockNumber) {
      if (i == 0)
        return 0;
      else
        return blockOffsets[i-1];
    }
  }

  return -1;
}    

/**
 * Returns the channel for this SpikeList.
 */
int SpikeList::getChannel ()
{
  return this->channel;
}

/**
 * Returns the number of blocks loaded in this SpikeList
 */
int SpikeList::getNumBlocks () 
{
  return this->numBlocks;
}

/**
 * Returns the number of spikes present in the arrays.
 */
int SpikeList::getNumSpikes () 
{
  if (!isLoaded())
    return -1;

  return this->numSpikes;
}

/**
 * Returns the number of spikes present in the block, with
 * the given sort.
 */
int SpikeList::getNumSpikes (int block, int sort) 
{
  int iStart, iEnd, i, count = 0;

  iStart = (block == 0) ? 0 : blockOffsets[block-1];
  iEnd = (block == numBlocks-1) ? numSpikes : blockOffsets[block];

  for (i=iStart;i<iEnd;i++) {
    if (newClasses[i] == sort) count++;
  }

  return count;
}

/**
 * Given an int array of at least 3 elements (0, 1, 2)
 * this will return the number of spikes in each sort class.
 */
void SpikeList::getSpikesPerSort (int *result) 
{
  int i;

  for (i=0;i<3;i++)
    result[i] = 0;

  for (i=0;i<this->numSpikes;i++)
    result[newClasses[i]]++;

  return;
}

/** 
 * Returns the pointer to the new spikeClasses array.
 */
int* SpikeList::getSpikeClasses () 
{
  if (!isLoaded())
    return NULL;

  return this->newClasses;
}

/**
 * Returns a pointer to the spikeTimes array.
 */
double* SpikeList::getSpikeTimes () 
{
  if (!isLoaded())
    return NULL;

  return this->spikeTimes;
}

/**
 * Determines if the .txt and .mat files have been loaded.
 */
bool SpikeList::isLoaded () 
{
  return this->loaded;
}

/**
 * If the spike times are loaded, determines if they
 * increase monotonically within an individual block.
 */
bool SpikeList::isTimeMonotonic () 
{
  if (!isLoaded())
    return false;

  int i, block = 1;
  double t = this->spikeTimes[0];
  for (i=1;i<numSpikes;i++) {
    if (i == this->blockOffsets[block]) {
      block++;
      t = this->spikeTimes[i];
      continue;
    }
    if (spikeTimes[i] < t) return false;
    t = spikeTimes[i];
  }

  return true;
}

/**
 * Loads a given channel file, using the same arguments as the constructor uses.
 */
void SpikeList::loadFile (int channel, string fn, int *sorts, int numSorts)
{
  this->channel = channel;
  this->fileName = new string(fn);

  // Now load the files
  loadFiles();

  // If we haven't hit an error, try remapping classes
  remapClasses(sorts,numSorts);

  // Still no error, try computing block offsets
  computeBlockOffsets();

  this->loaded = true;
}

// * * * * * * * * * * * * * * * *
// * Private Methods 
// * * * * * * * * * * * * * * * *

/**
 * If data was loaded, this will clear all of the memory of
 * that old data.
 */
void SpikeList::freeMemory ()
{
  if (loaded) {
    delete this->fileName;

    if (spikeTimes) delete [] spikeTimes;
    if (oldClasses) delete [] oldClasses;
    if (newClasses) delete [] newClasses;
    if (blocks)     delete [] blocks;

    if (blockOffsets) delete [] blockOffsets;
    if (blockNumbers) delete [] blockNumbers;

    loaded = false;
    channel = -1;
    numSpikes = -1;
    numBlocks = -1;
  }
}

/**
 * Computes the offsets to the start of individual blocks in 
 * the blocks array. By virtue of this method's existence we're assuming
 * that the blocks are monotonically increasing, so we are therefore
 * also requiring that 
 *   min(blockNumber) == blocks[0]
 * and
 *   max(blockNumber) == blocks[numSpikes-1]
 */
void SpikeList::computeBlockOffsets () 
{
  int tmpBlocks[MAX_NUM_BLOCKS];
  int tmpOffsets[MAX_NUM_BLOCKS];

  int blockIndex = 0;
  int i;

  tmpOffsets[blockIndex] = blockIndex;
  tmpBlocks[blockIndex] = blocks[0];

  for (i=1;i<this->numSpikes;i++) { // go through all spikes
    if (blocks[i] != tmpBlocks[blockIndex]) { // we've hit the next block start
      blockIndex++;
      tmpOffsets[blockIndex] = i; // set offset to start of new block
      tmpBlocks[blockIndex] = blocks[i]; // and get new block number
    }
  }

  this->numBlocks = blockIndex+1;

  blockOffsets = new int[numBlocks];
  blockNumbers = new int[numBlocks];

  for (i=0;i<this->numBlocks;i++) {
    blockOffsets[i] = tmpOffsets[i];
    blockNumbers[i] = tmpBlocks[i];
  }

  return;
}

/**
 * Loads the files, both the matlab file and the txt files.
 */
void SpikeList::loadFiles () throw (char*)
{
  stringstream ss;
  ss.str("");

  MATFile *mFile;
  mxArray *_spikeTimes, *_classList;
  int result, dim;
  const int *dims;

  ss << *this->fileName << ".mat";

  // Open .mat file
  mFile = matOpen(ss.str().c_str(),"r");
  ss.str("");

  if (mFile == NULL) 
    throw "SpikeList file returns an invalid file pointer";

  // Get spikeTimes variable 
  _spikeTimes = matGetVariable(mFile, "SpikeTimes");

  if (_spikeTimes == NULL)
    throw "SpikeTimes variable does not exist in SpikeList file";

  // Get dimensions & verify
  dim = mxGetNumberOfDimensions(_spikeTimes);
  dims = (int*)mxGetDimensions(_spikeTimes);
  
  if (dim != 2) 
    throw "Dimensions of SpikeTimes variable are invalid";

  this->numSpikes = FMAX(dims[0],dims[1]);

  // Ensures that the spikeTimes is of double format
  if (!mxIsDouble(_spikeTimes))
    throw "SpikeTimes variable is not of double format";

  // Gather pointer to spikeTimes, and copy into our array
  double *mxPr; 
  spikeTimes = new double[numSpikes];

  mxPr = mxGetPr(_spikeTimes);

  if (mxPr == NULL)
    throw "Mx pointer to SpikeTimes variable is invalid";

  int i;
  for (i=0;i<this->numSpikes;i++)
    spikeTimes[i] = *(mxPr+i);

  // Get classList variable 
  _classList = matGetVariable(mFile, "classList");

  if (_classList == NULL)
    throw "ClassList variable does not exist in SpikeList file";

  // Get dimensions & verify
  dim = mxGetNumberOfDimensions(_classList);

  if (dim != 2) 
    throw "ClassList variable has improper dimensions";

  int M, N;
  M = mxGetM(_classList);
  N = mxGetN(_classList);

  if ((M != this->numSpikes) ||
      (N != 3))
    throw "ClassList variable has improper number of rows or columns";

  if (!mxIsDouble(_classList))
    throw "ClassList variable is not of type double"; 

  // Get pointer to the starting double data, increment by M
  // to get to the 2nd column
  mxPr = mxGetPr(_classList);
  mxPr += M;

  // Declare our data array, copy in after cast to int
  oldClasses = new int[numSpikes];

  for (i=0;i<this->numSpikes;i++)
    oldClasses[i] = (int)*(mxPr+i);

  // Free the matlab data, close up file
  mxDestroyArray(_classList);
  mxDestroyArray(_spikeTimes);
  matClose(mFile);

  // Now operate on .txt file
  ss << *this->fileName << ".txt";

  // Declare empty array and dummy pointers
  blocks = new int[numSpikes];

  double dummyDbl;
  int dummyCls;

  FILE *fp;
  result = fopen_s(&fp,ss.str().c_str(),"r");
  if (fp == NULL)
    throw "Text file containing block information cannot be opened";

  for (i=0;i<this->numSpikes;i++)
    fscanf_s(fp,"%d %f %d\n",blocks+i,&dummyDbl,&dummyCls);

  fclose(fp);

  return;
}

/**
 * Given the map from the PooledSort class, initializes
 * the newClass array.
 */
void SpikeList::remapClasses (int *sortMap, int numSorts) throw (char*)
{
  int i;
  newClasses = new int[numSpikes];

  for (i=0;i<this->numSpikes;i++) {

#ifdef CLASS_VALUE_CHECK
    // If this is defined, we input-check the class values to ensure
    // that no class values are out of the sort range
    if (oldClasses[i]+1 < 0 || oldClasses[i]+1 >= numSorts)
      throw "oldClass value out of range of sort map - seg fault prevented";
#endif

    newClasses[i] = sortMap[oldClasses[i]+1]; // the +1 here removes the '0' class
  }

  return;
}