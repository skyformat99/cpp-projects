#include <stdio.h>

#include <string>
#include <iostream>

#include "PooledSort.h"
#include "SpikeList.h"
#include "ByteBuffer.h"
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "ClientFarm.h"

#include "TankServerDefs.h"

using namespace std;

void processConnection (ClientSocket *, char*);
void testConnection (ClientSocket *);

/**
 * Usage: testClient <tank> <server>
 */
int main (int argc, char* argv[]) {

  // Create and connect client
  ClientSocket *cs;

  try {
    cs = new ClientSocket(argv[2],13000);
  } catch (char *c) {
    cout << "Error connecting: " << c << endl;
    exit(1);
  }

  cout << "Client connected ... " << endl;

//  testConnection(cs);
  processConnection(cs,argv[1]);

  // Disconnect and finalize
  cs->disconnectSocket();
  delete cs;

  exit(0);
}

void processConnection (ClientSocket *cs, char* tnk) 
{
  string *tank = new string(tnk);
  int iResult;
  ByteBuffer *bb = new ByteBuffer();
  unsigned char flag;

  // First try to find the tank file
  bb->putByte(CLIENT_FIND_TANK);
  bb->putString(*tank);

  cs->sendBB(bb);
  cs->recvACK();

  bb->reset();
  cs->recvBB(bb);
  cs->sendACK();

  bb->getByte(&flag);
  if (flag == SERVER_TANK_FOUND)
    cout << "Tank file found on server: " << *(bb->getString()) << endl;
  else
    cout << "Tank file not found on server" << endl;

  bb->reset();

  // Try to find the single unit
  bb->putByte(CLIENT_FIND_SU_DIR);
  bb->putString(*tank);

  cs->sendBB(bb);
  cs->recvACK();

  bb->reset();
  cs->recvBB(bb);
  cs->sendACK();

  bb->getByte(&flag);
  if (flag == SERVER_SU_DIR_FOUND) 
    cout << "Single unit dir found on server!" << endl;
  else
    cout << "Single unit dir not found on server!" << endl;

  bb->reset();

  // Now we go for the pooling data, if we can get it
  bb->putByte(CLIENT_FIND_POOLING);
  bb->putString(*tank);

  cs->sendBB(bb);
  cs->recvACK();

  bb->reset();
  cs->recvBB(bb);
  cs->sendACK();

  PooledSort ps;

  bb->getByte(&flag);
  int errCode;
  if (flag == SERVER_POOL_FOUND) {
    ps.unserialize(bb);
    cout << "Pooling found on server! numChannels: " 
         << ps.numActiveChannels() << endl;

  } else if (flag == SERVER_POOL_ERROR) {
    bb->getInt(&errCode);
    cout << "Pooling error (code # " << errCode << ")" << endl;

  } else {
    cout << "Pooling not found on server!" << endl;
  }

  bb->reset();

  int i,numChannels;

  // Now go for the spikeList data!
  if (ps.isLoaded()) {
    bb->putByte(CLIENT_FIND_SPIKELISTS);
    bb->putString(*tank);

    cs->sendBB(bb);
    cs->recvACK();

    bb->reset();

    cs->recvBB(bb);
    cs->sendACK();

    SpikeList **spikes;

    bb->getByte(&flag);
    if (flag == SERVER_SPIKES_FOUND) {
      cout << "Spikes found on server!" << endl;

       bb->getInt(&numChannels);
       spikes = new SpikeList*[numChannels];
       for (i=0;i<numChannels;i++) {
         spikes[i] = new SpikeList();
         spikes[i]->unserialize(bb);

         if (spikes[i]->isLoaded())
           cout << "  SpikeList for Channel " << spikes[i]->getChannel() 
                << " loaded!\t(Spikes: " << spikes[i]->getNumSpikes() << "\tBlocks: " 
                << spikes[i]->getNumBlocks() << ")" << endl;
         else
           cout << "  SpikeList for Channel " << spikes[i]->getChannel() << " NOT loaded!" << endl;
       }
       
    } else {
      cout << "Spikes not found on server!" << endl;
    }

    bb->reset();
  }

  // Now disconnect
  bb->putByte((unsigned char)CLIENT_DISCONNECT);

  cs->sendBB(bb);
  cs->recvACK();

  delete bb;
  delete tank;
}

void testConnection (ClientSocket *cs) 
{
  int iResult;
  ByteBuffer *bb = new ByteBuffer();

  int* myInt = new int[4];
  myInt[0] = 23595;
  myInt[1] = 9306;
  myInt[2] = -24953;
  myInt[3] = -56;
  bb->putInt(myInt,4);
  bb->putBool(false);
  bb->putString("this is a test string");
  bb->putFloat(3.14159);

  iResult = cs->sendBB(bb);

  cout << "Sent " << iResult << " bytes!" << endl;

  delete bb;
}