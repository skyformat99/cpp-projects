#include "Block.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

Block :: Block ()
{
  isLoaded = false;

  name = NULL;
  number = -1;

  blockID = -1;

  vector<int>* ecIDs = NULL;
  vector<string>* ecCodes = NULL;

  module = NULL;
}

Block :: ~Block ()
{
  if (name) delete name;
  if (module) delete module;

  if (ecIDs) delete ecIDs;
  if (ecCodes) delete ecCodes;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

int Block :: gatherEventCodes (MySQLpp* mysql, fstream* logstream) 
{
  ecIDs = new vector<int>();
  ecCodes = new vector<string>();

  if (!module)
    throw "Error: can't gather event codes, module is null!";

  try {
    module->getEventCodes(mysql,ecCodes,ecIDs);
  } catch (char* c) {
    ERROUT("Error: couldn't get event codes from blockID " << blockID
        << ": " << c << endl);
  }

  if (ecIDs->size() != ecCodes->size())
    throw "Error: event code vectors do not agree in size!";

  return ecIDs->size();
}

int Block :: getBlockNumber ()
{
  return number;
}

int Block :: getNumSweeps (TTank* tt)
{
  if (tt->selectBlock(number) != 0) 
    throw "Error: unable to select block in ttank";

  tt->createEpocIndexing();

  int count;
  try {
    count = tt->eventRead("Swep",MAX_EVENTS);
  } catch (char* c) {
    throw "Error: unable to read Swep events in block";
  }

  if (count == MAX_EVENTS)
    throw "Count == MAX_EVENTS, probably an error";

  return count;
}

void Block :: loadFromXML (XMLNode* block)
{
  name = new string(block->getAttribute("name"));
  number = atoi((block->getAttribute("number")).c_str());

  module = new Module();
  module->setName(block->getAttribute("module"));
  module->loadProperties(block);

  isLoaded = true;
}

void Block :: populateArrays (int numSweps, int* swepVals, double* swepTimes,
    int numEventCodes, int* eventCodeIDs, int** codeVal, 
    TTank* tt, fstream* logstream)
{
  int i,j;

  // First populate the eventCodeID array from the vector
  for (i=0;i<numEventCodes;i++)
    eventCodeIDs[i] = (*ecIDs)[i]; 

  // Connect to the block on the TTank server
  if (tt->selectBlock(number) != 0) 
    throw "Error: unable to select block in ttank";

  tt->createEpocIndexing();

#ifdef DEBUG
  cout << __FILE__ << ":(" << __LINE__ << "): ";
  cout << "Finished connecting to TTank block" << endl;
#endif  

  // First deal with swep's
  int swepCount;
  try {
    swepCount = tt->eventRead("Swep",MAX_EVENTS);
  } catch (char* c) {
    throw "Error: unable to read Swep events in block";
  }

  if (swepCount == MAX_EVENTS)
    throw "Count == MAX_EVENTS, probably an error";

  if (swepCount != numSweps)
    throw "Error: new swepCount != old swepCount";

  double* tmpSwep = new double[numSweps];
  double* tmpSwepTimes = new double[numSweps];

  try {
    tt->eventParse(numSweps,tmpSwep,numSweps);
    tt->eventParseInfo(numSweps,6,tmpSwepTimes,numSweps);
  } catch (char* c) {
    throw "Error parsing swep data from block";
  }

#ifdef DEBUG
  cout << __FILE__ << ":(" << __LINE__ << "): ";
  cout << "Parsed Swep data: numSweps = " << numSweps << endl;
#endif  

  // Now deal with other event codes
  int* eventCount = new int[numEventCodes];
  double** tmpEventData = new double*[numEventCodes];
  for (i=0;i<numEventCodes;i++) {

    try {
      eventCount[i] = tt->eventRead((*ecCodes)[i],MAX_EVENTS);
    } catch (char* c) {
      throw "Error: unable to read events in block";
    }

    tmpEventData[i] = new double[eventCount[i]];

    try {
      tt->eventParse(eventCount[i],tmpEventData[i],eventCount[i]);
    } catch (char* c) {
      throw "Error parsing other data from block";
    }

#ifdef DEBUG
    cout << __FILE__ << ":(" << __LINE__ << "): ";
    cout << "  EC (" << i << "): " << (*ecCodes)[i] << ": eventCount = " << eventCount[i] << endl;
#endif  

  }

  // First see if we need to truncate swepCount due to a shorter eventCodeCount
  // assume eventCount[i] == 732
  //        numSweps == 733 
  //   - then this would iterate for j = 732 only
  // assume eventCount[i] == 733
  //        numSweps == 733
  //   - then this wouldn't iterate at all
  bool doneTruncate = false;
  for (i=0;i<numEventCodes;i++) {
    for (j=eventCount[i];j<numSweps;j++) {
      doneTruncate = true;
      tmpSwep[j] = -1;
    }
  }

  // If we did any truncation, write out a short note to the log
  if (doneTruncate) 
    (*logstream) << "Truncating swepCount down to eventCount on block " 
                 << number << endl;

  // Now make sure $swepCount is not < any eventCodeCount
  stringstream errStr;
  double* eventDataSwap;
  for (i=0;i<numEventCodes;i++) {
    if (numSweps < eventCount[i]) {

#ifdef DEBUG
      cout << __FILE__ << ":(" << __LINE__ << "): ";
      cout << "Throw error here: numSweps: " << numSweps 
           << " tmpSwep[" << (numSweps-1) << "]: " << tmpSwep[numSweps-1] 
           << " " << (*ecCodes)[i] << " count: " << eventCount[i] << endl;
#endif

      // First verify that tmpSwep[end] <= eventCount[i], meaning that
      // Sweps act as an index (from 1 ... n) into tmpEventData array
      if (tmpSwep[numSweps-1] > eventCount[i]) {
        errStr << "Error: Swep count (" << numSweps 
               << ") lower than code (" << (*ecCodes)[i] 
               << ") count (" << eventCount[i] << ") on block "
               << number << " and cannot be fixed with truncation" << endl;
        throw errStr.str();
      }

      // To fix: we need to remove the eventCode data for the sweps that are missing

      // First copy the event data into a swap buffer
      eventDataSwap = new double[eventCount[i]];
      for (j=0;j<eventCount[i];j++)
        eventDataSwap[j] = tmpEventData[i][j];

      // Now delete the old event Data
      delete [] tmpEventData[i];

      // Create a new buffer with the size of the sweps
      tmpEventData[i] = new double[numSweps];

      // Use the swep value to index the eventDataSwap, and fill the data array
      try {
        for (j=0;j<numSweps;j++)
          tmpEventData[i][j] = eventDataSwap[(int)(tmpSwep[j]-1)];
      } catch (...) {
        errStr << "Error: index out of bounds attempting to fix swep count ("
               << numSweps << ") lower than code count (" << eventCount[i] 
               << ") on block " << number << endl;
        throw errStr.str();
      }

      delete [] eventDataSwap;

      // For now, report this info out, so that the user can verify the blocks
      cerr << "[Fixed SwepCount on Block " << number << "]";
      (*logstream) << "Fixed swepCount < eventCount by truncating eventData to swepData" << endl;
    }
  }
  
  // Now populate the arrays passed as arguments
  for (i=0;i<numSweps;i++) {
    swepVals[i] = (int)tmpSwep[i];
    swepTimes[i] = tmpSwepTimes[i];

    if (swepVals[i] != -1) {

      // Remember that codeVal (from Tank.cpp) is in the order
      // [numEventCodes][numSweps] 
      for (j=0;j<numEventCodes;j++) {
        codeVal[j][i] = (int)tmpEventData[j][i];
      }
    }
  }

#ifdef DEBUG
  cout << __FILE__ << ":(" << __LINE__ << "): ";
  cout << "Populated all arrays passed as arguments" << endl;
#endif  

  // And lastly, clean up all allocated memory
  if (tmpSwep)      delete [] tmpSwep;
  if (tmpSwepTimes) delete [] tmpSwepTimes;

  for (i=0;i<numEventCodes;i++)
    if (tmpEventData[i]) delete [] tmpEventData[i];

  if (eventCount)   delete [] eventCount;
  if (tmpEventData) delete [] tmpEventData;
}

int Block :: upload (MySQLpp* mysql, int tankID, string* dbStr, 
    fstream* logstream)
{
  // Get the module ID from the database
  int moduleID;
  try {
    moduleID = module->gatherID(mysql);
  } catch (char* c) {
    ERROUT("Error gathering module ID on tank_id " << tankID << ": " 
        << c << endl);
  }

  // Insert the block 
  stringstream ss, errorMsg;
  ss << "INSERT INTO " << *dbStr << ".block (tank_id, block, "
     << " module_id) VALUES (" << tankID << ", " << number 
     << ", " << moduleID << ")";

  errorMsg << "Error inserting block " << number << " on tankID "
           << tankID << "!";

  try {
    blockID = mysql->insertSingleItem(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error inserting block " << number << " tankID " << tankID 
        << ": " << c << endl);
  }

  // Upload the module properties
  module->upload(mysql,blockID,dbStr,logstream);

  return blockID;
}


// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *