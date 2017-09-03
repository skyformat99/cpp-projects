#include "ByteBuffer.h"

/** Display verbose packet-debugging info for each send/recv */
//#define DEBUG1

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

/**
 * Declares a new ByteBuffer with a specified
 * initial size.
 */
ByteBuffer::ByteBuffer () {
  this->buf = new unsigned char[INITIAL_SIZE];

  this->maxSize = INITIAL_SIZE;
  this->size = 0;
  this->index = 0;

  this->increase_multiplier = 2;
}

/**
 * Declares a new ByteBuffer to the given size.
 */
ByteBuffer::ByteBuffer (long in_size) {
  this->buf = new unsigned char[in_size];

  this->maxSize = in_size;
  this->size = 0;
  this->index = 0;

  this->increase_multiplier = 2;
}

/**
 * Frees memory associated with byte buffer.
 */
ByteBuffer::~ByteBuffer () {
  delete this->buf;
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

/**
 * Fills the buffer by receiving all the data from the socket.
 */
int ByteBuffer::fillBuffer (SOCKET s) 
{
  int recvdData;
  int iResult = 0, totalResult = 0;
  int incoming_size;

  // First receive the size of the buffer to be sent
  unsigned char *sizeBuf = new unsigned char[sizeof(int)];
  iResult = recv(s, (char*)sizeBuf, sizeof(int), 0);

  // Check for errors on this receive
  if (iResult == 0) {
    #ifdef DEBUG1
      cout << "recv(size) closed connection gracefully" << endl;
    #endif
    return 0;
  } else if (iResult == SOCKET_ERROR) {
    #ifdef DEBUG1
      cout << "recv(size) failed with error: " << WSAGetLastError() << endl;
    #endif
    return SOCKET_ERROR;
  }

  totalResult += iResult;
  memcpy(&incoming_size,sizeBuf,sizeof(int));
  delete sizeBuf;

  // Now make sure that we have enough room in the buffer
  while (this->maxSize - this->size < incoming_size)
    increaseBuffer();

  recvdData = 0;

  int index = 0;

  // Loop until we've received all of the data ... this is dangerous! ***
  while (recvdData < incoming_size) {
    iResult = recv(s, (char *)(this->buf + this->size + recvdData), 
                     this->maxSize - this->size, 0);

    #ifdef DEBUG1
      cout << "  Iter: " << index++ << " this->size: " << this->size 
           << " recvdData: " << recvdData << " iResult: " << iResult << endl;
    #endif

    if (iResult == 0) {
      #ifdef DEBUG1
        cout << "recv(data) closed connection gracefully" << endl;
      #endif
      return 0;

    } else if (iResult == SOCKET_ERROR) {
      #ifdef DEBUG1
        cout << "recv(data) failed with error: " << WSAGetLastError() << endl;
      #endif

      if (WSAGetLastError() == WSAEFAULT) continue;

      return SOCKET_ERROR;
    }

    recvdData += iResult;
  }

  #ifdef DEBUG1
    cout << "Recv'd Size: " << recvdData << " bytes (expected " 
         << incoming_size << " bytes)" << endl;
  #endif

  totalResult += recvdData;
  this->size += recvdData;

  return totalResult;
}

/**
 * Returns the populated size of the
 * buffer, not the full allocated size of the buffer.
 */
long ByteBuffer::getBufferSize () 
{
  return this->size;
}

/**
 * Returns the max allocated size of the buffer. Used
 * when writing into the buffer, i.e. from a socket.
 */
long ByteBuffer::getMaxSize () 
{
  return this->maxSize;
}

/**
 * Reads a single bool off the buffer.
 */
void ByteBuffer::getBool (bool *b)
{
  getBool(b,1);
}

/**
 * Reads a give number of bool's off the buffer.
 */
void ByteBuffer::getBool (bool *b, int num)
{
  memcpy(b,this->buf + this->index,sizeof(bool)*num);

  this->index += ( sizeof(bool) * num );
}

/**
 * Reads a single byte off of the buffer.
 */
void ByteBuffer::getByte (unsigned char* c)
{
  getByte(c,1);
}

/**
 * Reads a given number of bytes off the buffer.
 */
void ByteBuffer::getByte (unsigned char* c, int num) 
{
  memcpy(c,this->buf + this->index,sizeof(unsigned char)*num);

  this->index += ( sizeof(unsigned char) * num );
}

/**
 * Reads a single double off the buffer.
 */
void ByteBuffer::getDouble (double *d)
{
  getDouble(d,1);
}

/**
 * Reads a given number of doubles off the buffer.
 */
void ByteBuffer::getDouble (double *d, int num)
{
  memcpy(d,this->buf + this->index,sizeof(double)*num);

  this->index += ( sizeof(double) * num );
}

/**
 * Reads a single float off the buffer.
 */
void ByteBuffer::getFloat (float *f) 
{
  getFloat(f,1);
}

/**
 * Reads a given number of floats off the buffer.
 */
void ByteBuffer::getFloat (float *f, int num) 
{
  memcpy(f,this->buf + this->index,sizeof(float)*num);

  this->index += ( sizeof(float) * num );
}

/** 
 * Reads a single integer off the buffer.
 */
void ByteBuffer::getInt (int *i) 
{
  getInt(i,1);
}

/** 
 * Reads a set number of integers off the buffer.
 */
void ByteBuffer::getInt (int *i, int num) 
{
  memcpy(i,this->buf + this->index,sizeof(int)*num);

  this->index += ( sizeof(int) * num );
}    

/**
 * Reads a char* array off the buffer, and returns as a 
 * string pointer.
 */
string* ByteBuffer::getString () 
{
  int strLen;
  getInt(&strLen);

  // Declare an empty buffer of length + 1 (for '\0')
  char *readStr = new char[strLen+1];

  // Copy into empty buffer, and null-terminate
  memcpy(readStr,this->buf + this->index,strLen);
  readStr[strLen] = '\0';

  // Increment the read index
  this->index += strLen;

  // Declare a new string to the buffer, delete the buffer,
  // and return the new string pointer
  string *s = new string(readStr);
  delete readStr;

  return s;
}

/**
 * Pushes a single bool onto the buffer.
 */
void ByteBuffer::putBool (bool value)
{
  putBool(&value,1);
}

/**
 * Pushes an array of bool's onto the buffer.
 */
void ByteBuffer::putBool (bool *values, int num)
{
  while (this->size + (sizeof(bool)*num) > this->maxSize)
    increaseBuffer();

  memcpy(this->buf + this->size,values,sizeof(bool)*num);

  this->size += sizeof(bool)*num;  
}

/**
 * Pushes a single byte onto the buffer.
 */
void ByteBuffer::putByte (unsigned char value)
{
  putByte(&value,1);
}

/**
 * Pushes an array of bytes onto the buffer.
 */
void ByteBuffer::putByte (unsigned char* values, int num)
{
  while (this->size + (sizeof(unsigned char)*num) > this->maxSize)
    increaseBuffer();

  memcpy(this->buf + this->size,values,sizeof(unsigned char)*num);

  this->size += sizeof(unsigned char)*num;
}

/**
 * Pushes a single double onto the buffer.
 */
void ByteBuffer::putDouble (double value)
{
  putDouble(&value,1);
}

/**
 * Pushes an array of doubles onto the buffer.
 */
void ByteBuffer::putDouble (double *values, int num)
{
  while (this->size + (sizeof(double)*num) > this->maxSize)
    increaseBuffer();

  memcpy(this->buf + this->size,values,sizeof(double)*num);

  this->size += sizeof(double)*num;
}

/**
 * Pushes a single float onto the buffer.
 */
void ByteBuffer::putFloat (float value) 
{
  putFloat(&value,1);
}

/**
 * Pushes an array of floats onto the buffer, using one
 * large memcpy operation.
 */
void ByteBuffer::putFloat (float *values, int num) 
{
  while (this->size + (sizeof(float)*num) > this->maxSize)
    increaseBuffer();

  memcpy(this->buf + this->size,values,sizeof(float)*num);

  this->size += sizeof(float)*num;
}

/**
 * Pushes a single integer onto the buffer.
 */
void ByteBuffer::putInt (int value) 
{
  putInt(&value,1);
}

/**
 * Pushes an array of integers onto the buffer, using one
 * large memcpy operation.
 */
void ByteBuffer::putInt (int *values, int num) 
{
  while (this->size + (sizeof(int)*num) > this->maxSize)
    increaseBuffer();

  memcpy(this->buf + this->size,values,sizeof(int)*num);

  this->size += sizeof(int)*num;
}

/**
 * Pushes a string onto the buffer. First pushes the
 * length (as an integer), then each character.
 */
void ByteBuffer::putString (string s) 
{
  // First push the length onto the buffer
  putInt(s.length());

  while (this->size + s.length() > this->maxSize)
    increaseBuffer();

  memcpy(this->buf + this->size,s.c_str(),s.length());

  this->size += s.length();
}

/**
 * Resets the byteBuffer back to its original
 * state.
 */
void ByteBuffer::reset () 
{
  delete this->buf;

  this->buf = new unsigned char[INITIAL_SIZE];

  this->maxSize = INITIAL_SIZE;
  this->size = 0;
  this->index = 0;

  this->increase_multiplier = 2;
}

/**
 * Uses the socket send (...) function to send the entire
 * buffer. Returns the total number of bytes sent.
 */
int ByteBuffer::sendBuffer (SOCKET s) 
{
  int DATA_SIZE = 4096;
  int totalResult = 0, iResult = 0, dataSentSize = 0;

  // First we need to send the size of the buffer
  unsigned char* sizeBuf = new unsigned char[sizeof(int)];
  memcpy(sizeBuf,&(this->size),sizeof(int));

  iResult = send(s, (char*)sizeBuf, sizeof(int), 0);

  if (iResult == SOCKET_ERROR) {
    #ifdef DEBUG1
      cout << "send(size) failed with error: " << WSAGetLastError() << endl;
    #endif
    return SOCKET_ERROR;
  }

  totalResult += iResult;
  delete sizeBuf;

  // Now send the actual data, in DATA_SIZE chunks
  unsigned char* sendBuf = new unsigned char[DATA_SIZE];
  int sizeToSend, mainBufferIndex = 0;

  while (mainBufferIndex < this->size) {

    // Determine what size to send: use the full DATA_SIZE, or
    // truncate if we're already near the end of the buffer
    sizeToSend = (this->size - mainBufferIndex > DATA_SIZE) 
        ? DATA_SIZE : (this->size - mainBufferIndex);

    // Send the chunk of data
    iResult = send (s, (char*)(this->buf + mainBufferIndex),sizeToSend, 0);

    // Make sure there was no SOCKET_ERROR on send
    if (iResult == SOCKET_ERROR) {
      #ifdef DEBUG1
        cout << "send(data) failed with error: " << WSAGetLastError() << endl;
      #endif
      return SOCKET_ERROR;
    }

    // Make sure the amount sent was the amount requested to be sent
    #ifdef DEBUG1
      if (iResult != sizeToSend) {
        cout << "Error: mismatch in iResult (" << iResult 
             << ") and sizeToSend (" << sizeToSend << ")" << endl;
      }
    #endif

    dataSentSize += iResult; // Increase sent amount by data sent
    mainBufferIndex += sizeToSend; // Increase main buffer index by sent amount
  }

  #ifdef DEBUG1
    cout << "Actual Sent Size: " << dataSentSize << " bytes " 
         << "(expected send size: " << this->size << " bytes)" << endl;
  #endif

  return (totalResult + dataSentSize);
}

/**
 * Trims the buffer down exactly to its size, setting
 * no extra room for more information.
 */
void ByteBuffer::trimToSize () 
{
  this->increase_multiplier = 2;
  this->maxSize = this->size;

  unsigned char* new_buf = new unsigned char[this->size];

  memcpy(new_buf,this->buf,this->size);

  delete this->buf;
  this->buf = new_buf;
}

// * * * * * * * * * * * * * * * *
// * Private Methods 
// * * * * * * * * * * * * * * * *

/**
 * Increase the buffer size, based on the current size
 * and the number of times we've had to increase already.
 *
 * Assume original size is N
 * First increase will set size = 2*N
 * Second increase will set size = 4 * (2*N) = 8*N
 * Third increase will set size = 8 * (8*N) = 64*N
 * Fourth increase will set size = 16 * (64*N) = 1024*N
 */
void ByteBuffer::increaseBuffer () 
{
  long newMax = this->maxSize * this->increase_multiplier;

  unsigned char* new_buf = new unsigned char[newMax];

  memcpy(new_buf,this->buf,this->size);

  this->increase_multiplier *= 2;
  this->maxSize = newMax;

  // Delete old buffer, set new buffer pointer
  delete this->buf;
  this->buf = new_buf;
}