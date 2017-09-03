#ifndef __ttankX
#define __ttankX

#include <process.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <tchar.h>

#include "UploadUtil.h"

#import "progid:TTank.X" no_namespace

using namespace std;

class TTank {

private:

  _DTTankXPtr p;

  void initialize ();

public:

  TTank ();
  TTank (string);
  ~TTank ();

  int connectServer (string);
  int closeServer ();

  int addTank (string,string);
  bool containsTank (string);
  void closeTank ();
  int openTank (string);
  int removeTank (string);

  vector<string> getServers ();
  vector<string> getTanks ();

  void createEpocIndexing ();
  int selectBlock (int);

  int eventRead (string,int);
  void eventParse (int,double*,int);
  void eventParseInfo (int,int,double*,int);
};

#endif