#pragma once

#include <iostream>
#include <string>
#include "math.h"

#include "Style.h"
#include "GeoElemCollection.h"

using namespace std;

class StyleGroup
{

private:
  int nbins;
  Style** styles;

  double min;
  double max;
  double step;

  void initGroup (string, string, int);

public:
  StyleGroup (string, string, int, double, double);
  ~StyleGroup ();

  Style* operator[] (int);

  void setElementStyle (double, GeoElemCollection*, int);
  void writeStyleDeclaration (ostream*);

};