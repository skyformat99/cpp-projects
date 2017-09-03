#include "Experiment.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

Experiment :: Experiment ()
{
  isLoaded = false;

  date = NULL;
  strain = NULL;
  animal = NULL;
  dob = NULL;
  gender = NULL;
  target = NULL; 
  electrode = NULL;
  type = NULL;

  tanks = NULL;
  numTanks = 0;
}

Experiment :: ~Experiment ()
{
  int i;
  for (i=0;i<numTanks;i++)
    delete tanks[i];

  if (tanks) delete [] tanks;

  if (date)      delete date;
  if (strain)    delete strain;
  if (animal)    delete animal;
  if (dob)       delete dob;
  if (gender)    delete gender;
  if (target)    delete target;
  if (electrode) delete electrode;
  if (type)      delete type;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

void Experiment :: loadFromXML (XMLNode* expt) 
{
  date = new string(expt->getAttribute("date"));
  strain = new string(expt->getAttribute("strain"));
  animal = new string(expt->getAttribute("animal"));
  dob = new string(expt->getAttribute("dob"));
  gender = new string(expt->getAttribute("gender"));
  target = new string(expt->getAttribute("target"));
  electrode = new string(expt->getAttribute("electrode"));
  type = new string(expt->getAttribute("type"));

  XMLNodeList* tankList = new XMLNodeList();
  expt->getNodes("tank",tankList);

  numTanks = tankList->length();

  tanks = new Tank*[numTanks];

  int i;
  XMLNode *tank;
  for (i=0;i<numTanks;i++) {
    tanks[i] = new Tank();

    tank = new XMLNode();
    tankList->getNode(i,tank);

    tanks[i]->loadFromXML(tank);

    delete tank;
  }

  delete tankList;

  isLoaded = true;
}

void Experiment :: printSummary (ostream *o)
{
  if (!isLoaded) return;

  (*o) << "Experiment " << (*date) << " - " << numTanks << " tanks, "
       << "Animal [" << (*animal) << "] DOB: " << (*dob) << endl;
}

int Experiment :: size ()
{
  return numTanks;
}

Tank* Experiment :: operator[] (int i)
{
  if (i >= 0 && i < numTanks) 
    return tanks[i];
  else
    throw "Error: index out of bounds!";
}

void Experiment :: upload (SingleUnitDir* su,
    MySQLpp* mysql, TTank* ttank, fstream* logstream, string* dbStr) {

  Timer* timer = new Timer();

  string newDOB;
  try {
    newDOB = reformatDate(dob);
  } catch (char* c) {
    ERROUT("DOB reformat error: expt " << *date << ": " << c << endl);
  }

  // Need to insert the experiment first
  stringstream ss, errorMsg;
  ss << "INSERT INTO " << *dbStr << ".experiment (strain, id_num, " 
     << "gender, DOB, target, electrode) VALUES ('" 
     << *strain << "', " << *animal << ", '" << *gender 
     << "', '" << newDOB << "', '" << *target  << "', '"
     << *electrode << "')";

  errorMsg << "Error inserting experiment information";
  int exptID;

#ifdef DEBUG
  cout << "Experiment info query: " << ss.str() << endl;
#endif

  try {
    exptID = mysql->insertSingleItem(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error inserting experiment info: " << c << endl);
  }

  cout << "Uploading experiment " << (*date) << " - " 
       << numTanks << " tanks, " << "Animal [" << (*animal) 
       << "] DOB: " << (*dob) << endl;;

  // Now iterate through tanks and upload
  int i;
  for (i=0;i<numTanks;i++) {
    tanks[i]->upload(su,mysql,ttank,logstream,dbStr,exptID);
  }

  cout << "  Finished Experiment - Elapsed Time: " 
       << timer->elapsed() << " sec" << endl;
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

string Experiment :: reformatDate (string* date) throw (char*)
{
  // Need to dismantle the date here
  size_t firstSlash = date->find_first_of("/");

  if (firstSlash == -1)
    throw "Date format error!";

  int month;
  try {
    month = atoi(date->substr(0,firstSlash).c_str());
  } catch (...) {
    throw "Date format error: month invalid";
  }

  size_t secondSlash = date->find_first_of("/",firstSlash+1);

  if (secondSlash == -1)
    throw "Date format error!";

  int day;
  try {
    day = atoi(date->substr(firstSlash+1,(secondSlash-firstSlash)-1).c_str());
  } catch (...) {
    throw "Date format error: day invalid";
  }

  int yr;
  try {
    if (date->length() - (secondSlash+1) == 4)
      yr = atoi(date->substr(secondSlash+1).c_str());
    else
      yr = atoi(date->substr(secondSlash+1).c_str())+2000;
  } catch (...) {
    throw "Date format error: year invalid";
  }

  stringstream ss;
  ss << yr << "-";

  if (month < 10)
    ss << "0" << month << "-";
  else
    ss << month << "-";

  if (day < 10)
    ss << "0" << day;
  else
    ss << day;

  return ss.str();
}