#ifndef __byteBuffer
#define __byteBuffer

#include <string>
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

//#define INITIAL_SIZE 256
#define INITIAL_SIZE 4194304

using namespace std;

class ByteBuffer {

private:

  /** Private Fields */
  unsigned char* buf;

  // The current allocated size of the buffer
  long maxSize;

  // The current populated size of the buffer 
  long size;

  // The current position in the buffer, used for reads 
  long index;

  // The number by which we multiply the current buffer on increases 
  int increase_multiplier;

  /** Private Methods */
  void increaseBuffer ();

public:

  /** Constructors & Destructors */
  ByteBuffer ();
  ByteBuffer (long);
  ~ByteBuffer ();

  int fillBuffer (SOCKET);

  long getBufferSize ();
  long getMaxSize ();

  /** Collection of "GET" functions */
  void getBool (bool*);
  void getBool (bool*, int);
  void getByte (unsigned char*);
  void getByte (unsigned char*, int);
  void getDouble (double*);
  void getDouble (double*,int);
  void getFloat (float*);
  void getFloat (float*, int);
  void getInt (int*);
  void getInt (int*, int);
  string* getString ();

  /** Collection of "PUT" functions */
  void putBool (bool);
  void putBool (bool*, int);
  void putByte (unsigned char);
  void putByte (unsigned char*, int);
  void putDouble (double);
  void putDouble (double*, int);
  void putFloat (float);
  void putFloat (float*, int);
  void putInt (int);
  void putInt (int*, int);
  void putString (string);

  void reset ();
  int sendBuffer (SOCKET);

  void trimToSize ();

};

#endif