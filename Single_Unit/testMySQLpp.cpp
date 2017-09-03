#include <string>
#include <iostream>

#include "MySQLpp.h"

using namespace std;

/**
 * Main entry into function. 
 */
int main (int argc, char *argv[]) {

  // Setup MySQL connection object
  string dbHost = "uberwalton.urmc-sh.rochester.edu";
  string dbUser = "joe";
  string dbPass = "joepass";
  string dbStr = "db_util";
  MySQLpp* mysql = new MySQLpp();
  try {
    mysql->connectToServer(dbHost,dbUser,dbPass,dbStr,0);
  } catch (char* c) {
    throw "Error connecting to MYSQL server!";
  }

  string query = "SHOW databases";
  vector<string> vec;

  mysql->querySingleColumn(&query,&vec);

  cout << "Vector size: " << vec.size() << endl;

  int i;
  for (i=0;i<vec.size();i++)
    cout << "(" << i << ") " << vec[i] << endl;

  exit(0);
}