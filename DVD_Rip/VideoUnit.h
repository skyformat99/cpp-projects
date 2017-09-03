#ifndef __VIDEOUNIT__
#define __VIDEOUNIT__

#include <string>

using namespace std;

class VideoUnit {
  
private:
  int vtsNumber;
  int numChapters;
  int pgcNumber;
  string pgcDurationString;
  double pgcDurationMin;

public:
  VideoUnit (int,int,int,string,double);

  int getVTS();
  int getPGC();
  double getDurMin();
  string toString();

};

#endif