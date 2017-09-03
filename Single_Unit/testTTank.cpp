#include "TTank.h"

#include <iostream>

#define MAX_EVENTS 500000

using namespace std;

/**
 * Main entry into function. 
 */
int main (int argc, char *argv[]) {

  if (argc < 3) {
    cerr << "Usage: " << argv[0] << " <tank> <block>" << endl;
    exit(1);
  }

  TTank* tt;

  try {
    tt = new TTank("Local");
  } catch (char* c) {
    cerr << "Error opening TTank server: " << *c << endl;
    exit(1);
  }

  vector<string> servers = tt->getServers();
  cout << "Number of servers: " << servers.size() << endl;

  vector<string> tanks = tt->getTanks();
  cout << "Number of tanks: " << tanks.size() << endl;

  vector<string>::iterator it;
  for (it = tanks.begin(); it != tanks.end(); ++it)
    cout << "\t" << *it << endl;
  
  if (!tt->containsTank(argv[1])) {
    cerr << "Error: server does not contain tank specified as argument!" 
         << endl;
    exit(1);
  }

  if (tt->openTank(argv[1]) != 0) {
    cerr << "Error: openTank returned error code" << endl;
    exit(1);
  }

  cout << "Opened tank " << argv[1] << " ..." << endl;

  int blockNum;
  try {
    blockNum = atoi(argv[2]);
  } catch (...) {
    cerr << "Error: block number not a valid integer" << endl;
    exit(1);
  }

  if (tt->selectBlock(blockNum) != 0) {
    cerr << "Error: block unable to be selected in tank" << endl;
    exit(1);
  }

  tt->createEpocIndexing();

  cout << "Opened block " << blockNum << ", created indexing ..." << endl;

  int i, count;
  double* d;

//  for (i=0;i<500;i++) {
//    cout << "Starting iteration " << i << " ..." << endl;

    try {
      count = tt->eventRead("Swep",MAX_EVENTS);
    } catch (char* c) {
      cerr << "Error reading events: " << c << endl;
      exit(1);
    }

    cout << "Count of returned Swep events: " << count << endl;

    d = new double[count];

    try {
      tt->eventParse(count,d,count);
    } catch (char* c) {
      cerr << "Error parsing events: " << c << endl;
      exit(1);
    }

    cout << "Successfully parsed Swep events ..." << endl;
    cout << "  Swep 201: " << d[201] << endl;
    cout << "  Swep " << (count-1) << ": " << d[(count-1)] << endl;

    delete [] d;
    cout << "Successfully deleted Swep events ..." << endl;

    d = new double[count];

    try {
      tt->eventParseInfo(count,6,d,count);
    } catch (char* c) {
      cerr << "Error parsing info events: " << c << endl;
      exit(1);
    }

    cout << "Successfully parsed Swep info events ..." << endl;
    cout << "  Swep Info 201: " << d[201] << endl;
    cout << "  Swep Info " << (count-1) << ": " << d[(count-1)] << endl;

    delete [] d;
    cout << "Successfully deleted Swep Info events ..." << endl;

//    Sleep(200);
//  }

  try {
    tt->closeTank();
    tt->closeServer();
  } catch (char* c) {
    cerr << "Error closing server: " << c << endl;
    exit(1);
  }

  cout << "Closed gracefully ... exiting ..." << endl;

  exit(0);
}