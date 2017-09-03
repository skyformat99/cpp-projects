#include <iostream>
#include <math.h>

#include "GridPoint.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

GridPoint::GridPoint (double x, double y)
{
  this->x = x;
  this->y = y;

  this->f = NULL;
}

GridPoint::GridPoint (double x, double y, size_t n, string* m) 
{
  this->x = x;
  this->y = y;

  this->f = new Fingerprint(n,m);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

double GridPoint::calcWeightedTimedDistance(Fingerprint* f, double oldX, double oldY, 
    double weightM, double weightB,
    double speedM2, double speedM1, double speedB, double duration, bool useTimes)
{
  double euclidDist = sqrt( (oldX - x) * (oldX - x) + (oldY - y) * (oldY - y) );
  double velocity = euclidDist / duration;

  double rawCalc = this->f->calcWeightedTimedDistance(f,weightM,weightB);
  double speedWt = speedM2 * exp( speedM1 * velocity ) + speedB;
//  double speedWt = speedM2 * (velocity * velocity) + speedM1 * velocity + speedB;

  cout << "old:," << oldX << "," << oldY << ",new:," << x << "," << y 
       << ",dist:," << euclidDist << ",dur:," << duration << ",raw:," 
       << rawCalc << ",speedWt:," << speedWt << endl;

  if (rawCalc == DBL_MAX || !useTimes)
    return rawCalc;
  else
    return rawCalc * speedWt;
}

bool GridPoint::equals (GridPoint* gp)
{
  return (this->x == gp->x && 
          this->y == gp->y);
}

double GridPoint::getX ()
{
  return this->x;
}

double GridPoint::getY ()
{
  return this->y;
}

void GridPoint::update (string mac, double rssi)
{
  this->f->update(mac, rssi);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *