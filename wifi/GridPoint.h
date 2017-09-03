#ifndef __GridPoint_h__
#define __GridPoint_h__

#include "Fingerprint.h"

using namespace std;

class GridPoint
{

public:
  GridPoint (double, double);
  GridPoint (double, double, size_t, string*);

  double calcWeightedTimedDistance(Fingerprint*, double, double, 
    double, double, double, double, double, double, bool);

  bool equals (GridPoint*);

  double getX ();
  double getY ();

  void update (string, double);

private:

  double x;
  double y;

  Fingerprint* f;
};

#endif