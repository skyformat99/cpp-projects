#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

#include <sys/stat.h> 

#include "GeoElemCollection.h"
#include "GeoElem.h"
#include "Coords.h"
#include "StyleGroup.h"
#include "Style.h"

/** Char length for reading a line of text **/
#define BUF_LEN 128

/** Diretory where the data files are stored **/
#define DATA_FILE_DIR "Data Files/"

using namespace std;

class State : public GeoElemCollection
{

private:

  StyleGroup* sg;

  GeoElem** elems;
  int numElems;

  void State::initFromFile (string*);
  void readAttribFile (string, vector<GeoElem*>*);
  void readCoordFile (string);


public:
  State (string*);
  ~State ();

  void displayInfo ();
  void displayElem (int);
  int findGeoElem (int*, string&);

  void setGeoStyle ( int, Style* );
  void setMetaData (int, double);
  void setStyleGroup (StyleGroup*);
  void writeTestFile(char*);


  static bool does_exist (string*);
};