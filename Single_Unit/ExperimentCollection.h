#ifndef __experiment_collection__
#define __experiment_collection__

#include <string>
#include <iostream>

#include "Experiment.h"

#include "XMLDocument.h"
#include "XMLNode.h"
#include "XMLNodeList.h"

#include "SingleUnitDir.h"
#include "MySQLpp.h"
#include "TTank.h"
#include "UploadUtil.h"

class ExperimentCollection {

private:
  bool isLoaded;

  Experiment** expts;
  int numExperiments;

public:
  ExperimentCollection ();
  ~ExperimentCollection ();

  int size ();
  Experiment* operator[] (int);

  bool containsAllTanks (SingleUnitDir*);

  void loadFromXML (char*);
  void printSummary (ostream*);

  void upload (SingleUnitDir*,MySQLpp*,TTank*,fstream*,string*);
};

#endif