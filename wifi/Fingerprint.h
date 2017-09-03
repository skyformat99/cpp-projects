#ifndef __Fingerprint_h__
#define __Fingerprint_h__

#include <string>

using namespace std;

class Fingerprint
{

public:
  Fingerprint (size_t, string*);

  double calcWeightedTimedDistance (Fingerprint*, double, double);

  void update (string, double);

private:

  size_t numElements;

  string* macs;
  double* rssi;

};

#endif