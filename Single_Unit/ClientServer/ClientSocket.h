#ifndef __clientSocket
#define __clientSocket

#include "Socket.h"
#include "ByteBuffer.h"
#include "TankServerDefs.h"

#include <string>
#include <sstream>

using std::string;
using std::stringstream;

class ClientSocket : public Socket {

private:

  string host;
  int port;

  struct addrinfo *result,
                  *ptr,
                  hints;

  void initWinsock ();

public:

  ClientSocket ();
  ClientSocket (string,int);
  ~ClientSocket ();

  void connectSocket (string,int);
  string getHost ();
  int getPort ();
  bool ping ();
  bool trySocket (string,int,int);
};

#endif