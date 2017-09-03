#include <iostream>
#include <cfloat>

#include "Grid.h"
#include "Fingerprint.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Grid::Grid (size_t n, string* m) 
{
  this->numMacs = n;
  this->macs = new string[n];
  for (int i=0; i<n; i++)
    this->macs[i] = m[i];

  points = new vector<GridPoint*>();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void Grid::add (string mac, double x, double y, double rssi)
{
  GridPoint* gp = new GridPoint(x, y, this->numMacs, this->macs);

  for (vector<GridPoint*>::iterator it = this->points->begin(); it != this->points->end(); ++it) {
    if ((*it)->equals(gp)) {
      (*it)->update(mac, rssi);
      return;
    }
  }

  gp->update(mac, rssi);
  this->points->push_back(gp);
}

bool Grid::calcDistance (Fingerprint* f, double& x, double& y)
{
  return calcWeightedDistance (f,x,y,0.,1.);
}

bool Grid::calcWeightedDistance (Fingerprint* f, double& x, double& y, 
    double slope, double intercept)
{
  return calcWeightedTimedDistance(f,x,y,slope,intercept,0.,0.,1.,1.,false);
}

bool Grid::calcWeightedTimedDistance(Fingerprint* f, double& x, double& y,
    double weightM, double weightB, double speedM2, double speedM1, double speedB, 
    double duration, bool useTimes)
{
  if (!f) return false;

  double minDistance = DBL_MAX;
  GridPoint* minGP = NULL;
  double d;

  for (vector<GridPoint*>::iterator it = this->points->begin(); it != this->points->end(); ++it) {
    d = (*it)->calcWeightedTimedDistance(f,x,y,weightM,weightB,speedM2,speedM1,speedB,duration,useTimes);

    if (d < minDistance) {
      minGP = (*it);
      minDistance = d;
    }
  }

  if (minGP != NULL) {
    x = minGP->getX();
    y = minGP->getY();
  }

  return (minGP != NULL);
}

void Grid::print ()
{
  cout << "Grid (# GPs: " << this->points->size() << ")" << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *