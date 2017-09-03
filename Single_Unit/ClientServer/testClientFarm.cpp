#include <stdio.h>

#include <vector>
#include <string>
#include <iostream>

#include "PooledSort.h"
#include "SpikeList.h"
#include "ByteBuffer.h"
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "ClientFarm.h"
#include "TTank.h"

#include "TankServerDefs.h"

#define DEBUG1 false

using namespace std;

int main (int argc, char* argv[]) {

  ClientFarm *cf;
  try {
    cf = new ClientFarm("C:\\usr\\cpp\\Single_Unit\\ClientServer\\servers.txt");
  } catch (char *c) {
    cout << "Error creating ClientFarm: " << c << endl;
    exit(1);
  }

  vector<string> tanks;
  int numElements = cf->getTankList(argv[1],&tanks);

  if (numElements > 0) {
    cout << numElements << " tanks found!" << endl;
  
    vector<string>::iterator it;
    for ( it = tanks.begin() ; it < tanks.end(); it++ ) 
      cout << "\t" << *it << endl;
  } else 
    cout << "Error: No Tanks Found!" << endl;

/**
  // Determine if single unit data exists
  if (cf->doesSingleUnitExist(argv[1],&host))
    cout << "SU found! (Host: " << host << ")" << endl;
  else 
    cout << "SU not found!" << endl;

  // Get the pooled sort data
  PooledSort ps;
  int numChannels = 0,i;

  int errCode = cf->getPoolingData(argv[1],&ps);
  if (errCode == 0) {
    numChannels = ps.numActiveChannels();
    cout << "Pooling data transmitted! NumChannels: " << numChannels << endl;
  } else {
    cout << "Pooling data not found!" << endl;
  }

  // Get the spike data
  if (numChannels > 0) {
    SpikeList **spikes = new SpikeList*[numChannels];
    for (i=0;i<numChannels;i++)
      spikes[i] = new SpikeList();

    if (cf->getSpikeData(argv[1],&numChannels,spikes)) {
      for (i=0;i<numChannels;i++) {
        if (spikes[i]->isLoaded())
          cout << "  SpikeList for Channel " << spikes[i]->getChannel() 
               << " loaded!\t(Spikes: " << spikes[i]->getNumSpikes() << "\tBlocks: " 
               << spikes[i]->getNumBlocks() << ")" << endl;
        else
          cout << "  SpikeList for Channel " << spikes[i]->getChannel() << " NOT loaded!" << endl;
      }
    } else {
      cout << "Spike data not found!" << endl;
    }
  }
**/

  delete cf;

/**
  // Try some basic ttank manipulation
  TTank* tt = NULL;
  string localRoot = "E:/Walton_Data/DBA/Tanks";

  try {
    tt = new TTank("Local");
  } catch (char *c) {
    cerr << "TTank Error: " << c << endl;
    tt->closeServer();
    exit(1);
  }

  // Remove the tank if it's contained previously
  if (tt->containsTank(argv[1]))
    tt->removeTank(argv[1]);
  
  // Now add it using built root/host
  if (tt->addTank(argv[1],localRoot) != 0) {
    cerr << "Error adding tank " << argv[1] << endl;
    tt->closeServer();
    exit(1);
  }

  tt->closeServer();

  delete tt;
**/

  exit(0);
}
