#include "ClientSocket.h"

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

/**
 * Creates a new ClientSocket object and initializes
 * the winSock elements.
 */
ClientSocket::ClientSocket ()
{
  try {
    initWinsock();
  } catch (...) {
    throw "Error initializing WinSock!";
  }
}

/**
 * Creates a new ClientSocket object, initializes
 * the winSock elements, and connects to the given host.
 */
ClientSocket::ClientSocket (string host, int port) 
{
  try {
    initWinsock();
  } catch (...) {
    throw "Error initializing WinSock!";
  }

  this->host = host;
  this->port = port;

  connectSocket(host,port);
}

/**
 * Finalizes the clientSocket and cleans up WinSock.
 */
ClientSocket::~ClientSocket ()
{
  Socket::~Socket();
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

/**
 * Connects to the given host / port. Throws a string error
 * if the connection fails.
 */
void ClientSocket::connectSocket (string host, int port)
{
  int iResult;
  stringstream ss;
  ss.str("");
  ss << port;

  // Resolve the server address and port
  iResult = getaddrinfo(host.c_str(), ss.str().c_str(), &hints, &result);
  if ( iResult != 0 ) 
    throw "connectSocket() getaddrinfo failed!";

  // Create a SOCKET for connecting to server
  mainSocket = socket(result->ai_family, result->ai_socktype, 
                      result->ai_protocol);

  if (mainSocket == INVALID_SOCKET) {
    freeaddrinfo(result);
    throw "connectSocket() invalid socket";
  }

  // Connect to server.
  iResult = connect( mainSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) 
    throw "connectSocket() error connecting";

  freeaddrinfo(result);
}

/** 
 * Returns string representation of the host.
 */
string ClientSocket::getHost ()
{
  return this->host;
}

/**
 * Returns int representation of the port.
 */
int ClientSocket::getPort () 
{
  return this->port;
}

/**
 * Pings the current serverConnection to see if it's active.
 */
bool ClientSocket::ping ()
{
  ByteBuffer *bb = new ByteBuffer();

  try {
    bb->putByte(SOCKET_PING);

    sendBB(bb);
    recvACK();

    return true;
  } catch (...) {}

  return false;
}

/**
 * Creates a non-blocking socket and attempts to connect to the given host/port. 
 * Sleeps for the given timeOut period (in ms), and then checks if the socket is
 * writeable. This is useful for pinging servers; if we didn't use this, we'd have
 * to wait the full TCP timeout (OS-dependent) on a false result. This method allows
 * us to avoid that wait by specifying our own timeout. 
 */
bool ClientSocket::trySocket (string host, int port, int timeout)
{
  bool returnVal = false;
  int iResult;
  stringstream ss;
  ss.str("");
  ss << port;

  // Resolve the server address and port
  iResult = getaddrinfo(host.c_str(), ss.str().c_str(), &hints, &result);
  if ( iResult != 0 ) 
    throw "trySocket() getaddrinfo failed!";

  // Create a SOCKET for connecting to server
  SOCKET testSocket = socket(result->ai_family, result->ai_socktype, 
                      result->ai_protocol);

  if (testSocket == INVALID_SOCKET) {
    freeaddrinfo(result);
    throw "trySocket() invalid socket";
  }

  u_long iMode = 1;
  ioctlsocket(testSocket, FIONBIO, &iMode);

  // Connect to server.
  iResult = connect( testSocket, result->ai_addr, (int)result->ai_addrlen);

  if (iResult != SOCKET_ERROR)
    throw "trySocket() error on non-blocking connect!";

  if (WSAGetLastError() != WSAEWOULDBLOCK)
    throw "trySocket() error statement incorrect";

  Sleep(timeout);

  fd_set *checkSet = new fd_set();
  FD_ZERO(checkSet);
  FD_SET(testSocket,checkSet);

  timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  iResult = select(NULL,NULL,checkSet,NULL,&tv);

  if (iResult == SOCKET_ERROR)
    throw "trySocket() select error";

  if (FD_ISSET(testSocket,checkSet))
    returnVal = true;
  
  freeaddrinfo(result);

  try {
    closesocket(testSocket);
  } catch (...) {}

  return returnVal;
}

// * * * * * * * * * * * * * * * *
// * Private Methods 
// * * * * * * * * * * * * * * * *

/**
 * Initializes the winSock components.
 */
void ClientSocket::initWinsock ()
{
  Socket::initWinsock();

  result = NULL;
  ptr = NULL;

  // Init hints structure
  ZeroMemory( &hints, sizeof(hints) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}