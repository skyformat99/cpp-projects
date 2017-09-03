#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "PooledSort.h"

using namespace std;

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
    cerr << "Error: " << c << endl;
    exit(1);
  } catch (...) {
    cerr << "Error loading PooledSort file!" << endl;
    exit(1);
  }

  cout << "First try printing our own way:" << endl;

  // Loop through channels, grab active ones, and print strings
  string* s;
  int i;
  for (i=1;i<=16;i++) {
    if (!ps->isChannelActive(i)) continue;

    try {
      s = (*ps)[i];
    } catch (char* c) {
      cerr << "Error: " << c << endl;
    }

    cout << "  " << i << ": " << *s << endl;
    delete s;
  }

  cout << "Now try printing with the print function: " << endl;

  ps->printString(&cout,NULL);

  cout << "Run some information queries to the PooledSort object:" << endl;

  if (ps->isLoaded())
    cout << "  is Loaded!" << endl;
  else
    cout << "  is Not Loaded!" << endl;

  int numActive = ps->numActiveChannels();
  cout << "  NumActive: " << numActive << endl;

  cout << "Re-load the pooling file ..." << endl;

  try {
    ps->loadFile(argv[1]);
  } catch (char* c) {
    cerr << "Error: " << c << endl;
    exit(1);
  } catch (...) {
    cerr << "Error loading PooledSort file!" << endl;
    exit(1);
  }

  if (ps->isLoaded())
    cout << "  is Loaded!" << endl;
  else
    cout << "  is Not Loaded!" << endl;

  numActive = ps->numActiveChannels();
  cout << "  NumActive: " << numActive << endl;

  cout << "Delete the old PooledSort object and make a new one ..." << endl;

  delete ps;
  ps = new PooledSort();

  cout << "and re-load into this new one." << endl;

  try {
    ps->loadFile(argv[1]);
  } catch (char* c) {
    cerr << "Error: " << c << endl;
    exit(1);
  } catch (...) {
    cerr << "Error loading PooledSort file!" << endl;
    exit(1);
  }

  ps->printString(&cout,new string("\tThis is the new one!"));

  cout << "And delete to exit." << endl;

  delete ps;

  exit(0);
}