#include <string>
#include <iostream>

#include "SingleUnitDir.h"

using namespace std;

/**
 * Main entry into function. 
 */
int main (int argc, char *argv[]) {

  if (argc < 3) {
    cout << "Usage: " << argv[0] << " <su_dir> <tank>" << endl;
    exit(1);
  }

  SingleUnitDir* su = new SingleUnitDir(argv[1]);

  if (su->contains(argv[2]))
    cout << "SU contains tank " << argv[2] << endl;
  else
    cout << "SU does not contain tank " << argv[2] << endl;

  string pooledFile;
  if (su->getPoolFile(argv[2],&pooledFile))
    cout << "Pooling file: " << pooledFile << endl;

  int chan = 2;
  string chanFile;
  if (su->getChannelFileBase(argv[2],chan,&chanFile))
    cout << "Chan " << chan << " file: " << chanFile << endl;

  exit(0);
}