#ifndef __block__
#define __block__

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "Module.h"

#include "XMLNode.h"
#include "XMLNodeList.h"

#include "MySQLpp.h"
#include "TTank.h"
#include "UploadUtil.h"

/** The maximum number of events to read from the TTank */
#define MAX_EVENTS 600000

class Block {

private:
  bool isLoaded;

  string* name;
  int number;

  int blockID;

  vector<int>* ecIDs;
  vector<string>* ecCodes;

  Module* module;

public:
  Block ();
  ~Block ();

  int gatherEventCodes(MySQLpp*,fstream*);
  int getBlockNumber ();
  int getNumSweeps (TTank*);

  void loadFromXML (XMLNode*);

  void populateArrays (int,int*,double*,int,int*,int**, 
      TTank*,fstream*);

  int upload (MySQLpp*,int,string*,fstream*);
};

#endif