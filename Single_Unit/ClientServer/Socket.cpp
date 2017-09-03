#include "Socket.h"

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

Socket::~Socket () {

  finalize();
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

void Socket::disconnectSocket () 
{
  int iResult;

  // shutdown the connection since no more data will be sent
  iResult = shutdown(mainSocket, SD_SEND);

  if (iResult == SOCKET_ERROR) 
    throw "disconnectSocket(): Shutdown failed";
}

/**
 * Finalizes the socket.
 */
void Socket::finalize ()
{
  try {
    closesocket(mainSocket);
  } catch (...) {}

  WSACleanup();
}

/**
 * Receives information from the underlying socket, and passes 
 * to the byteBuffer.
 */
int Socket::recvBB (ByteBuffer *bb)
{
  int iResult;

  iResult = bb->fillBuffer(mainSocket);

  if (iResult == SOCKET_ERROR) {
    closesocket(mainSocket);
    throw new string("Socket error: recv failed");
  }

  return iResult;
}

/**
 * Blocking receive of an acknowledgement packet.
 * Only returns when the packet has been received.
 */
void Socket::recvACK ()
{
  int iResult, ack;

  unsigned char* ack_buf = new unsigned char[sizeof(int)];
  iResult = recv(mainSocket,(char*)ack_buf,sizeof(int),0);

  if (iResult != sizeof(int))
    throw new string("Error on ACK recv!");

  memcpy(&ack,ack_buf,sizeof(int));

  if (ack != ACK_VALUE)
    throw new string("Error on ACK recv!");
}

/**
 * Sends the contents of the ByteBuffer through the given 
 * socket. Returns the number of bytes sent.
 */
int Socket::sendBB (ByteBuffer *bb)
{
  int iResult;

  iResult = bb->sendBuffer(mainSocket);

  if (iResult == SOCKET_ERROR) {
    closesocket(mainSocket);
    throw new string("Socket error: send failed");
  }

  return iResult;
}

/**
 * Sends an acknowledgement packet to show that
 * the data has been received.
 */
void Socket::sendACK ()
{
  int iResult, ack = ACK_VALUE;

  unsigned char* ack_buf = new unsigned char[sizeof(int)];
  memcpy(ack_buf,&ack,sizeof(int));

  iResult = send(mainSocket,(char*)ack_buf,sizeof(int),0);

  if (iResult != sizeof(int))
    throw new string("Error: ACK sent incorrectly!");
}

// * * * * * * * * * * * * * * * *
// * Protected Methods 
// * * * * * * * * * * * * * * * *

void Socket::initWinsock ()
{
  int iResult;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) 
    throw new string("WSAStartup failed!");

  this->mainSocket = INVALID_SOCKET;
}