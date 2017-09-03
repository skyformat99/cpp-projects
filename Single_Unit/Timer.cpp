#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string>

#include "Timer.h"

using namespace std;

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

Timer::Timer () {

  reset();

}

Timer::~Timer() {}

// * * * * * * * * * * * * * * * *
// * Public Methods
// * * * * * * * * * * * * * * * *

double Timer::elapsed () {

  time_t time1, time2;
  unsigned short millitm1, millitm2;
  double diff;

  if (_ftime64_s(&eTime) != 0)
    fprintf_s(stderr,"Error setting end time!\n");

  time1 = eTime.time;
  time2 = sTime.time;
  millitm1 = eTime.millitm;
  millitm2 = sTime.millitm;

  diff = (((double)time1) + ((double)millitm1) / 1000.) - 
         (((double)time2) + ((double)millitm2) / 1000.);

  return diff;
}

void Timer::print () {

  print(stdout);

}

void Timer::print (const char* msg) {

  print(stdout,msg);

}

void Timer::print (string* msg) {

  print(&cout,msg);

}

void Timer::print (FILE *stream) {

  fprintf_s(stream,"Elapsed time: %0.4f sec\n",elapsed());

}

void Timer::print (ostream* strm) {

  (*strm) << "Elapsed time: " << elapsed() << endl;

}

void Timer::print (FILE *stream, const char* msg) {

  fprintf_s(stream,"%s (%0.4f sec)\n",msg,elapsed());

}

void Timer::print (ostream* strm, string* msg) {

  (*strm) << *msg << " (" << elapsed() << " sec)" << endl;

}


void Timer::reset () {

 if (_ftime64_s(&sTime) != 0)
   fprintf_s(stderr,"Error resetting timer!\n");

}