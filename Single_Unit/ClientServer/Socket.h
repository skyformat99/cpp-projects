#ifndef __socket
#define __socket

#include "ByteBuffer.h"

#include <winsock2.h>
#include <ws2tcpip.h>
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib

#define ACK_VALUE -999

class Socket {

protected:

  WSADATA wsaData;
  SOCKET mainSocket;

  void initWinsock ();

public:

  ~Socket ();

  void disconnectSocket ();
  void finalize ();

  int recvBB (ByteBuffer*);
  void recvACK ();

  int sendBB (ByteBuffer*);
  void sendACK ();

};

#endif