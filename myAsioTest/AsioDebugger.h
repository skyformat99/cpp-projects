#ifndef __ASIO_DEBUGGER__
#define __ASIO_DEBUGGER__

#define FILENAME "c:/mlogger.txt"

#include <iostream>
#include <fstream>

using namespace std;

class AsioDebugger
{
private:
  static AsioDebugger* singleton;
  static bool instanceFlag;

  ofstream* mout;

  AsioDebugger(); // private constructor following Singleton pattern
  void getFilename (char* buffer, int max_size);

public:

  ~AsioDebugger();
  static AsioDebugger* getAsioDebugger ();

  void flush ();
  void printTime ();

  // Overloaded operator<< for debugging
  AsioDebugger& operator<< (char c);
  AsioDebugger& operator<< (signed char c);
  AsioDebugger& operator<< (unsigned char c);
  AsioDebugger& operator<< (const char* s);
  AsioDebugger& operator<< (const signed char* s);
  AsioDebugger& operator<< (const unsigned char* s);

  AsioDebugger& operator<< (bool val);
  AsioDebugger& operator<< (short val);
  AsioDebugger& operator<< (unsigned short val);
  AsioDebugger& operator<< (int val);
  AsioDebugger& operator<< (unsigned int val);
  AsioDebugger& operator<< (long val);
  AsioDebugger& operator<< (unsigned long val);
  AsioDebugger& operator<< (float val);
  AsioDebugger& operator<< (double val);

};

#endif // __ASIO_DEBUGGER__