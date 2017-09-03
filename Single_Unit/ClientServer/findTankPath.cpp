#include <stdio.h>

#include <string>
#include <iostream>

#include "ClientFarm.h"

/** Formats host-dir string to WINS format */
void formatHostDir (string, string, string*);

using namespace std;

int main (int argc, char* argv[]) {

  if (argc <= 2) {
    cout << "Usage: findTankPath <tankName> <serverFile>" << endl;
    exit(1);
  }

  ClientFarm *cf;
  try {
    cf = new ClientFarm(argv[2]);
  } catch (char *c) {
    cout << "Error creating ClientFarm: " << c << endl;
    exit(1);
  }

  string host;
  string dir;
  string formattedStr;
  if (cf->doesTankExist(argv[1],&host,&dir)) {

    try {
      formatHostDir(host,dir,&formattedStr);
    } catch (char *c) {
      cout << "Error formatting host dir: " << c << endl;
      exit(1);
    }

    cout << formattedStr << endl;

  } else {
    cout << "Error: Tank not found!" << endl;
  }

  delete cf;

  exit(0);
}

/**
 * Given the host name and directory to a tank file, returns a formatted
 * string in the standard WINS format (i.e. \\host\drive$\path) for use
 * in adding the tank to the TTankEng.
 */
void formatHostDir (string host, string dir, string* formattedStr) 
{
  stringstream s;
  int startIndex;
  int pos;

  if (host == "localhost" || host == "uber-server.urmc-sh.rochester.edu") {
    startIndex = 0;

  } else {

    startIndex = 3;
    s << "\\\\" << host << "\\";

    if (dir.size() <= 3)
      throw "dir string must contain drive letter";

    if ((pos = dir.find(":\\")) == string::npos) 
      throw "dir string must contain ':\' combination";

    if (pos != 1)
      throw "dir string must contain ':\' at pos 1";

    s << dir.at(0) << "$\\";
  }

  if (dir.size() > 3) {

    // Replace all back-slashes with forward-slashes
    while ((pos = dir.find("/")) != string::npos)
      dir.replace(pos,1,"\\");

    // Find index of final "\", to chop off filename
    pos = dir.find_last_of("\\");

    // And add to stream
    if (pos != string::npos)
      s << dir.substr(startIndex,pos-startIndex) << "\\";
  }

  *formattedStr = s.str();
}