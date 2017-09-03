#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <math.h>

#define CSV_BUF_LEN 4096

using namespace std;

class FileCSV
{

private:

  int numCols;
  string* columnLabels;
  vector<string>* geoID;
  vector<double>** data;

  int countColumns (string*);
  void extractLine (string*);
  void gatherColumnLabels (string*, string*);
  void gatherColumnLabels (string*);
  void importFile (string*);
  bool isHeader (string*);

public:
  FileCSV (string*);
  ~FileCSV();

  void columnStats (int, double*, double*, double*, double*, int*);
  void displayHeaders (bool);
  void displayInfo();

  string at (int);
  double at (int, int);

  int size ();

  string getStateCode ();
};