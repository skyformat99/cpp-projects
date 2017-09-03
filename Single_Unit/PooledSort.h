#ifndef __pooledSort
#define __pooledSort

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>

#include "mat.h"
#include "matrix.h"

#include "UploadUtil.h"

using namespace std;

/** Max number of autoclass classes allowed **/
#define MAX_AUTOCLASSES 128

/** Number of channels to load **/
#define N_CHAN 16

/** Delimiter used when displaying the sort information **/
#define PRINT_DELIM " "

/**
 * Class operates on a Matlab .mat pooling file, which contains
 * a single cell array sortedList.
 */
class PooledSort {

private:

  string *fileName;

  bool loaded;

  int numActive;
  int* chan;
  int* numSorts;
  int** sorts;
  bool* chanActive;

  int channelIndex (int);
  void freeMemory ();
  void loadFile ();

public:

  PooledSort ();
  PooledSort (const char*);
  PooledSort (string);
  ~PooledSort ();

  void getChannels (int*, int);

  string* operator[] (int);

  int getMaxSort (int);
  int getNumSorts (int);
  int getSorts (int, int*, int);

  bool isChannelActive (int);
  bool isLoaded ();
  void loadFile (const char*);
  int numActiveChannels ();

  void printString (FILE*, char*);
  void printString (ostream*, string*);
};

#endif