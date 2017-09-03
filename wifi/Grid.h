#ifndef __Grid_h__
#define __Grid_h__

#include "GridPoint.h"

#include <vector>

// Forward declaration
class Fingerprint;

using namespace std;

class Grid
{

public:
  Grid (size_t, string*);

  void add (string, double, double, double);

  bool calcDistance (Fingerprint*,double&,double&);
  bool calcWeightedDistance (Fingerprint*,double&,double&,double,double);
  bool calcWeightedTimedDistance(Fingerprint*,double&,double&,
           double,double,double,double,double,double,bool);

  void print ();

private:

  vector<GridPoint*>* points; 

  size_t numMacs;
  string* macs;

};

#endif