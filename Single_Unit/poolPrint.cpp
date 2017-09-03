#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "PooledSort.h"

/**
 * Loads in the PooledClass .mat file, and prints out
 * the channel / sort code for each active channel.
 *
 * Compiled using MSVC: 
 *   cl /O2 readPooled.cpp /link libmat.lib libmx.lib
 */
int main (int argc, char* argv[]) {

  // Verify input arguments
  if (argc < 2) {
    fprintf(stderr,"Usage: %s <.mat-file>\n",argv[0]);
    exit(1);
  }

  // Initialize a new PooledSort object
  PooledSort* ps;

  // Load the file, return error if exists
  try {
    ps = new PooledSort(argv[1]);
  } catch (char* c) {
    cerr << "Error loading pooled sort: " << c << endl;
    exit(1);
  } catch (...) {
    cerr << "Error loading pooled sort!" << endl;
    exit(1);
  }
 
  string* s;
  int i;
  // Loop through channels, grab active ones, and print strings
  for (i=1;i<=16;i++) {
    if (!ps->isChannelActive(i)) continue;

    try {
      s = (*ps)[i];
    } catch (char* c) {
      cerr << "Error: " << c << endl;
    }

    printf("%d#%s\n",i,(*s).c_str());
    delete s;
  }

  exit(0);
}