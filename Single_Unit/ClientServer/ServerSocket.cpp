#include "ServerSocket.h"

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

/**
 * Constructs a new serverSocket object.
 */
ServerSocket::ServerSocket () 
{
  initWinsock();
}

/**
 * Create a new serverSocket object, and immediately 
 * bind to a port.
 */
ServerSocket::ServerSocket (int port)
{
  initWinsock();

  bindToPort(port);
}

/**
 * Descructs a serverSocket.
 */
ServerSocket::~ServerSocket ()
{
  try {
    closesocket(ListenSocket);
  } catch (string s) {}

  Socket::~Socket();
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

/**
 * Binds to a specified port. Does not begin listening for
 * a connection, however.
 */
void ServerSocket::bindToPort (int port) 
{
  int iResult;
  stringstream ss;
  ss.str("");
  ss << port;

  // Resolve the server address and port
  iResult = getaddrinfo(NULL, ss.str().c_str(), &hints, &result);
  if ( iResult != 0 )
    throw "bindToPort(): getaddrinfo failed";

  // Create a SOCKET for connecting to server
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    freeaddrinfo(result);
    throw "bindToPort(): Socket failed";
  }

  // And bind the TCP listening socket to the right address
  iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    freeaddrinfo(result);
    throw "bindToPort(): Bind failed";
  }

  freeaddrinfo(result);
}

/**
 * Instructs the listening socket to wait for clients to connect.
 * This method blocks until a client connects.
 */
void ServerSocket::listenForConnection () 
{
  int iResult;

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) 
    throw new string("Listen failed: ");

  // Accept a client socket
  mainSocket = accept(ListenSocket, NULL, NULL);
  if (mainSocket == INVALID_SOCKET) 
    throw new string("Accept failed: ");

  // No longer need server socket
  closesocket(ListenSocket);
}

// * * * * * * * * * * * * * * * *
// * Private Methods 
// * * * * * * * * * * * * * * * *

/**
 * Initializes the winSock elements.
 */
void ServerSocket::initWinsock ()
{
  try {
    Socket::initWinsock();
  } catch (...) {
    throw "initWinsock(): error initializing WinSock";
  }

  int iResult;

  this->ListenSocket = INVALID_SOCKET;

  this->result = NULL;

  // Initialize 'hints' structure
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
}