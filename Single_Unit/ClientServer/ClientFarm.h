#ifndef __clientFarm
#define __clientFarm

#include "ClientSocket.h"
#include "PooledSort.h"
#include "SpikeList.h"
#include "VectorUtilities.h"

#include <vector>
#include <string>

using namespace std;

class ClientFarm {

private:
  vector<ClientSocket*> servers;

  void initServerList (string);
  bool doesParamExist (string, string*, string*, byte, byte);

public:
  ClientFarm (string);
  ~ClientFarm ();

  bool doesTankExist (string, string*, string*);
  bool doesSingleUnitExist (string, string*, string*);

  int getPoolingData (string, PooledSort*);
  bool getSpikeData (string, int*, SpikeList**);
  int getTankList (string, vector<string>*);

  void sendAllDisconnect ();

};

#endif