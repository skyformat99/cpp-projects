#include <iostream>
#include <math.h>
#include "Fingerprint.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Fingerprint::Fingerprint (size_t n, string* m) 
{
  int i;

  this->macs = new string[n];
  for (i=0; i<n; i++)
    this->macs[i] = m[i];

  this->rssi = new double[n];
  for (i=0; i<n; i++)
    this->rssi[i] = DBL_MAX;

  this->numElements = n;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * Calculates a weighted distance, with the parameter 'f'
 * being the sampled Fingerprint and (*this) being the data point
 * read from the file.
 *
 * Velocity is measured in % hallway / second; a good start
 * for the weight is 
 */
double Fingerprint::calcWeightedTimedDistance (Fingerprint* f, 
    double slope, double intercept)
{
  double weight, ssq = 0.;
  int i, n = 0;

  for (i=0; i<this->numElements; i++) {
    if (this->rssi[i] == DBL_MAX ||
        f->rssi[i] == DBL_MAX) continue;

    weight = f->rssi[i] * slope + intercept;
    ssq += (this->rssi[i] - f->rssi[i]) * (this->rssi[i] - f->rssi[i]) * weight;
    n++;
  }

  if (n == 0) return DBL_MAX;

  return sqrt(ssq);
}

void Fingerprint::update (string mac, double r)
{
  for (int i = 0; i < this->numElements; i++) {
    if (mac.compare(this->macs[i]) == 0) {
      this->rssi[i] = r;

      return;
    }
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *