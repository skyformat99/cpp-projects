#include "ExperimentCollection.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

ExperimentCollection :: ExperimentCollection () 
{
  isLoaded = false;

  expts = NULL;
  numExperiments = 0;
}

ExperimentCollection :: ~ExperimentCollection ()
{
  int i;
  for (i=0;i<numExperiments;i++)
    delete expts[i];

  if (expts) delete [] expts;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

bool ExperimentCollection :: containsAllTanks (SingleUnitDir* su)
{
  int i,j;
  int numTanks;

  Tank* tank;

  for (i=0;i<numExperiments;i++) {
    numTanks = expts[i]->size();

    for (j=0;j<numTanks;j++) {
      tank = (*expts[i])[j];

      if (!su->contains(tank->getName())) return false;
    }
  }

  return true;
}

void ExperimentCollection :: loadFromXML (char* c)
{
  XMLDocument* doc = new XMLDocument(c);

  XMLNode* condSheet = new XMLNode();
  doc->getRootNode("condition_sheet",condSheet);

  XMLNodeList* exptList = new XMLNodeList();
  condSheet->getNodes("experiment",exptList);

  numExperiments = exptList->length();

  expts = new Experiment*[numExperiments];

  int i;
  XMLNode* expt = NULL;
  for (i=0;i<numExperiments;i++) {
    expts[i] = new Experiment();

    expt = new XMLNode();
    exptList->getNode(i,expt);

    expts[i]->loadFromXML(expt);

    delete expt;
  }

  delete exptList;
  delete condSheet;
  delete doc;

  isLoaded = true;
}

void ExperimentCollection :: printSummary (ostream* o)
{
  if (!isLoaded) return;

  (*o) << "ExperimentCollection :: " << numExperiments 
       << " Experiments ::" << endl;

  int i;
  for (i=0;i<numExperiments;i++) {
    (*o) << "  (" << (i+1) << ") ";
    expts[i]->printSummary(o);
  }
}

int ExperimentCollection :: size () 
{
  return numExperiments;
}

Experiment* ExperimentCollection :: operator[] (int i)
{
  if (i >= 0 && i < numExperiments)
    return expts[i];
  else
    throw "Error: index out of bounds!";
}

void ExperimentCollection :: upload (SingleUnitDir* su,
    MySQLpp* mysql, TTank* ttank, fstream* logstream, string* dbStr) 
{
  int i;

#ifdef DEBUG
  cout << "Uploading the ExperimentCollection to " << *dbStr << " ..." << endl;
#endif

  for (i=0;i<numExperiments;i++) {
    expts[i]->upload(su,mysql,ttank,logstream,dbStr);
  }
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *