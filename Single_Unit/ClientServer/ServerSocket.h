#ifndef __serverSocket
#define __serverSocket

#include "Socket.h"

#include <string>
#include <sstream>

using std::string;
using std::stringstream;

class ServerSocket : public Socket {

private:

  SOCKET ListenSocket;
  struct addrinfo *result, 
                  hints;

  void initWinsock ();

public:

  ServerSocket ();
  ServerSocket (int);
  ~ServerSocket ();

  void bindToPort (int);

  void listenForConnection ();
};

#endif