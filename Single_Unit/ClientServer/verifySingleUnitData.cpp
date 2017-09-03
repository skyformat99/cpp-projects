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

#define CHECK_ALL 0
#define CHECK_POOLED_ONLY 1
#define CHECK_DIRECTORY_ONLY 2

using namespace std;

int main (int argc, char* argv[]) {

  if (argc <= 2) {
    cout << "Usage: verifySingleUnitData <tankName> <serversFile> [<check_level>=ALL]" << endl;
    exit(1);
  }

  int checkLevel = CHECK_ALL;
  if (argc >= 4) {
    try {
      checkLevel = atoi(argv[3]);
    } catch (...) {
      checkLevel = CHECK_ALL;
    }
  }

  ClientFarm *cf;
  try {
    cf = new ClientFarm(argv[2]);
  } catch (char *c) {
    cout << "Error: Problem Creating ClientFarm: " << c << endl;
    exit(1);
  }

  string host;
  string dir;

  // Determine if single unit data exists
  try {
    if (!(cf->doesSingleUnitExist(argv[1],&host,&dir))) {
      cout << "Error: SU Directory not found" << endl;
      exit(0);
    }
  } catch (...) {
    cout << "Error: Could not poll for SU directory" << endl;
    exit(0);
  }

  // Are we done after verifying the directory?
  if (checkLevel == CHECK_DIRECTORY_ONLY) {
    delete cf;
    cout << "TRUE" << endl;
    exit(0);
  }

  // Get the pooled sort data
  PooledSort ps;
  int numChannels = 0,i;

  try {
    int errCode = cf->getPoolingData(argv[1],&ps);
    if (errCode == 0) {
      numChannels = ps.numActiveChannels();
    } else {
      cout << "Error: Pooling Data Not Found" << endl;
      exit(0);
    }
  } catch (...) {
    cout << "Error: Problem while gathering pooling data" << endl;
    exit(0);
  }

  // Are we done after pooling?
  if (checkLevel == CHECK_POOLED_ONLY) {
    delete cf;
    cout << "TRUE" << endl;
    exit(0);
  }

  // Get the spike data
  try {
    if (numChannels > 0) {
      SpikeList **spikes = new SpikeList*[numChannels];
      for (i=0;i<numChannels;i++)
        spikes[i] = new SpikeList();

      if (cf->getSpikeData(argv[1],&numChannels,spikes)) {
        for (i=0;i<numChannels;i++) {
          if (!(spikes[i]->isLoaded())) {
            cout << "Error: Problem Loading Spike Data For Channel " 
                 << spikes[i]->getChannel() << endl;
            exit(0);
          }
        }
      } else {
        cout << "Error: Spike Data Not Found on tank " << argv[1] << endl;
        exit(0);
      }
    } else {
      cout << "NumChannels == 0" << endl;
      exit(0);
    }
  } catch (...) {
    cout << "Error: problem while gathering spike data" << endl;
    exit(0);
  }

  delete cf;
  cout << "TRUE" << endl;
  exit(0);
}
