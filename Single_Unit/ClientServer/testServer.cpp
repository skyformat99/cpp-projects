#include <string>
#include <iostream>
#include "ByteBuffer.h"
#include "ServerSocket.h"

#include "TankCollection.h"
#include "TankServerDefs.h"

/** Forces the tankCollection to print out the tank dir's */
#define DEBUG1 false

/** Forces the printing of the tankName for queries */
#define DEBUG2 false

/** Forces printing of all recv commands */
#define DEBUG3 false

/** Forces detailed printing of Find_Spikelists query */
#define DEBUG4 false

/** Default directory file **/
#define DEFAULT_DIR_FILE "c:/usr/cpp/Single_Unit/ClientServer/dirs.txt"

using namespace std;

void parsePathForTank (string, string*);
void processConnection (ServerSocket*,TankCollection*);

int main (int argc, char* argv[]) {

  string *dirFile;
  if (argc >= 2)
    dirFile = new string(argv[1]);
  else
    dirFile = new string(DEFAULT_DIR_FILE);

  // Set up TankCollection
  TankCollection *tc;
  try {
    tc = new TankCollection(*dirFile);
  } catch (char *c) {
    cout << "Error creating TankCollection: " << c << endl;
    exit(1);
  }

  if (DEBUG1) tc->printDirectories();

  // Set up serverSocket
  ServerSocket *ss;
  try {
    ss = new ServerSocket();
  } catch (char *c) {
    cout << "Error creating ServerSocket: " << c << endl;
    exit(1);
  }

  // Loop forever, listening and processing connections
  while (true) {

    // First try to listen; if we can't do that, just exit
    try {
      cout << "Listening for connection ..." << endl;
      ss->bindToPort(13000);
      ss->listenForConnection();
      cout << "Server Connected ..." << endl;
    } catch (char *c) {
      cout << "Error listening for a connection: " << c << endl;
      exit(1);
    } catch (...) {
      cout << "Error listening for a connection ..." << endl;
      exit(1);
    }

    // Now try processing the connections
    try {
      processConnection(ss,tc);
    } catch (char *c) { 
      cout << "Error processing connections: " << c << endl;
    } catch (...) {
      cout << "Error processing connections ..." << endl;
    }

    // And lastly try disconnecting
    try {
      ss->disconnectSocket();
    } catch (char *c) {
      cout << "Error disconnecting socket: " << c << endl;
      continue;
    } catch (...) {
      cout << "Error disconnecting socetk ..." << endl;
    }
  }

  // Finalize
  delete ss;
  delete tc;

  exit(0);
}

/**
 * Handles connection-processing at the server level. Receives
 * byteBuffers until the client shuts down, either gracefully or 
 * not-so-gracefully.
 */
void processConnection (ServerSocket *ss, TankCollection *tc) 
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

  int numAdded;
  vector<string> tankVector;
  vector<string>::iterator it;

  // Receive until the peer shuts down the connection
  while (isConnected) {

    // First thing should be the byte flag
    iResult = ss->recvBB(bb);
    if (iResult == SOCKET_ERROR) 
      throw "processConn(): no byte flag";
    bb->getByte(&flag);

    // And acknowledge that we received something
    ss->sendACK();

    switch (flag) {

      case SOCKET_PING: 
        // do nothing, ACK is good enough
        break;

      case CLIENT_DISCONNECT:
        if (DEBUG3) cout << "recv CLIENT_DISCONNET" << endl;
        isConnected = false;
        break;

      case CLIENT_FIND_TANK:
        if (DEBUG3) cout << "recv CLIENT_FIND_TANK" << endl;

        tankName = bb->getString();
        bb->reset();

        if (DEBUG2) cout << "\tTank name: " << *tankName << endl;

        isFound = tc->findTankFiles(*tankName,&tankDir);

        if (isFound) {
          bb->putByte(SERVER_TANK_FOUND);
          bb->putString(tankDir);

        } else
          bb->putByte(SERVER_TANK_NOT_FOUND);

        iResult = ss->sendBB(bb);
        if (iResult == SOCKET_ERROR) 
          throw "processConn(): send tank data failed";
        ss->recvACK();

        break;

      case CLIENT_FIND_TANKS:
        if (DEBUG3) cout << "recv CLIENT_FIND_TANKS" << endl;

        tankName = bb->getString();
        bb->reset();
 
        if (DEBUG2) cout << "\tTank name: " << *tankName << endl;

        numAdded = tc->findTankFiles(*tankName,&tankVector);

        if (numAdded > 0) {
          bb->putByte(SERVER_TANK_FOUND); 
          bb->putInt((int)tankVector.size());

          for ( it = tankVector.begin() ; it < tankVector.end(); it++ ) {
            try {
              parsePathForTank(*it,tankName);
              bb->putString(*tankName);
            } catch (...) {
              bb->putString("ERROR PARSING");
            }
          }

        } else
           bb->putByte(SERVER_TANK_NOT_FOUND);

        iResult = ss->sendBB(bb);
        if (iResult == SOCKET_ERROR)
          throw "processConn(): send tank data failed";
        ss->recvACK();

        break;

      case CLIENT_FIND_SU_DIR:
        if (DEBUG3) cout << "recv CLIENT_FIND_SU_DIR" << endl;
        
        tankName = bb->getString();
        bb->reset();

        if (DEBUG2) cout << "\tTank name: " << *tankName << endl;

        isFound = tc->findValidSingleUnitDir(*tankName,&suDir);

        if (isFound) {
          bb->putByte(SERVER_SU_DIR_FOUND);
          bb->putString(suDir);

        } else 
          bb->putByte(SERVER_SU_DIR_NOT_FOUND);

        iResult = ss->sendBB(bb);
        if (iResult == SOCKET_ERROR)
          throw "processConn(): send SU dir failed";
        ss->recvACK();

        break;

      case CLIENT_FIND_POOLING:
        if (DEBUG3) cout << "recv CLIENT_FIND_POOLING" << endl;

        tankName = bb->getString();
        bb->reset();

        if (DEBUG2) cout << "\tTank name: " << *tankName << endl;

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
//            ps->serialize(bb);

          } else if (isFound) {
            bb->putByte(SERVER_POOL_ERROR);
//            bb->putInt(ps->getErrCode());

          } else {
            bb->putByte(SERVER_POOL_NOT_FOUND);
          }
        }

        iResult = ss->sendBB(bb);
        if (iResult == SOCKET_ERROR)
          throw "processConn(): findPooling result failed";
        ss->recvACK();

        break;

      case CLIENT_FIND_SPIKELISTS:
        if (DEBUG3) cout << "recv CLIENT_FIND_SPIKELISTS" << endl;

        tankName = bb->getString();
        bb->reset();

        if (DEBUG2) cout << "\tTank name: " << *tankName << endl;

        // If this is called before "FIND_SU_DIR", try again!
        if (!isFound)
          isFound = tc->findValidSingleUnitDir(*tankName,&suDir);

        if (DEBUG4)
          cout << "  Finished finding SU directory: " << suDir << endl;

        // If this is called before PooledSort is loaded, try loading
        if (isFound && !ps->isLoaded()) {
          if (DEBUG4) cout << "  Pooling isn't loaded!" << endl;
          isFound = tc->findPooling(*tankName,suDir,*ps);
        }

        if (DEBUG4)
          cout << "  Finished finding pooling ..." << endl;

        if (!isFound || !ps->isLoaded())
          bb->putByte(SERVER_SPIKES_NOT_FOUND);
        else {
          numChannels = ps->numActiveChannels();

          if (DEBUG4)
            cout << "  Found pooling: numChannels: " << numChannels << endl;

          // Declare and initialize SpikeList
          spikes = new SpikeList*[numChannels];
          for (i=0;i<numChannels;i++)
            spikes[i] = new SpikeList();

          if (DEBUG4)
            cout << "  Declared empty SpikeLists, ready to find ..." << endl;

          isFound = tc->findSpikes(*tankName,suDir,spikes,ps);

          if (isFound) {
            if (DEBUG4)
              cout << "  Found SpikeLists! Sending ..." << endl;

            bb->putByte(SERVER_SPIKES_FOUND);
            bb->putInt(numChannels);
//            for (i=0;i<numChannels;i++) 
//              spikes[i]->serialize(bb);

          } else {
            if (DEBUG4)
              cout << "  Didn't find SpikeLists!" << endl;

            bb->putByte(SERVER_SPIKES_NOT_FOUND);
          }

          for (i=0;i<numChannels;i++)
            delete spikes[i];
          delete spikes;
        }

        iResult = ss->sendBB(bb);
        if (iResult == SOCKET_ERROR)
          throw "processConn(): findSpikes result failed";
        ss->recvACK();

        break;

      default:
        throw "processConn(): Unknown recv flag";
    }

    bb->reset();
  }

  delete bb;
  delete ps;
}

/**
 * Pulls the tank name out of a full path of the tank.
 */
void parsePathForTank (string path, string* tank) 
{
  if (path.substr(path.size()-4) != ".tev")
    throw "Error parsing string!\n";

  int pos_back, pos_for, pos;
  if ((pos_back = path.find_last_of("/")) == string::npos)
    pos_back = 0;

  if ((pos_for = path.find_last_of("\\")) == string::npos)
    pos_for = 0;

  pos = (pos_back > pos_for) ? pos_back : pos_for;

  *tank = path.substr(pos+1,7);

  return;
}