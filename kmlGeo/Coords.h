#pragma once

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Coords
{

private:

  int id;
  int numPairs;

  double *x, *y;

  vector<string> tok (string&);

  void getCoords (string&, double*, double*);
  void getTriplet (string&, int*, double*, double*);

public:

  Coords (vector<string>*);
  Coords (Coords* c);
  ~Coords ();

  int getID ();

  void writeCoordinates(ostream*);

};