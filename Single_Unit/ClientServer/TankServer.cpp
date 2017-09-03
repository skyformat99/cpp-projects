#include <string>
#include <iostream>

#include "TankServer.h"

#include "ByteBuffer.h"
#include "TankServerDefs.h"

using namespace std;

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

TankServer::TankServer () {
  tc = NULL;
  ss = NULL;
}

TankServer::~TankServer () {
  if (tc != NULL)
    delete tc;

  if (ss != NULL)
    delete ss;
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

void TankServer::run (int port) 
{
  cout << "Listening for connection ..." << endl;

  ss->bindToPort(13000);
  ss->listenForConnection();

  cout << "Server Connected ..." << endl;

  processConnection(ss,&tc);

  ss->disconnectSocket();
}

// * * * * * * * * * * * * * * * *
// * Private Methods 
// * * * * * * * * * * * * * * * *

void TankServer::processConnection (ServerSocket *ss, TankCollection *tc) 
{
  int i, iResult;
  ByteBuffer *bb = new ByteBuffer();

  bool isConnected = true;
  unsigned char flag;
  string *tankName;
  string suDir, tankDir;
  bool isFound;
  PooledSort *ps = new PooledSort();
  SpikeList **spikes = NULL;
  int numChannels = -1;

  // Receive until the peer shuts down the connection
  while (isConnected) {

    // First thing should be the byte flag
    iResult = ss->recvBB(bb);
    bb->getByte(&flag);

    // And acknowledge that we received something
    ss->sendACK();

    switch (flag) {

      case CLIENT_DISCONNECT:
        cout << "recv CLIENT_DISCONNET" << endl;
        isConnected = false;
        break;

      case CLIENT_FIND_TANK:
        cout << "recv CLIENT_FIND_TANK" << endl;

        tankName = bb->getString();
        bb->reset();

        isFound = tc->findTankFiles(*tankName,&tankDir);

        if (isFound) {
          bb->putByte(SERVER_TANK_FOUND);
          bb->putString(tankDir);
        } else
          bb->putByte(SERVER_TANK_NOT_FOUND);

        ss->sendBB(bb);
        ss->recvACK();

        break;

      case CLIENT_FIND_SU_DIR:
        cout << "recv CLIENT_FIND_SU_DIR" << endl;
        
        tankName = bb->getString();
        bb->reset();

        isFound = tc->findValidSingleUnitDir(*tankName,&suDir);

        if (isFound)
          bb->putByte(SERVER_SU_DIR_FOUND);
        else
          bb->putByte(SERVER_SU_DIR_NOT_FOUND);

        ss->sendBB(bb);
        ss->recvACK();

        break;

      case CLIENT_FIND_POOLING:
        cout << "recv CLIENT_FIND_POOLING" << endl;

        tankName = bb->getString();
        bb->reset();

        // If this is called before "FIND_SU_DIR" try finding again
        if (!isFound)
          isFound = tc->findValidSingleUnitDir(*tankName,&suDir);

        // Still not found? return an error
        if (!isFound) {
          bb->putByte(SERVER_POOL_NOT_FOUND);
        } else {
          isFound = tc->findPooling(*tankName,suDir,*ps);

          if (isFound && ps->isLoaded()) {
            bb->putByte(SERVER_POOL_FOUND);
            ps->serialize(bb);

          } else if (isFound) {
            bb->putByte(SERVER_POOL_ERROR);
            bb->putInt(ps->getErrCode());

          } else {
            bb->putByte(SERVER_POOL_NOT_FOUND);
          }
        }

        ss->sendBB(bb);
        ss->recvACK();

        break;

      case CLIENT_FIND_SPIKELISTS:
        cout << "recv CLIENT_FIND_SPIKELISTS" << endl;

        tankName = bb->getString();
        bb->reset();

        // If this is called before "FIND_SU_DIR", try again!
        if (!isFound)
          isFound = tc->findValidSingleUnitDir(*tankName,&suDir);

        // If this is called before PooledSort is loaded, try loading
        if (!ps->isLoaded())
          isFound = tc->findPooling(*tankName,suDir,*ps);

        if (!isFound || !ps->isLoaded())
          bb->putByte(SERVER_SPIKES_NOT_FOUND);
        else {
          numChannels = ps->numActiveChannels();

          // Declare and initialize SpikeList
          spikes = new SpikeList*[numChannels];
          for (i=0;i<numChannels;i++)
            spikes[i] = new SpikeList();

          isFound = tc->findSpikes(*tankName,suDir,spikes,ps);

          if (isFound) {
            bb->putByte(SERVER_SPIKES_FOUND);
            bb->putInt(numChannels);
            for (i=0;i<numChannels;i++) 
              spikes[i]->serialize(bb);

          } else {
            bb->putByte(SERVER_SPIKES_NOT_FOUND);

          }
        }

        ss->sendBB(bb);
        ss->recvACK();

        break;

      default:
        cout << "UNKNOWN recv: " << flag << endl;
        break;
    }

    bb->reset();
  }

  // Must free memory assocated with this string
  delete &suDir;

  delete bb;
  delete ps;
  if (spikes != NULL) {
    for (i=0;i<numChannels;i++)
      delete spikes[i];
    delete spikes;
  }
}
