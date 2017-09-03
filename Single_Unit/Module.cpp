#include "Module.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

Module :: Module ()
{
  isLoaded = false;

  name = NULL;
  id = -1;

  propNames = NULL;
  propVals = NULL;
  numProperties = 0;
}

Module :: ~Module ()
{
  int i;
  for (i=0;i<numProperties;i++) {
    delete propNames[i];
    delete propVals[i];
  }

  if (propNames) delete [] propNames;
  if (propVals)  delete [] propVals;

  if (name) delete name;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

int Module :: gatherID (MySQLpp* mysql)
{
  stringstream ss;
  ss << "SELECT m.id FROM db_util.modules m WHERE m.name = '" 
     << *name << "'";

  vector<string> vec;

  mysql->querySingleColumn(&(ss.str()),&vec);

  if (vec.size() != 1)
    throw "Error: module id query returned invalid count!";

  string moduleIDStr = vec[0];

  try {
    id = atoi(moduleIDStr.c_str());
  } catch (...) {
    throw "Error: module ID format exception!";
  }

  return id;
}

void Module :: getEventCodes (MySQLpp* mysql, vector<string>* ecCodes,
    vector<int>* ecIDs)
{
  stringstream ss;
  ss << "SELECT CONCAT(CAST(c.id AS CHAR),'-',c.name) "
     << "FROM db_util.module_codes mc "
     << "INNER JOIN db_util.event_codes c ON (mc.code_id = c.id) "
     << "WHERE mc.module_id = " << id;

  vector<string> vec;

  mysql->querySingleColumn(&(ss.str()),&vec);

  int i;
  string concatStr;
  int dashInd;
  int ecID;
  string ecCode;

  for (i=0;i<vec.size();i++) {
    concatStr = vec[i];

    dashInd = concatStr.find_first_of("-");

    if (dashInd == -1)
      throw "Error: concat failed on event code query";

    try {
      ecID = atoi(concatStr.substr(0,dashInd).c_str());
    } catch (...) {
      throw "Error: could not pull event code ID off concat string";
    }

    ecCode = concatStr.substr(dashInd+1);

    ecIDs->push_back(ecID);
    ecCodes->push_back(ecCode);
  }

  return;
}


void Module :: loadProperties (XMLNode* block)
{
  XMLNodeList* propList = new XMLNodeList();
  block->getNodes("block-property",propList);

  numProperties = propList->length();

  propNames = new string*[numProperties];
  propVals = new string*[numProperties];

  int i;
  XMLNode *prop;
  for (i=0;i<numProperties;i++) {
      
    prop = new XMLNode();
    propList->getNode(i,prop);

    propNames[i] = new string(prop->getAttribute("name"));
    propVals[i] = new string(prop->getAttribute("value"));

    delete prop;
  }

  delete propList;

  isLoaded = true;
}

void Module :: setName (string s)
{
  name = new string(s);
}

void Module :: upload (MySQLpp* mysql, int blockID, string* dbStr, 
    fstream* logstream)
{
  stringstream ss, errorMsg;
  vector<string> vec;
  string propIDStr;
  int propID;

  int i;
  for (i=0;i<numProperties;i++) {
    ss.str("");
    ss << "SELECT p.id FROM db_util.block_parameters p WHERE " 
       << "p.name = '" << *(propNames[i]) << "'";

    vec.clear();
    try {
      mysql->querySingleColumn(&(ss.str()),&vec);
    } catch (char* c) {
      ERROUT("Error getting propID for prop " << *(propNames[i])
          << ": " << c << endl);
    }

    if (vec.size() != 1) {
      ERROUT("Error: prop " << *(propNames[i]) << " bad ID" << endl);
    }
    propIDStr = vec[0];

    try {
      propID = atoi(propIDStr.c_str());
    } catch (...) {
      ERROUT("Error: could not convert" << propIDStr << " to int" << endl);
    }

    ss.str("");
    ss << "INSERT INTO " << *dbStr << ".block_properties (block_id, " 
       << "parameter_id, value) VALUES (" << blockID << ", " 
       << propID << ", '" << *(propVals[i]) << "')";

    errorMsg.str("");
    errorMsg << "Failed on insert of blockID " << blockID 
             << " property " << *(propNames[i]) << " value "
             << *(propVals[i]);

    try {
      mysql->insertSingleItem(&(ss.str()),&errorMsg);
    } catch (char* c) {
      ERROUT("Error inserting blockProp: " << c << endl);
    }
  }
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *