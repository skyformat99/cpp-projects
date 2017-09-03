#ifndef __tank__
#define __tank__

#include <string>
#include <iostream>
#include <math.h>
#include <sstream>

#include "Block.h"

#include "XMLNode.h"
#include "XMLNodeList.h"

#include "SingleUnitDir.h"
#include "MySQLpp.h"
#include "TTank.h"

#include "PooledSort.h"
#include "SpikeList.h"
#include "Timer.h"
#include "UploadUtil.h"

/** Writes output message followed by elapsed-time **/
#define TIMEROUT(a) \
  ss.str(""); \
  ss << a; \
  timer->print(&cout,&(ss.str()));

/** Number by which to multiply floating points prior to 'floor()' call */
#define PSTH_MULT 1000000

class Tank {

private:
  bool isLoaded;

  string* name;

  Block** blocks;
  int numBlocks;

  void gatherChannels (PooledSort*,SingleUnitDir*,int,int*,
      string*,fstream*);
  void loadPooledSort (PooledSort*,SingleUnitDir*,fstream*);
  void loadSpikeLists (PooledSort*,SpikeList**,int,
      int*,string*,fstream*);

public:
  Tank ();
  ~Tank ();

  string getName ();

  void loadFromXML (XMLNode*);

  void upload (SingleUnitDir*,MySQLpp*,TTank*,fstream*,string*,int);

};

#endif