#include "PooledSort.h"

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

/**
 * Construct an empty PooledSort object.
 */
PooledSort::PooledSort () 
{
  numActive = 0;
  loaded = false;

  this->fileName = new string();
}

/**
 * Constructs a new PooledSort instance.
 */
PooledSort::PooledSort (const char* fn) 
{
  // Initialize variables
  numActive = 0;
  loaded = false;

  this->fileName = new string(fn);

  // Now load the file
  loadFile();
}

/** 
 * Constructs a new PooledSort instance with a string
 * filename.
 */
PooledSort::PooledSort (string fn)
{
  // Initialize variables
  numActive = 0;
  loaded = false;

  this->fileName = new string(fn);

  // Now load the file
  loadFile();
}

/**
 * Destructs the current PooledSort instance.
 */
PooledSort::~PooledSort () 
{
  freeMemory();
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

/**
 * Copies the channel numbers from the private field into the
 * array passed as the argument. 
 */
void PooledSort::getChannels (int *channels, int size)
{
  memcpy(channels,this->chan,sizeof(int)*size);
}

/**
 * For the given channel number, populates the buffer with
 * the string representation of the channels (up to buf_len).
 * Returns 0 on success, or error code on error.
 */
string* PooledSort::operator[] (int channel) 
{
  stringstream ss;
  ss.str("");

  int chanIndex;
  chanIndex = channelIndex(channel);

  int i;

  if (chanIndex == -1)
    throw "Error populating string buffer - channel does not exist";

  if (numSorts[chanIndex] == 0) 
    throw "Error populating string buffer - no sorts exist for channel";

  ss << sorts[chanIndex][0];

  for (i=1;i<numSorts[chanIndex];i++)
    ss << PRINT_DELIM << sorts[chanIndex][i];

  return new string(ss.str());
}

/*
 * Returns the max value in the sorts array, for a given channel.
 */
int PooledSort::getMaxSort (int chan) 
{
  int chanIndex;
  chanIndex = channelIndex(chan);

  if (chanIndex == -1) 
    return -1;

  int i, maxSort = -1;
  for (i=0;i<numSorts[chanIndex];i++) {
    if (sorts[chanIndex][i] > maxSort) maxSort = sorts[chanIndex][i];
  }
  
  return maxSort;   
}

/**
 * Returns the number of sorts for the given channel.
 */
int PooledSort::getNumSorts (int channel) 
{
  int chanIndex;
  chanIndex = channelIndex(channel);

  if (chanIndex == -1) 
    return -1;

  return numSorts[chanIndex];
}

/**
 * Fills the passed argument with the sort values for the given
 * channel. Returns 0 on success, -1 on error.
 */
int PooledSort::getSorts (int channel, int *sorts, int numSorts) 
{
  int i, chanIndex;

  chanIndex = channelIndex(channel);

  if (chanIndex == -1) 
    return -1;

  for (i=0;i<numSorts;i++) 
    sorts[i] = this->sorts[chanIndex][i];

  return 0;
}

/**
 * Returns true if channel is active, false otherwise (or if
 * file is not loaded).
 */
bool PooledSort::isChannelActive (int channel) 
{
  if (!isLoaded())
    return false;

  return chanActive[channel-1];
}

/**
 * Returns true if loaded, false otherwise.
 */
bool PooledSort::isLoaded () 
{
  return loaded;
}

/**
 * Loads a file with the given name, and returns
 * true if loaded (or false otherwise).
 */
void PooledSort::loadFile (const char *fn) 
{
  if (loaded)
    freeMemory();

  this->fileName = new string(fn);

  // Now load the file
  loadFile();
}

/**
 * Returns the number of active channels, or -1 if
 * the file has not been loaded.
 */
int PooledSort::numActiveChannels () 
{
  if (!isLoaded())
    return -1;

  return numActive;
}

/**
 * Writes out all of the channel data to a file pointer.
 */
void PooledSort::printString (FILE* stream, char* msg) 
{
  if (!isLoaded())
    return;

  string* str_buf;

  fprintf(stream,"%s\n",msg);

  int i;
  for (i=1;i<=N_CHAN;i++) {

    if (!isChannelActive(i)) 
      continue;

    try {
      str_buf = (*this)[i];
      fprintf(stream,"  Channel %d: %s\n",i,str_buf->c_str());
      delete str_buf;
    } catch (char* c) {
      continue;
    }

  }
}

/**
 * Writes out all of the channel data to an out-stream.
 */
void PooledSort::printString (ostream* strm, string* msg)
{
  if (!isLoaded())
    return;

  string* str_buf;

  if (msg != NULL)
    (*strm) << *msg << endl;

  int i;
  for (i=1;i<=16;i++) {

    if (!isChannelActive(i))
      continue;

    try {
      str_buf = (*this)[i];
      (*strm) << "  Channel " << i << ": " << *str_buf << endl;
      delete str_buf;
    } catch (char* c) {
      continue;
    }
  }
}

// * * * * * * * * * * * * * * * *
// * Private Methods 
// * * * * * * * * * * * * * * * *

/**
 * Returns the channel index for the given channel,
 * or -1 if the data is not loaded or the channel does not exist.
 */
int PooledSort::channelIndex (int channel) 
{
  if (!isLoaded())
    return -1;

  if (!isChannelActive(channel)) 
    return -1;

  int i, chanIndex = -1;

  for (i=0;i<numActive;i++) {
    if (chan[i] == channel) chanIndex = i;
  }

  return chanIndex;
}

/**
 * If any data is loaded, deletes all arrays associated
 * with this loaded data.
 */
void PooledSort::freeMemory ()
{
  if (loaded) {
    int i;
    for (i=0;i<numActive;i++)
      delete [] sorts[i];

    delete [] sorts;
    delete [] chan;
    delete [] numSorts;
    delete [] chanActive;

    delete fileName;

    fileName = new string();
    numActive = 0;
    loaded = false;
  }
}

/**
 * Loads the pooled sort into the class. Returns 0 if no
 * error, or error code on error.
 */
void PooledSort::loadFile () 
{
  // Declare necessary variables
  MATFile *mFile;
  mxArray *_sort, *_cell;
  const int* dims;
  const int CHAR_BUFFER_LEN = 256;
  char buf[CHAR_BUFFER_LEN];
  char DELIMITOR[] = " ,";
  int dim, result, M, N;

  // Open .mat file
  mFile = matOpen((*fileName).c_str(),"r");

  if (mFile == NULL)
    throw "Pooled file pointer is null";

  // Get sortedList variable 
  _sort = matGetVariable(mFile, "sortedList");

  if (_sort == NULL) 
    throw "SortedList variable does not exist in pooled file";

  // Get dimensions & verify
  dim = mxGetNumberOfDimensions(_sort);
  dims = (int*)mxGetDimensions(_sort);
  
  if (dim != 2) 
    throw "SortedList variable is of wrong size";

  if (dims[0] != N_CHAN && dims[1] != N_CHAN) 
    throw "SortedList variable does not have correct number of rows";

  // Verify array type
  if (!mxIsCell(_sort)) 
    throw "SortedList variable is not cell array";

  // Through each "channel", get the sort string and print
  int i, j;
  int* activeChanNums = new int[N_CHAN];
  int* numAutoClasses = new int[N_CHAN];
  int** classHolder = new int*[N_CHAN];

  for (i=0;i<N_CHAN;i++) {
    classHolder[i] = new int[MAX_AUTOCLASSES];

    _cell = mxGetCell(_sort,i);

    if (!mxIsChar(_cell)) 
      throw "SortedList variable is not cell array of characters";

    // Initialize number of auto-classes to 0
    numAutoClasses[i] = 0;

    M = mxGetM(_cell);
    N = mxGetN(_cell);

    if (M == 0 && N == 0)
      continue;

    // Get the char array string
    result = mxGetString(_cell,buf,CHAR_BUFFER_LEN);
    if (result == 1)
      throw "Failed pulling string from cell array in SortedList";

    char *pch;
    char *context;
    pch = strtok_s(buf,DELIMITOR,&context);
    while (pch != NULL) {
      classHolder[i][numAutoClasses[i]] = atoi(pch);
      numAutoClasses[i]++;
      pch = strtok_s(NULL,DELIMITOR,&context);
    }

    activeChanNums[numActive] = i;
    numActive++;

    mxFree(_cell);
  }

  if (numActive == 0)
    throw "No active channels found!";
 
  // malloc private fields, populate
  chanActive = new bool[N_CHAN];
  chan = new int[numActive];
  numSorts = new int[numActive];
  sorts = new int*[numActive];

  for (i=0;i<N_CHAN;i++) chanActive[i] = false;

  for (i=0;i<numActive;i++) {
    chan[i] = activeChanNums[i]+1; // add 1 to get actual channel # (1-16, not 0-15)
    chanActive[chan[i]-1] = true; // subtract 1; reason above
    numSorts[i] = numAutoClasses[chan[i]-1]; // subtract 1; reason above

    sorts[i] = new int[numSorts[i]];

    for (j=0;j<numSorts[i];j++) 
      sorts[i][j] = classHolder[chan[i]-1][j]; // subtract 1; reason above
  }

  // Delete new arrays & variables
  delete [] activeChanNums;
  delete [] numAutoClasses;
  for (i=0;i<N_CHAN;i++)
    delete [] classHolder[i];
  delete [] classHolder;


  mxFree(_sort);
  matClose(mFile);

  loaded = true;

  // If we've made it to end, return no error
  return;
}