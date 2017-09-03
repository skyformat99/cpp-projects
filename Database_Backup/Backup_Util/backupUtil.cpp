#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

/** Prints out verbose output errors */
#define DEBUG1 false

/** Prints out standard output messages */
#define DEBUG2 true

/** Number of files we want to keep */
#define NUM_TO_KEEP 5

using namespace std;

int main (int argc, char* argv[]) {

  if (argc < 3) {
    cerr << "Usage: backupUtil.exe <loadFile> <root>" << endl;
    exit(1);
  }

  // Get the number to keep if it was provided
  int numToKeep = NUM_TO_KEEP;
  if (argc == 4)
    sscanf(argv[3],"%d",&numToKeep);
  if (DEBUG2) cout << "Number to keep: " << numToKeep << endl;

  string* loadFile = new string(argv[1]);
  if (DEBUG2) cout << "Loading file: " << *loadFile << endl;

  // Gathers fileRoot, ensures trailing slash
  string* fileRoot = new string(argv[2]);
  if ( (*fileRoot).at((*fileRoot).length()-1) != '\\' &&
       (*fileRoot).at((*fileRoot).length()-1) != '/') 
    fileRoot = new string(*fileRoot + "\\");

  if (DEBUG2) cout << "Directory: " << *fileRoot << endl;

  // Handles loading of the file
  fstream fp((*loadFile).c_str(), fstream::in);

  if (!(fp.is_open())) {
    cerr << "Error: couldn't open file!" << endl;
    exit(1);
  }

  vector<string> files;
  string* fileStr;
  while (!fp.eof()) {
    fileStr = new string();
    fp >> *fileStr;

    if (DEBUG1) cout << "  " << *fileStr << endl;

    if (*fileStr != "")
      files.push_back(*fileStr);
  }
  fp.close();

  if (DEBUG2) cout << "Number of files loaded: " << files.size() << endl;

  // Handles finding and deleting the files of interest
  int numToKill = files.size() - numToKeep;

  if (numToKill <= 0) {
    if (DEBUG2) cout << "No files to delete!" << endl;
    exit(0);
  }

  if (DEBUG2) cout << "Deleting " << numToKill << " files." << endl;

  int i;
  string wholeFile;
  for (i=0;i<numToKill;i++) {

    wholeFile = *fileRoot + files[i];

    if (DEBUG2) cout << "  Deleting " << wholeFile << " ... ";

    if (remove(wholeFile.c_str()) != 0) {
      if (DEBUG2) cout << "FAILED" << endl;
    } else {
      if (DEBUG2) cout << "SUCCESSFUL" << endl;
    }

  }

  exit(0);
}