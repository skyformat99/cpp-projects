#ifndef __experiment__
#define __experiment__

#include <string>
#include <iostream>
#include <fstream>

#include "Tank.h"

#include "XMLNode.h"
#include "XMLNodeList.h"

#include "SingleUnitDir.h"
#include "MySQLpp.h"
#include "TTank.h"
#include "Timer.h"
#include "UploadUtil.h"

class Experiment {

private:
  bool isLoaded;

  string* date;
  string* strain;
  string* animal;
  string* dob;
  string* gender;
  string* electrode;
  string* target;
  string* type;

  Tank** tanks;
  int numTanks;

  string reformatDate (string*);

public:
  Experiment ();
  ~Experiment ();

  int size ();
  Tank* operator[] (int);

  void loadFromXML (XMLNode*);
  void printSummary (ostream*);

  void upload (SingleUnitDir*,MySQLpp*,TTank*,fstream*,string*);
};

#endif