#include <iostream>
#include <fstream>
#include <time.h>
#include "AsioDebugger.h"

using namespace std;

bool AsioDebugger::instanceFlag = false;
AsioDebugger* AsioDebugger::singleton = NULL;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// CONSTRUCTORS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

AsioDebugger::AsioDebugger ()
{
  char buffer[100];
  getFilename(buffer,100);

  mout = new ofstream(buffer, ios_base::app); 

  delete &buffer;
}

AsioDebugger::~AsioDebugger ()
{
  instanceFlag = false;
  if (mout) {
    mout->close();
    delete mout;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

AsioDebugger* AsioDebugger::getAsioDebugger ()
{
  if (!instanceFlag) 
  {
    singleton = new AsioDebugger();
    instanceFlag = true;
  }

  return singleton;
}

void AsioDebugger::flush ()
{
  mout->flush();
}

void AsioDebugger::printTime ()
{
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[100];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,100,"<%a %d-%b-%Y %H:%M:%S>\t",timeinfo);

  (*mout) << buffer;

  delete &buffer;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PUBLIC METHODS - OVERLOADED OPERATORS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

AsioDebugger& AsioDebugger::operator<< (char c)
{  (*mout) << c;  return *this; }

AsioDebugger& AsioDebugger::operator<< (signed char c)
{  (*mout) << c;  return *this; }

AsioDebugger& AsioDebugger::operator<< (unsigned char c)
{  (*mout) << c;  return *this; }

AsioDebugger& AsioDebugger::operator<< (const char* s)
{  (*mout) << s;  return *this; }

AsioDebugger& AsioDebugger::operator<< (const signed char* s)
{  (*mout) << s;  return *this; }

AsioDebugger& AsioDebugger::operator<< (const unsigned char* s)
{  (*mout) << s;  return *this; }

AsioDebugger& AsioDebugger::operator<< (bool val)
{  (*mout) << val;  return *this; }

AsioDebugger& AsioDebugger::operator<< (short val)
{  (*mout) << val;  return *this; }

AsioDebugger& AsioDebugger::operator<< (unsigned short val)
{  (*mout) << val;  return *this; }

AsioDebugger& AsioDebugger::operator<< (int val)
{  (*mout) << val;  return *this; }

AsioDebugger& AsioDebugger::operator<< (unsigned int val)
{  (*mout) << val;  return *this; }

AsioDebugger& AsioDebugger::operator<< (long val)
{  (*mout) << val;  return *this; }

AsioDebugger& AsioDebugger::operator<< (unsigned long val)
{  (*mout) << val;  return *this; }

AsioDebugger& AsioDebugger::operator<< (float val)
{  (*mout) << val;  return *this; }

AsioDebugger& AsioDebugger::operator<< (double val)
{  (*mout) << val;  return *this; }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void AsioDebugger::getFilename (char* buffer, int max_size)
{
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

//  strftime(buffer,max_size,"c:/asio_logger_%H-%M-%S.txt",timeinfo);
  strftime(buffer,max_size,"c:/asio_logger_%d-%b-%Y.txt",timeinfo);
}
