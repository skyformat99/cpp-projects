#include "TankCollection.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <io.h>

/** Prints out "Found match!" for findFile method */
#define DEBUG1 false

/** Prints out "Looking for " for findTank, findPooling methods */
#define DEBUG2 false

/** Prints out debug info for findValidSingleUnitDir method */
#define DEBUG3 false

/** Prints out each line when initializing Search Dir's */
#define DEBUG4 false

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

/**
 * Declare a new TankCollection, and initialize the search directories
 * from the property file string.
 */
TankCollection::TankCollection (string propFile)
{
  initSearchDirectories(propFile);
}

/**
 * Finalize the tank Collection
 */
TankCollection::~TankCollection ()
{
  dirs.clear();
}

// * * * * * * * * * * * * * * * *
// * Public Methods 
// * * * * * * * * * * * * * * * *

/**
 * Finds a given file in the given directory.
 */
bool TankCollection::findFile (string file, string path, string *result) 
{
  stringstream ss;
  long iResult;
  struct _finddata_t fileinfo;

  iResult = _findfirst(file.c_str(),&fileinfo);

  if (iResult == -1L) return false;

  ss << path << fileinfo.name;

  *result = ss.str();

  if (DEBUG1) cout << "Found match! " << *result << endl;

  _findclose(iResult);

  return true;
}

/**
 * Finds a set of files in the given path using the matching string.
 */
bool TankCollection::findFile (string file, string path, vector<string> *result)
{
  stringstream ss;
  long iResult;
  struct _finddata_t fileinfo;

  iResult = _findfirst(file.c_str(),&fileinfo);

  if (iResult == -1L) return false;

  ss << path << fileinfo.name;
  result->push_back(ss.str());
  ss.str("");

  while( _findnext(iResult, &fileinfo) == 0 ) {

    ss << path << fileinfo.name;
    result->push_back(ss.str());
    ss.str("");

  }

  if (DEBUG1) cout << "Found " << result->size() << " matches!" << endl;

  return true;
}

/**
 * Finds the given pooling file, and populates the PooledSort 
 * reference.
 */
bool TankCollection::findPooling (string tankName, 
         string tankDir, PooledSort& ps)
{
  stringstream ss;
  ss.str("");

  // First make sure we're "slash-terminated"
  char c;
  c = tankDir.at(tankDir.length()-1);
  if (c != '/' && c != '\\')
    tankDir = *(new string(tankDir + FILE_SEP));

  // Generate the file we're looking for
  ss << tankDir << TANK_POOL_PREFIX << tankName << TANK_POOL_SUFFIX;

  if (DEBUG2) cout << "Looking for " << ss.str() << endl;

  string ps_file;
  try {
    if (!findFile(ss.str(),tankDir,&ps_file))
      return false;
  } catch (...) {
    cerr << "Error finding file in findPooling method" << endl;
    return false;
  }

  try {
    ps.loadFile(ps_file.c_str());
  } catch (...) {
    cerr << "Error loading PooledSort file once found" << endl;
    return false;
  }

  if (ps.isLoaded())
    return true;
  else
    return false;
}

/**
 * Finds the spikes in the given tankCollection, and populates
 * the SpikeList pointer array.
 */
bool TankCollection::findSpikes (string tankName, string tankDir, 
         SpikeList** spikes, PooledSort* ps) 
{
  stringstream ss;
  string channelFileName;
  int i, numSorts;
  int numChannels = ps->numActiveChannels();
  int *sorts = NULL;
  bool isLoaded = true;

  // First make sure we're "slash-terminated" on the tankDir
  char c;
  c = tankDir.at(tankDir.length()-1);
  if (c != '/' && c != '\\')
    tankDir = *(new string(tankDir + FILE_SEP));

  // Get the channel numbers
  int *channels = new int[numChannels];
  ps->getChannels(channels,numChannels);

  // Loop over each channel and load in the spikeTimes
  for (i=0;i<numChannels;i++) {

    // Finds the channelFileName based on search parameters
    ss.str("");
    ss << tankDir << TANK_SPIKE_PREFIX << tankName << TANK_SPIKE_MIDDLE 
       << channels[i] << TANK_SPIKE_SUFFIX;
    
    if (!findFile(ss.str(),tankDir,&channelFileName)) {
      isLoaded = false;
      break;
    }

    channelFileName = channelFileName.substr(0,channelFileName.length()-4);

    // Get the sorts - if we can't load them, break
    numSorts = ps->getNumSorts(channels[i]);

    if (sorts != NULL)
      delete sorts;

    sorts = new int[numSorts];
    if (ps->getSorts(channels[i],sorts,numSorts) != 0) {
      isLoaded = false;
      break;
    }

    // And now go load the spikeTime file
    spikes[i]->loadFile(channels[i],(char*)channelFileName.c_str(),sorts,numSorts);

    isLoaded = isLoaded && spikes[i]->isLoaded();
  }

  try {
    if (sorts != NULL)
      delete sorts;
    delete channels;
  } catch (...) {
    cerr << "Error deleting following a !findFile in TankCollection" << endl;
  }

  return isLoaded;
}

/**
 * Finds the given tank in the tankCollection. Returns true
 * if the tank is found, false if not.
 */
bool TankCollection::findTankFiles (string tank, string* dir) 
{
  stringstream ss;
  ss.str("");

  bool isFound = false;

  vector<string>::iterator it;
  for ( it = dirs.begin() ; it < dirs.end(); it++ ) {

    ss.str("");
    ss << *it << tank << TANK_SUFFIX;

    if (DEBUG2) cout << "Looking for tank " << ss.str() << endl;

    if (!findFile(ss.str(),*it,dir))
      continue;
    else 
      return true;
  }

  return false;
}

int TankCollection::findTankFiles (string tank, vector<string> *result) 
{
  int numAdded = 0;
  stringstream ss;
  ss.str("");

  vector<string> singleResult;

  vector<string>::iterator it;
  for ( it = dirs.begin() ; it < dirs.end(); it++ ) {

    singleResult.clear();

    ss.str("");
    ss << *it << tank << TANK_SUFFIX;

    if (DEBUG2) cout << "Looking for tank " << ss.str() << endl;

    if (!findFile(ss.str(),*it,&singleResult))
      continue;
    else 
      numAdded += VectorUtilities::mergeUniqueElements(result,&singleResult);
  }

  return numAdded;
}

/**
 * Finds a single-unit directory that contains the pooling file. 
 * if none exists, returns false, otherwise returns true.
 */
bool TankCollection::findValidSingleUnitDir (string tank, string *dir)
{
  stringstream ss;

  vector<string> *suDirs = new vector<string>();

  vector<string>::iterator it;
  for (it = dirs.begin(); it < dirs.end(); it++) {

    ss.str("");
    ss << *it << tank << SU_DIR_SEARCH;

    if (!findFile(ss.str(),*it,suDirs))
      continue;
  }

  if (DEBUG3) cout << "Found " << suDirs->size() << " possible SU dir's!" << endl;

  if (suDirs->size() == 0)
    return false;

  char c;
  string ps_file;
  string *s;

  // Now iterate through all suDirectories that we've gathered,
  // and see if we can find a pooling file here
  for (it = suDirs->begin(); it < suDirs->end(); it++) {
    s = new string(*it);

    // Make sure 's' is slash-terminated
    c = s->at(s->length()-1);
    if (c != '/' && c != '\\') 
      s = new string(*s + FILE_SEP);

    if (DEBUG3) cout << "  Looking in " << *s << endl;

    // Generate the pooling file we're looking for
    ss.str("");
    ss << *s << TANK_POOL_PREFIX << tank << TANK_POOL_SUFFIX;

    if (!findFile(ss.str(),*s,&ps_file)) {
      delete s;
      continue;
    }

    *dir = *s;

    delete s;
    delete suDirs;

    return true;
  }

  delete suDirs;

  return false;
}

/**
 * Print the directories used to search for the tanks.
 */
void TankCollection::printDirectories () {

  vector<string>::iterator it;
  cout << "Dirs vector contains:";
  for ( it = dirs.begin() ; it < dirs.end(); it++ )
    cout << "\n\t" << *it;

  cout << endl;

}

// * * * * * * * * * * * * * * * *
// * Private Methods 
// * * * * * * * * * * * * * * * *

/**
 * Read in the directories from the properties file.
 * These are directories used to search for the tank file.
 */
void TankCollection::initSearchDirectories (string propFile)
{
  dirs.clear();

  string dir_string;
  char c;

  // Open the file stream
  fstream filestr (propFile.c_str(), fstream::in);

  if (!filestr.is_open())
    throw "initSearchDirectories(): Error opening property file!";

  try {
    getline(filestr,dir_string);
  } catch (...) {
    throw "initSearchDirectories(): error getting line";
  }

  if (DEBUG4) cout << "  line: #" << dir_string << "#" << endl;

  // Loop through the istream until EOF is reached
  while (!filestr.eof()) {

    // If the read line is "", we're done
    if (dir_string.length() == 0)
      break;

    // Verify the line's "slash-capped"
    c = dir_string.at(dir_string.length()-1);
    if (c != '/' && c != '\\')
      dir_string += FILE_SEP;
    
    // And put it on the vector
    dirs.push_back(dir_string);

    // Read the next line
    try {
      getline(filestr,dir_string);
    } catch (...) {
      throw "initSearchDirectories(): error getting line";
    }

    if (DEBUG4) cout << "  line: #" << dir_string << "#" << endl;
  }

  try {
    filestr.close();
  } catch (...) {
    throw "initSearchDirectories(): error closing filestream";
  }
}

