#include <sstream>
#include <iostream>
#include "Coords.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

Coords::Coords (vector<string>* vec)
{
  if (vec->size() < 2)
    throw "Error: vector size is < 2 elements!";

  int id_;
  double x_, y_;
  vector<double> coordX;
  vector<double> coordY;

  try {
    getTriplet((*vec)[0],&id_,&x_,&y_);
  } catch (...) {
    this->id = -1;

    return;
  }

  this->id = id_;
  coordX.push_back(x_);
  coordY.push_back(y_);

  vector<string>::iterator it = vec->begin();
  it++;

  for ( ; it < vec->end(); it++ ) {

    getCoords(*it,&x_,&y_);

    coordX.push_back(x_);
    coordY.push_back(y_);
  }

  if (coordX.size() != coordY.size())
    throw "Error: X and Y coordinate pairs must be equal in number";

  this->numPairs = coordX.size();

  this->x = new double[numPairs];
  this->y = new double[numPairs];

  for (int i=0; i<numPairs; i++) {
    x[i] = coordX[i];
    y[i] = coordY[i];
  }
}

Coords::Coords (Coords* c)
{
  this->id = c->id;
  this->numPairs = c->numPairs;

  this->x = new double[numPairs];
  this->y = new double[numPairs];

  for (int i=0; i<numPairs; i++) {
    x[i] = c->x[i];
    y[i] = c->y[i];
  }
}

Coords::~Coords ()
{
  if (this->x) delete [] this->x;
  if (this->y) delete [] this->y;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

int Coords::getID ()
{
  return this->id;
}

void Coords::writeCoordinates (ostream* fout)
{
  for (int i=0; i < numPairs; i++) {
    fout->setf(ios::scientific);
    fout->precision(15);
    *fout << "            " << x[i] << ", " << y[i] << endl;
  }
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

void Coords::getCoords (string& src, double* x, double* y)
{
  stringstream ss (src);

  try {
    ss >> *x;
    ss >> *y;
  } catch (...) {
    throw "Error reading coords pair!";
  }
}

void Coords::getTriplet (string& src, int* id, double* x, double* y)
{
  stringstream ss (src);

  try {
    ss >> *id;
    ss >> *x;
    ss >> *y;
  } catch (...) {
    throw "Error reading triplet!";
  }
}

vector<string> Coords::tok (string& src)
{
  vector<string> v;

  stringstream ss (src);
  string s;

  while (getline(ss,s,' '))
    v.push_back(s);

  return v;
}