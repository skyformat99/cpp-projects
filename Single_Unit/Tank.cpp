#include "Tank.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

Tank :: Tank ()
{
  isLoaded = false;

  name = NULL;

  blocks = NULL;
  numBlocks = 0;
}

Tank :: ~Tank ()
{
  int i;
  for (i=0;i<numBlocks;i++)
    delete blocks[i];

  if (blocks) delete [] blocks;

  if (name) delete name;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

string Tank :: getName ()
{
  if (isLoaded)
    return (*name);
  else
    throw "Error: tank is not loaded!";
}

void Tank :: loadFromXML (XMLNode *tank)
{
  name = new string(tank->getAttribute("name"));

  XMLNodeList* blockList = new XMLNodeList();
  tank->getNodes("block",blockList);

  numBlocks = blockList->length();

  blocks = new Block*[numBlocks];

  int i;
  XMLNode *block;
  for (i=0;i<numBlocks;i++) {
    blocks[i] = new Block();

    block = new XMLNode();
    blockList->getNode(i,block);

    blocks[i]->loadFromXML(block);

    delete block;
  }

  delete blockList;

  isLoaded = true;
}

void Tank :: upload (SingleUnitDir* su, MySQLpp* mysql, 
    TTank* ttank, fstream* logstream, string* dbStr, int exptID)
{
  stringstream ss, errorMsg;

  // Declare timer
  Timer* timer = new Timer();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Insert the tank information 
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  ss << "INSERT INTO " << *dbStr << ".tank (expt_id, name) VALUES " 
     << "(" << exptID << ", '" << *name << "')";

  errorMsg << "Error inserting tank " << *name << " into dB";

  int tankID;
  try {
    tankID = mysql->insertSingleItem(&(ss.str()),&errorMsg);
  } catch (char* c) {
    ERROUT("Error inserting tank: " << c << endl);
  }

  cout << "  Uploading tank " << *name << " (ID " << tankID << ") ";
#ifdef VERBOSE
  cout << endl;
#endif

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Initialize a new PooledSort object
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  PooledSort* ps = new PooledSort();
  loadPooledSort(ps,su,logstream);

  int numChannels = ps->numActiveChannels();
  int* channels = new int[numChannels];
  string* channelNames = new string[numChannels];

  gatherChannels(ps,su,numChannels,channels,channelNames,logstream);

#ifdef VERBOSE
  TIMEROUT("    Number of active channels: " << numChannels);
#else
  cout << ".";
#endif

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Initialize and load SpikeList array
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-    
  int i,j,k;
  SpikeList** s = new SpikeList*[numChannels];
  for (i=0;i<numChannels;i++)
    s[i] = new SpikeList();

  loadSpikeLists(ps,s,numChannels,channels,
      channelNames,logstream);

#ifdef VERBOSE
  TIMEROUT("    SpikeLists loaded!");
#else
  cout << ".";
#endif

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Prepare TTank for this tank
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  if (ttank->openTank(*name) != 0) {
    ERROUT("Could not open tank " << *name << " on ttank" << endl);
  }

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Declare arrays & matrices
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  (*logstream) << "  Number of blocks: " << numBlocks << endl;

#ifdef VERBOSE
  TIMEROUT("    Number of blocks: " << numBlocks);
#else
  cout << ".";
#endif

  int *blockNumber = new int[numBlocks];
  int *blockIDs = new int[numBlocks];
  int *numEventCodes = new int[numBlocks];
  int *numSweps = new int[numBlocks];

  int **eventCodeID = new int*[numBlocks];
  int **swepVals = new int*[numBlocks];
  double **swepTimes = new double*[numBlocks];
  int **swepID = new int*[numBlocks];

  int ***codeVal = new int**[numBlocks];

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Upload block data & start getting properties
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
  for (i=0;i<numBlocks;i++) {

#ifdef DEBUG
    cout << __FILE__ << ":(" << __LINE__ << "): ";
    cout << "Block " << blocks[i]->getBlockNumber() << " (i=" << i << ")" << endl;
#endif

    blockNumber[i] = blocks[i]->getBlockNumber();
    blockIDs[i] = blocks[i]->upload(mysql,tankID,dbStr,logstream);

#ifdef DEBUG
    cout << __FILE__ << ":(" << __LINE__ << "): ";
    cout << "Finished uploading block data" << endl;
#endif

    // Get the number of Sweps for the block, and declare arrays
    try {
      numSweps[i] = blocks[i]->getNumSweeps(ttank);
    } catch (char* c) {
      ERROUT("Error getting sweps from tank " << *name << " block "
          << blockNumber[i] << ": " << c << endl);
    }

    if (numSweps[i] == 0) {
      swepVals[i] = NULL;
      swepTimes[i] = NULL;
      swepID[i] = NULL;
    } else {
      swepVals[i] = new int[numSweps[i]];
      swepTimes[i] = new double[numSweps[i]];
      swepID[i] = new int[numSweps[i]];
    }

    // Get the number of event codes and declare arrays
    numEventCodes[i] = blocks[i]->gatherEventCodes(mysql,logstream);
    
    if (numEventCodes[i] == 0) {
      eventCodeID[i] = NULL;
      codeVal[i] = NULL;
    } else {
      eventCodeID[i] = new int[numEventCodes[i]];
      codeVal[i] = new int*[numEventCodes[i]];

      for (j=0;j<numEventCodes[i];j++) {
        codeVal[i][j] = new int[numSweps[i]];
      }
    }

#ifdef DEBUG
    cout << __FILE__ << ":(" << __LINE__ << "): ";
    cout << "NumSweeps: " << numSweps[i] << " numEventCodes: " << numEventCodes[i] << endl;
#endif

    try {    
      blocks[i]->populateArrays(numSweps[i],swepVals[i],swepTimes[i],
          numEventCodes[i],eventCodeID[i],codeVal[i],ttank,logstream);
    } catch (char* c) {
      ERROUT("Error populating block arrays on tank " 
          << *name << ": " << c << endl);
    } catch (string s) {
      ERROUT("Error populating block arrays on tank "
          << *name << ": " << s << endl);
    } catch (...) {
      ERROUT("Error populating block arrays on tank "
          << *name << endl);
    }

#ifdef DEBUG
    cout << __FILE__ << ":(" << __LINE__ << "): ";
    cout << "Finished populating arrays" << endl;
#endif

    for (j=0;j<numSweps[i];j++) {

      if (swepVals[i][j] != -1) {

        ss.str("");
        ss << "INSERT INTO " << *dbStr << ".sweep_list "
           << "(block_id,sweepNum) VALUES ("
           << blockIDs[i] << "," << swepVals[i][j] << ")";

        errorMsg.str("");
        errorMsg << "Error on swep (" << swepVals[i][j] << ") block (" 
                 << i << ") tank (" << *name << ") insert\n";

        try {
          swepID[i][j] = mysql->insertSingleItem(&(ss.str()),&errorMsg,
                             true,false);
        } catch (char* c) {
          ERROUT("Error inserting swep data: " << c << endl);
        }

      } 

    } // end iteration over numSweeps

  } // end iteration over numBlocks

  (*logstream) << "  Finished populating arrays from TTank!" << endl;

#ifdef VERBOSE
  TIMEROUT("    Finished populating arrays from TTank!");
#else
  cout << ".";
#endif

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Set up monster condition_list insert
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  ss.str("");
  ss << "INSERT INTO " << *dbStr << ".condition_list "
     << " (sweep_list_id,param_id,param) VALUES ";

  int uploadCount = 0;

  for (i=0;i<numBlocks;i++) {

    for (j=0;j<numSweps[i];j++) {

      if (swepVals[i][j] != -1) {

        for (k=0;k<numEventCodes[i];k++) {
 
          ss << "(" << swepID[i][j] << "," << eventCodeID[i][k] << ","
             << codeVal[i][k][j] << "),";
          uploadCount++;

        } // end numEventCodes iteration

      }  // end if

    } // end numSweeps iteration

  } // end numBlocks

  string sql;
  sql = ss.str();
  sql = sql.substr(0,sql.length()-1); // remove comma at end

  errorMsg.str("");
  errorMsg << "Error uploading condition list elements on tank ("
           << *name << ")\n";

  if (uploadCount > 0) {
    try {
      mysql->insertMultiItem(&sql,&errorMsg);
    } catch (char* c) {
      ERROUT("Error inserting condition elements: " << c << endl);
    }

    (*logstream) << "  Uploaded condition list elements for tank!" << endl;
  } else {
    (*logstream) << "  Nothing to upload for tank " << *name << "!" << endl;
  }

#ifdef VERBOSE
  TIMEROUT("    Uploaded condition list elements for tank!");
#else
  cout << ".";
#endif

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // For each channel & sort, do insert
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  int spikesPerSort[3];
  int m, n;
  int insertID, bOffset, numSpikes, sI, spikeCount, spikeListNumBlocks;
  double *spikeTimes;
  int *spikeClass; 
  stringstream psthBuf;
  int maxSort;
  int unitID;

  for (i=0;i<numChannels;i++) {

    // Run on each sort - sorts are from 1 to maxPooling
    maxSort = ps->getMaxSort(channels[i]);

    (*logstream) << "  - Starting on channel " << channels[i] 
                 << " maxSort: " << maxSort << endl;

    if (maxSort > 2) {
      ERROUT("Error on tankID " << tankID << " channel " << channels[i] << 
                " maxSort > 2 (value is " << maxSort << ")" << endl);
    }

    (*logstream) << "    numSpikes: " << s[i]->getNumSpikes() << endl;

    s[i]->getSpikesPerSort(spikesPerSort);

    for (j=1;j<=maxSort;j++) {

      if (spikesPerSort[j] == 0) continue;

      (*logstream) << "   Starting sort: " << j << " spikesPerSort: " 
                   << spikesPerSort[j] << endl;

      ss.str("");
      ss << "INSERT INTO unit (tank_id,channel,sort,num_spikes) VALUES ("
         << tankID << "," << channels[i] 
         << "," << j << "," << spikesPerSort[j] << ")";

      errorMsg.str("");
      errorMsg << "Could not complete MYSQL unit insert ("
               << channels[i] << " : id " << j << ") tank "
               << *name << endl;
      try {
        unitID = mysql->insertSingleItem(&(ss.str()),&errorMsg);
      } catch (char* c) {
        ERROUT("Error writing units to dB: " << c << endl);
      }

      numSpikes = s[i]->getNumSpikes();
      spikeTimes = s[i]->getSpikeTimes();
      spikeClass = s[i]->getSpikeClasses();

      spikeListNumBlocks = s[i]->getNumBlocks();

      (*logstream) << "   Channel " << channels[i] << " Sort " << j << " # Spikes "
                   << spikesPerSort[j] << " Total Spikes " << numSpikes 
                   << " unit ID " << unitID << endl;

      sI = 0;

      for (k=0;k<numBlocks;k++) {

        bOffset = s[i]->getBlockNextOffset(blockNumber[k]);

        sI = (blockNumber[k] == 0) ? 0 : s[i]->getBlockOffset(blockNumber[k]);

        // Increments spikeIndex to get past any spikes that jumped the stimulus gun
        while (spikeTimes[sI] <= swepTimes[k][0]) sI++;

        (*logstream) << "   -  Block-" << blockNumber[k] << " k: " << k << " sI: "
                     << sI << " bOffset: " << bOffset << endl;

        // Reset the String Buffer for this unit insert
        ss.str("");
        ss << "INSERT INTO unit_data (unit_id,sweep_list_id,spike_count,psth) VALUES ";

        boolean anyInserts = false;

        // Loop through all sweps but the last
        for (m=0;m<(numSweps[k]-1);m++) {

          psthBuf.str("");
          spikeCount = 0;

          while (sI < numSpikes &&
                 spikeTimes[sI] > swepTimes[k][m] &&
                 spikeTimes[sI] < swepTimes[k][m+1] &&
                 sI < bOffset) {

            if (spikeClass[sI] != j) { sI++; continue; }

            if (spikeCount > 0) psthBuf << " ";
            psthBuf << floor((spikeTimes[sI] - swepTimes[k][m]) * PSTH_MULT);

            spikeCount++;

            sI++;
          }

          // If we have anything to insert, do it here ...
          if (spikeCount > 0 && swepVals[k][m] != -1) {

            ss << "(" << unitID << "," << swepID[k][m] << "," << spikeCount
               << ",'" << psthBuf.str() << "'),";
            anyInserts = true;

          }

        } // end loop over sweeps through numSweps[k]-1

        psthBuf.str("");
        spikeCount = 0;

        // Now deal with last loop
        while (sI < numSpikes &&
               sI < bOffset) {

          if (spikeClass[sI] != j) { sI++; continue; }

          if (spikeCount > 0) psthBuf << " ";
          psthBuf << floor((spikeTimes[sI] - swepTimes[k][m]) * PSTH_MULT);

          spikeCount++;

          sI++;
        }

        // If we have anything to insert from last swep, do it here ...
        if (spikeCount > 0 && swepVals[k][m] != -1) {

          ss << "(" << unitID << "," << swepID[k][m] << "," << spikeCount
             << ",'" << psthBuf.str() << "'),";
          anyInserts = true;

        }

        // If we have anything in the buffer, we need to do the insert
        if (anyInserts) {
          sql = ss.str();
          sql = sql.substr(0,sql.length()-1);

          errorMsg << "Could not complete MYSQL full unit insert ("
                   << unitID << ") tank " << *name 
                   << " SQL Length: " << sql.length() << endl;

          try {
            mysql->insertMultiItem(&sql,&errorMsg);
          } catch (char* c) {
            ERROUT("Error writing full unit data: " << c << endl);
          } catch (...) {
            ERROUT("Error writing full unit data - tank " << *name << " len: " << sql.length() << endl);
          }
        }

      } // end loop over blocks

    } // end loop over sorts

  } // end loop over channels

#ifdef VERBOSE
  TIMEROUT("    Finished uploading individual unit data!");
#else
  cout << " (" << timer->elapsed() << " sec)" << endl;
#endif

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Clean up and delete arrays & objects
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-    
  ttank->closeTank();

  for (i=0;i<numBlocks;i++) {
    for (j=0;j<numEventCodes[i];j++) {
      if (codeVal[i][j]) delete [] codeVal[i][j];
    }

    if (eventCodeID[i]) delete [] eventCodeID[i];
    if (swepVals[i])    delete [] swepVals[i];
    if (swepTimes[i])   delete [] swepTimes[i];
    if (swepID[i])      delete [] swepID[i];
    if (codeVal[i])     delete [] codeVal[i];
  }

  if (eventCodeID) delete [] eventCodeID;
  if (swepVals)    delete [] swepVals;
  if (swepTimes)   delete [] swepTimes;
  if (swepID)      delete [] swepID;
  if (codeVal)     delete [] codeVal;

  if (blockNumber)   delete [] blockNumber;
  if (blockIDs)      delete [] blockIDs;
  if (numEventCodes) delete [] numEventCodes;
  if (numSweps)      delete [] numSweps;

  if (channelNames) delete [] channelNames;
  if (channels)     delete [] channels;

  for (i=0;i<numChannels;i++) {
    delete s[i];
  }

  if (s) delete [] s;
  if (ps) delete ps;
  if (timer) delete timer;
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

/**
 * Gathers the active channels & names into the given arrays,
 * and writes the results to the log stream.
 */
void Tank :: gatherChannels (PooledSort* ps, SingleUnitDir* su, 
    int numChannels, int* channels, string* channelNames, 
    fstream* logstream)
{
  // Find the active channel numbers and file names
  int i,j=0;
  string channelFilename;

  for (i=1;i<=N_CHAN;i++) {
    if (ps->isChannelActive(i)) {
      channels[j] = i;
      
      if (!su->getChannelFileBase(*name,i,&channelFilename)) {
        ERROUT("Error: could not get channel name for channel "
            << i << " on tank " << *name << endl);
      }
      channelNames[j] = channelFilename;

      (*logstream) << "    (" << i << "): " << channelNames[j] << endl;
      j++;
    }
  }
}

/**
 * Loads the PooledSort object given the SU directory and the tank 
 * name. Handles error checking and writing out to the log.
 */
void Tank :: loadPooledSort (PooledSort* ps, SingleUnitDir* su,
    fstream* logstream)
{
  string poolFilename;
  if (!su->getPoolFile(*name,&poolFilename)) {
    ERROUT("Error getting pool file name for tank " << *name << endl);
  }

  // Load the file, return error if exists
  try {
    ps->loadFile(poolFilename.c_str());
  } catch (char* c) {
    ERROUT("Error loading pooledSort on tank " << *name << ": " << c << endl);
  } catch (...) {
    ERROUT("Error loading pooledSort on tank " << *name << "!" << endl);
  }

  if (!ps->isLoaded()) {
    ERROUT("Error: pooledSort not loaded on tank " << *name << endl);
  }

  (*logstream) << "  PooledSort loaded for tank " << *name << endl;
}

/**
 * Populates the array of spikeList objects by loading the 
 * appropriate channel data. Handles the error catching and
 * writing out to the log. 
 */
void Tank :: loadSpikeLists (PooledSort* ps,
    SpikeList** s, int numChannels, int* channels, 
    string* channelNames, fstream* logstream)
{
  int i, numSorts;
  int* sorts;
  for (i=0;i<numChannels;i++) {
    
    numSorts = ps->getNumSorts(channels[i]);
    sorts = new int[numSorts];
    
    if (ps->getSorts(channels[i],sorts,numSorts) != 0) {
      ERROUT("Error: getSorts returned error on chan " 
          << channels[i] << " from tank " << *name << endl);
    }

    try {
      s[i]->loadFile(channels[i],channelNames[i],sorts,numSorts);
    } catch (char* c) {
      ERROUT("Error on tank " << *name << " creating SpikeList[" 
          << i << "]: " << c << endl);
    } catch (...) {
      ERROUT("Error on tank " << *name << " creating SpikeList[" 
          << i << "]" << endl);
    }

    if (s[i]->isLoaded()) {
      (*logstream) << "  - Loaded SpikeList for channel " 
                   << channels[i] << endl;
    } else {
      ERROUT("Error on tank " << *name << " loading SpikeList for channel " 
        << channels[i] << endl);
    }

    delete [] sorts;
  }
}

