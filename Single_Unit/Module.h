#ifndef __module_h__
#define __module_h__

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "XMLNode.h"
#include "XMLNodeList.h"

#include "MySQLpp.h"
#include "UploadUtil.h"

class Module {

private:
  bool isLoaded;

  string* name;
  int id;

  string** propNames;
  string** propVals;
  int numProperties;

public:
  Module ();
  ~Module ();

  int gatherID (MySQLpp*);
  void getEventCodes (MySQLpp*,vector<string>*,vector<int>*);

  void setName (string);
  void loadProperties (XMLNode*);

  void upload (MySQLpp*,int,string*,fstream*);
};

#endif