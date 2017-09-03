#pragma once

#include <sstream>
#include <iostream>
#include <string>
#include "Coords.h"
#include "Style.h"

using namespace std;

class GeoElem
{

private:

  int id;

  string stateID;
  string countyID;
  string tractID;
  string tractName;

  string metaData;
  bool isSet;

  Coords* coords;

  Style* style;

  string fillTractID (string&);
  string trimQuotes (string*);

public:

  GeoElem ();
  GeoElem (int,string*,string*,string*,string*);
  GeoElem (GeoElem&);
  ~GeoElem ();

  void addCoords (Coords*);
  void displayElem ();

  bool equals (string&);

  void setMetaData (string&);
  void setStyle (Style*);

  void writePlacemark (ostream*);

};
  