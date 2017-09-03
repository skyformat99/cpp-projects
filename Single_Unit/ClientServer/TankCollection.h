#ifndef __tankCollection
#define __tankCollection

#include <vector>
#include <string>

#include "PooledSort.h"
#include "SpikeList.h"
#include "VectorUtilities.h"

using namespace std;

#define FILE_SEP            "/"

#define TANK_SUFFIX       ".tev"

#define SU_DIR_SEARCH     "_Extracted_*"

#define TANK_POOL_PREFIX    "Tank"
#define TANK_POOL_SUFFIX    "_BlockAll_PooledSort.mat"

#define TANK_SPIKE_PREFIX   "Classed_Spikes_Tank"
#define TANK_SPIKE_MIDDLE   "_Chan"
#define TANK_SPIKE_SUFFIX   "N=*"

class TankCollection {

private:
  vector<string> dirs;

  void initSearchDirectories (string);

public:
  TankCollection (string);
  ~TankCollection ();

  static bool findFile (string, string, string*);
  static bool findFile (string, string, vector<string>*);

  bool findPooling (string, string, PooledSort&);
  bool findSpikes (string, string, SpikeList**, PooledSort*);
  bool findTankFiles (string, string*);
  int findTankFiles (string, vector<string>*);
  bool findValidSingleUnitDir (string, string *);

  void printDirectories ();

};

#endif