#include "ClientFarm.h"

#include "ClientSocket.h"
#include "TankServerDefs.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

/** Prints out parsing of the input servers txt file */
#define DEBUG1 false


// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

/**
 * Initialize a new ClientFarm with an appropriate Server File.
 * The server file lists the hosts:ports which this client farm
 * will attempt to use.
 */
ClientFarm::ClientFarm (string srvrFile) 
{
  initServerList(srvrFile);
}

/** 
 * Deconstructs a ClientFarm by destroying the server list.
 */
ClientFarm::~ClientFarm () 
{
  ClientSocket *cs;

  while (servers.size() > 0) {
    cs = servers.back();
    servers.pop_back();

    try {
      cs->disconnectSocket();
      cs->finalize();
    } catch (...) { 
      continue;
    }
  }

  servers.clear();
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

/**
 * Determines if the tank exists on any server in the farm. 
 * If it exists, returns true. If the optional *host parameter
 * is not NULL pointer, then this will be populated with
 * the name of the host on which the tank resides.
 */
bool ClientFarm::doesTankExist (string tank, string *host, string *dir) 
{
  return doesParamExist(tank,host,dir,CLIENT_FIND_TANK,SERVER_TANK_FOUND);
}

/**
 * Determines if the SU data + pooling exists on any server
 * in the farm. If it does, returns true, else false. If
 * the optional *host parameter is not null, fills with the host
 * on which the SU data resides.
 */
bool ClientFarm::doesSingleUnitExist (string tank, string *host, string *dir) 
{
  return doesParamExist(tank,host,dir,CLIENT_FIND_SU_DIR,SERVER_SU_DIR_FOUND);
}

/**
 * Gathers pooling data from any available host, if the data
 * exists; returns 0 on success. If no data exists, returns -1.
 */
int ClientFarm::getPoolingData (string tank, PooledSort *ps)
{
  ByteBuffer *bb = new ByteBuffer();
  unsigned char flag;
  int errCode = -1, iResult;

  // Iterate through clients, looking for tank
  vector<ClientSocket*>::iterator it;
  for (it = servers.begin(); it < servers.end(); it++) {

    if (!((*it)->ping()))
      throw "Error: cannot ping server!";

    bb->putByte(CLIENT_FIND_POOLING);
    bb->putString(tank);

    (*it)->sendBB(bb);
    (*it)->recvACK();

    bb->reset();
    (*it)->recvBB(bb);
    (*it)->sendACK();

    bb->getByte(&flag);
    int errCode;
    if (flag == SERVER_POOL_FOUND) {
//      ps->unserialize(bb);
      delete bb;
      return 0;

    } else if (flag == SERVER_POOL_ERROR) {
      bb->getInt(&errCode);

    } else {
      errCode = -1;
    }

    bb->reset();
  }

  delete bb;

  return errCode;
}

/**
 * Gathers the spike data from the specified tank, and stores in 
 * the spikes array. Also stores the retrieved number of channels in
 * the numChannels field. If no server has spike data, returns false; 
 * otherwise returns true.
 */
bool ClientFarm::getSpikeData (string tank, int *numChannels, SpikeList **spikes)
{
  ByteBuffer *bb = new ByteBuffer();
  unsigned char flag;
  int i;

  // Iterate through clients, looking for tank
  vector<ClientSocket*>::iterator it;
  for (it = servers.begin(); it < servers.end(); it++) {

    if (!((*it)->ping()))
      throw "Error: cannot ping server!";

    bb->putByte(CLIENT_FIND_SPIKELISTS);
    bb->putString(tank);

    (*it)->sendBB(bb);
    (*it)->recvACK();

    bb->reset();

    (*it)->recvBB(bb);
    (*it)->sendACK();

    bb->getByte(&flag);
    if (flag == SERVER_SPIKES_FOUND) {

      bb->getInt(numChannels);
      for (i=0;i<(*numChannels);i++) {
//        spikes[i]->unserialize(bb);
      }

      delete bb;
      return true;
    }

    bb->reset();
  }

  delete bb;
  return false;
}

/**
 * Gathers the unique tank files from across all servers, and populates
 * the passed vector with these unique names.
 */
int ClientFarm::getTankList (string base, vector<string> *tanks) 
{
  ByteBuffer *bb = new ByteBuffer();
  unsigned char flag;
  int i, numElements, totalElements = 0;
  vector<string> tempVector;

  // Iterate through clients, looking for tank
  vector<ClientSocket*>::iterator it;
  for (it = servers.begin(); it < servers.end(); it++) {

    if (!((*it)->ping()))
      throw new string("Error: cannot ping server!");

    bb->putByte(CLIENT_FIND_TANKS);
    bb->putString(base);

    (*it)->sendBB(bb);
    (*it)->recvACK();

    bb->reset();
    (*it)->recvBB(bb);
    (*it)->sendACK();

    bb->getByte(&flag);
    if (flag == SERVER_TANK_FOUND) {
      bb->getInt(&numElements);

      tempVector.clear();

      for (i=0;i<numElements;i++) 
        tempVector.push_back(*(bb->getString()));

      totalElements += VectorUtilities::mergeUniqueElements(tanks,&tempVector);
    }

    bb->reset();
  }

  delete bb;
  return totalElements;
}


/**
 * Sends the DISCONNECT flag to all servers on the clientFarm's list.
 */
void ClientFarm::sendAllDisconnect () 
{
  ByteBuffer *bb = new ByteBuffer();
  unsigned char flag;
  int i;

  // Iterate through clients, looking for tank
  vector<ClientSocket*>::iterator it;
  for (it = servers.begin(); it < servers.end(); it++) {

    if (!((*it)->ping()))
      throw "Error: cannot ping server!";

    bb->putByte(CLIENT_DISCONNECT);

    (*it)->sendBB(bb);
    (*it)->recvACK();

    bb->reset();
  }

  delete bb;
  return;
}


// * * * * * * * * * * * * * * * *
// * Private Methods 
// * * * * * * * * * * * * * * * *

/**
 * Searches for either a Tank or a SU Directory + Pool, depending on
 * the input bytes c_b and s_b. The c_b parameter corresponds to the Client
 * byte (i.e. either CLIENT_FIND_SU_DIR or CLIENT_FIND_TANK), and the s_b
 * parameter corresponds to the Server byte (i.e. either SERVER_SU_DIR_FOUND 
 * or SERVER_TANK_FOUND).
 */
bool ClientFarm::doesParamExist (string tank, string *host, string *dir, 
    byte c_b, byte s_b)
{
  ByteBuffer *bb = new ByteBuffer();
  unsigned char flag;

  // Iterate through clients, looking for tank
  vector<ClientSocket*>::iterator it;
  for (it = servers.begin(); it < servers.end(); it++) {

    if (!((*it)->ping()))
      throw new string("Error: cannot ping server!");

    bb->putByte(c_b);
    bb->putString(tank);

    (*it)->sendBB(bb);
    (*it)->recvACK();

    bb->reset();
    (*it)->recvBB(bb);
    (*it)->sendACK();

    bb->getByte(&flag);
    if (flag == s_b) {

      *dir = *(bb->getString());

      if (host != NULL)
        *host = (*it)->getHost();

      delete bb;
      return true;
    }

    bb->reset();
  }

  delete bb;
  return false;
}

/**
 * Reads the list of server hosts:ports from the given file,
 * and adds them to the vector. 
 */
void ClientFarm::initServerList (string srvrFile) 
{
  servers.clear();

  string hp_string;
  string::size_type loc;
  string host; long port;
  ClientSocket *cs;

  // Open the file stream
  fstream* filestr;
  try {
    filestr = new fstream(srvrFile.c_str(), fstream::in);  
  } catch (...) {
    throw "Error opening fstream to file";
  }

  if (!filestr->is_open())
    throw "Error opening property file!";

  // Gets the first line
  try {
    getline(*filestr,hp_string);
  } catch (...) {
    throw "Error reading lines from file";
  }

  // Loop through the istream until EOF is reached
  while (!filestr->eof()) {

    // If the read line is "", we're done
    if (hp_string.length() == 0)
      break;

    // If we have a host and port, separated by a colon
    if ((loc = hp_string.find(":",0)) != string::npos ) {

      host = hp_string.substr(0,loc);

      port = atol(hp_string.substr(loc+1).c_str());

      if (DEBUG1) cout << "Host: #" << host << "# Port: #" << port << "#" << endl;

      // Read the next line
      try {
        getline(*filestr,hp_string);
      } catch (...) {
        throw "Error reading next line from fstream";
      }

      // Make the connection, and push to the vector
      try {
        cs = new ClientSocket(host,port);
        servers.push_back(cs);

      } catch (char *c) {
        cout << "Error connecting to " << host << ": " << c << endl;
        continue;

      } catch (...) {
        continue;
      }
    }
  }

  try {
    filestr->close();
    delete filestr;
  } catch (...) {
    throw "Error closing fstream";
  }
}