#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "ExperimentCollection.h"
#include "Experiment.h"
#include "Tank.h"
#include "Block.h"
#include "Module.h"

#include "MySQLpp.h"
#include "SingleUnitDir.h"
#include "TTank.h"
#include "UploadUtil.h"

/** Macro to clear the screen using a system function **/
#define CLS system("cls");

#define LOGFILE_ROOT "c:/usr/cpp/Single_Unit/logs/"

/** Asks the user to verify an entry at the console */
bool consoleVerify (string);

/** Generate log file name and open log file */
void initializeLogFile ();

/** Gets the experiment collection and opens it */
void getExperimentCollection (int,char**,ExperimentCollection*);

/** Gets the single unit directory and creates object */
void getSingleUnitDir (int,char**,SingleUnitDir*,ExperimentCollection*);

/** Gets the database and sets up MySQL */
string getDatabaseSetup (MySQLpp*);

/** Checks to see if the database can be truncated */
void checkTruncate (string*, MySQLpp*, fstream*);

/** Truncates the tables in the given database */
void truncateDatabase (string*, MySQLpp*, fstream*);

/** Global log output stream for access in subfunctions **/
fstream* logstream;

/**
 * Main entry into function. 
 */
int main (int argc, char *argv[]) {

  // First thing we do is initialize the log file
  // Setup output logging file
  logstream = new fstream();
  try {
    initializeLogFile();
  } catch (char* c) {
    cerr << "Error opening log stream: " << c << endl;
    exit(1);
  }

  // Set up the ExperimentCollection, the SingleUnit Dir, MySQL & TTank
  ExperimentCollection* ec = new ExperimentCollection();
  getExperimentCollection(argc,argv,ec);

  SingleUnitDir* su = new SingleUnitDir();
  getSingleUnitDir(argc,argv,su,ec);

  MySQLpp* mysql = new MySQLpp();
  string dbStr = getDatabaseSetup(mysql);

  checkTruncate(&dbStr,mysql,logstream);

  TTank* ttank;
  try {
    ttank = new TTank("Local");
  } catch (char* c) {
    ERROUT("Error opening TTank server: " << c << endl);
  }  

  CLS

  // Now call ec->upload to finish the process
  try {
    ec->upload(su,mysql,ttank,logstream,&dbStr);
  } catch (char* c) {
    ERROUT("Error uploading experimentCollection: " << c << endl);
  } catch (...) {
    ERROUT("Error uploading experimentCollection: Error Unknown" << endl);
  }

  if (mysql) delete mysql;
  if (su)    delete su;
  if (ec)    delete ec;

  exit(0);
} 

/**
 * Asks the user the specified message, and then waits for
 * input. If the input matches either 'y', 'Y', 'yes', 'Yes', 
 * or 'YES', then true is returned; otherwise false is returned.
 */
bool consoleVerify (string msg) 
{
  cout << msg;

  string s;
  getline(cin,s);

  if (s.compare("y") == 0 ||
      s.compare("Y") == 0 ||
      s.compare("yes") == 0 ||
      s.compare("Yes") == 0 ||
      s.compare("YES") == 0 ||
      s.compare("") == 0) 
    return true;
  else
    return false;
}

/**
 * Computes the name of the log file based on the date and time
 * of the script running. Opens the log file on the passed file stream.
 */
void initializeLogFile ()
{
  SYSTEMTIME st;
  GetLocalTime(&st);

  stringstream logfilename;
  logfilename << LOGFILE_ROOT << "log_" << st.wYear << "-";

  if (st.wMonth < 10)
    logfilename << "0" << st.wMonth << "-";
  else
    logfilename << st.wMonth << "-";

  if (st.wDay < 10)
    logfilename << "0" << st.wDay << "_";
  else
    logfilename << st.wDay << "_";

  stringstream minStr;
  if (st.wMinute < 10)
    minStr << "0" << st.wMinute;
  else
    minStr << st.wMinute;

  int hr = st.wHour;
  if (hr < 1)
    logfilename << "12-" << minStr.str() << "-PM";
  else if (hr >= 1 && hr < 10)
    logfilename << "0" << hr << "-" << minStr.str() << "-AM";
  else if (hr >= 10 && hr < 12)
    logfilename << hr << "-" << minStr.str() << "-AM";
  else if (hr == 12)
    logfilename << hr << "-" << minStr.str() << "-PM";
  else if (hr > 12 && hr < 22)
    logfilename << "0" << (hr-12) << "-" << minStr.str() << "-PM";
  else 
    logfilename << (hr-12) << "-" << minStr.str() << "-PM";

  logfilename << ".txt";

  cout << "logFileName: " << logfilename.str() << endl;

  try {
    logstream->open(logfilename.str().c_str(), fstream::out | fstream::app);
  } catch (...) {
    throw "Error with fstream->open";
  }
}

/**
 * Loads the Experiment Collection from either the first argument, or
 * from user input. Verifies with the user that this is correct,
 * and writes to the log.
 */
void getExperimentCollection (int argc, char* argv[], ExperimentCollection* ec)
{
  CLS

  // If we have one arg, it's the xml sheet
  string xmlDoc;
  if (argc > 1) {
    xmlDoc = argv[1];
  } else {
    cout << "Enter full path to .xml sheet (Q to Quit):" << endl << "> ";
    getline(cin,xmlDoc);

    if (xmlDoc.compare("Q") == 0) { ERROUT("Quitting on user request"); }
  }

  // Loop until we get the correct XML sheet loaded
  while (true) {

    try {
      ec->loadFromXML((char*)xmlDoc.c_str());
    } catch (char* c) {
      ERROUT("Error loading XML document: " << c);
    } catch (...) {
      ERROUT("Error loading XML document");
    }

    ec->printSummary(&cout);
    cout << endl;

    if (consoleVerify("Is this correct? (y/n) ")) break;

    CLS

    cout << "Enter full path to .xml sheet (Q to Quit):" << endl << "> ";
    getline(cin,xmlDoc);

    if (xmlDoc.compare("Q") == 0) { ERROUT("Quitting on user request"); }

    cout << endl;
  }

  (*logstream) << "XML File: " << xmlDoc << endl;
  ec->printSummary(logstream);
}

/**
 * Gets the Single Unit Directory from the user, populates the
 * object, and logs this.
 */
void getSingleUnitDir (int argc, char* argv[], 
    SingleUnitDir* su, ExperimentCollection* ec)
{
  CLS

  // Now we get the Single Unit Directory
  string suDir;
  if (argc > 2) {
    suDir = argv[2];
  } else {
    cout << "Enter full path of SU Directory [Use '/' slashes only!] " 
         << "(Q to Quit):" << endl << "> ";
    getline(cin,suDir);

    if (suDir.compare("Q") == 0) { ERROUT("Quitting on user request"); }
  }

  while (true) {
    su->setDirectory(suDir);

    cout << "SU Dir: " << suDir << endl;

    if (ec->containsAllTanks(su)) {
      cout << "Single Unit Directory contains all tanks!" << endl << endl;
    } else {
      cout << "ERROR: Single Unit Directory does not contain all tanks!" 
           << endl << endl;
    }

    if (consoleVerify("Is this the correct SU? (y/n) ")) break;

    CLS

    cout << "Enter full path of SU Directory (Q to Quit):" << endl << "> ";
    getline(cin,suDir);

    if (suDir.compare("Q") == 0) { ERROUT("Quitting on user request"); }
    cout << endl;
  }

  (*logstream) << "SU Directory: " << suDir << endl;
}

/**
 * Sets up the Database by creating the MySQL connection,
 * querying the user for the appropriate database, and setting
 * this db in mysql. Returns the database being used.
 */
string getDatabaseSetup (MySQLpp* mysql) 
{
  // Setup MySQL connection object & choose database
  string dbHost = "uber-server.urmc-sh.rochester.edu";
  string dbUser = "joe";
  string dbPass = "joepass";
  string dbStr = "db_util";

  try {
    mysql->connectToServer(dbHost,dbUser,dbPass,dbStr,0);
  } catch (char* c) {
    ERROUT("Error connecting to MySQL server: " << c << endl);
  }
  mysql->initializeDebugging("db_util","cpp_upload_log","str");
  mysql->initializeLogging(logstream);
  
  vector<string> dbs;
  string query = "SHOW DATABASES";
  try {
    mysql->querySingleColumn(&query,&dbs);
  } catch (char* c) {
    ERROUT("Error querying for databases: " << c << endl);
  } catch (...) {
    ERROUT("Error querying for databases");
  }

  string dbEntry;
  int dbIndex;
  while (true) {
    CLS
    cout << "Enter number of database (Q to Quit):" << endl;

    int i;
    for (i=0;i<dbs.size();i++) 
      cout << "(" << i << ")\t" << dbs[i] << endl;

    cout << "> ";

    getline(cin,dbEntry);
    if (dbEntry.compare("Q") == 0) { ERROUT("Quitting on user request"); }

    try {
      dbIndex = atoi(dbEntry.c_str());
    } catch (...) {
      ERROUT("Error converting db entry choice to integer");
    }

    cout << endl << "You chose " << dbs[dbIndex] << " ... ";
    if (consoleVerify("was this correct? (y/n) ")) break;
  }

  dbStr = dbs[dbIndex];
  (*logstream) << "Using db " << dbStr << endl;

  stringstream errormsg;
  errormsg << "Error switching database to current";
  query = "USE " + dbStr;
  try {
    mysql->execQuery(&query,&errormsg);
  } catch (char* c) {
    ERROUT("Error switching dB: " << c << endl);
  }

  return dbStr;
}

/**
 * Determines if the database is a test database, and 
 * if it is, asks the user if they'd like to truncate first.
 */
void checkTruncate (string* dbStr, MySQLpp* mysql, 
    fstream* logstream) {

  // First check the dbStr to see if it ends in "_test",
  // the test database default
  int dbLen = dbStr->length();
  
  if (dbLen <= 5)
    return;

  if (dbStr->substr(dbLen-5).compare("_test") != 0) 
    return;

  CLS

  // Now ask the user if they would like to truncate
  // the test database
  cout << "Database " << *dbStr << " appears to be a test database." << endl;
  cout << "Would you like to truncate (remove all data) prior to uploading?";
  if (consoleVerify(" (y/n) ")) {
    truncateDatabase(dbStr,mysql,logstream);
  }

  return;
}

/**
 * Given the name of the databse, this function will make
 * successive calls to the execQuery truncating each of the
 * tables in the database.
 */
void truncateDatabase (string* dbStr, MySQLpp* mysql, 
    fstream* logstream)
{
  stringstream ss, errorMsg;

  errorMsg << "Error truncating the database!";

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".block";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".block_properties";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".block_unit_properties";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".condition_list";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".experiment";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".experiment_properties";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".sweep_list";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".tank";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".unit";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".unit_data";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  ss.str("");
  ss << "TRUNCATE " << *dbStr << ".unit_properties";
  try {
    mysql->execQuery(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error on execQuery: " << c << endl);
  }

  (*logstream) << "Truncated database " << *dbStr << endl;
}

