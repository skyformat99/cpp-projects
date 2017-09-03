#ifndef __timer_h
#define __timer_h

#include <stdio.h>
#include <sys/timeb.h>

#include <string>
#include <iostream>

#include "UploadUtil.h"

using namespace std;

class Timer {

private:

  struct __timeb64 sTime, eTime;

public:

  Timer ();
  ~Timer ();

  double elapsed ();
  void print ();
  void print (const char*);
  void print (string*);
  void print (FILE*);
  void print (ostream*);
  void print (FILE*,const char*);
  void print (ostream*,string*);
  void reset ();

};

#endif