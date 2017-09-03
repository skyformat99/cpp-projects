#ifndef __mysql_pp__
#define __mysql_pp__

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

/** Required for MYSQL calls */
#include <windows.h>
#include "mysql.h"

#include "UploadUtil.h"

/**
 * Must be linked with: libmysql.lib
 */

/**
 * Interacts with the MySQL C routines in an encapsulated
 * C++ class.
 */
class MySQLpp {

private:

  MYSQL* conn;
  bool connected;

  bool debugging;
  string debug_db;
  string debug_table;
  string debug_column;

  bool logging;
  fstream* logstream;

  void writeAndThrowErrorMessage (stringstream*,bool);

public:

  MySQLpp ();
  MySQLpp (string,string,string,string,int);
  ~MySQLpp ();

  void connectToServer (string,string,string,string,int);

  void execQuery (string*,stringstream*);
  void execQuery (string*,stringstream*,bool);

  void initializeDebugging (string,string,string);
  void initializeLogging (fstream*);
  void initializeLogging (string);

  int insertSingleItem (string*,stringstream*);
  int insertSingleItem (string*,stringstream*,bool);
  int insertSingleItem (string*,stringstream*,bool,bool);

  void insertMultiItem (string*,stringstream*);
  void insertMultiItem (string*,stringstream*,bool);
  void insertMultiItem (string*,stringstream*,bool,bool);

  void querySingleColumn (string*,vector<string>*);
};

#endif