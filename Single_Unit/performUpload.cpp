#include <math.h>

#include <string>
#include <iostream>
#include <sstream>

#include "PooledSort.h"
#include "SpikeList.h"
#include "NumRecipe.h"
#include "Timer.h"
#include "MySQLpp.h"

#include "windows.h"

using namespace std;

/**
 * Note regarding max_allowed_packet variable in MySQL server:
 *   In order to handle large uploads from C++ to the MySQL server, the MySQL
 * GLOBAL variable max_allowed_packet must be set to a large value: currently
 * we are using 16777216. This can be checked using the command:
 *    SHOW variables LIKE 'max_allowed%';
 * and can be set using the command:
 *    SET GLOBAL max_allowed_packet=16777216;
 * If there are problems uploading individual unit data, and an error such as the 
 * following is coming up:
 *   "Could not complete MYSQL full unit insert (366) SQL Length: _____"
 * the this is probably the problem. 
 */

/** Number by which to multiply floating points prior to 'floor()' call */
#define PSTH_MULT 1000000

/** Helper function to write out errors to cerr and logstream **/
#define ERROR_OUT(a) \
  cerr << a; \
  logstream << a; \
  logstream.flush(); \
  logstream.close(); \
  if (mysql) delete mysql; \
  exit(1);

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Main function thread
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int main (int argc, char* argv[]) {

  // Check usage
  if (argc < 4) {
    cerr << "Usage: " << argv[0] << " <root> <base> <db>" << endl;
    cerr << "argc: " << argc << endl;
    exit(1);
  }

  string rootStr = argv[1];
  string baseStr = argv[2];
  string dbStr = argv[3];

  // Declare timer
  Timer *t = new Timer();
  t->reset();

  // Setup output logging file
  SYSTEMTIME st;
  GetLocalTime(&st);
  stringstream logfilename;
  logfilename << "log_" << st.wYear << "-" 
              << st.wMonth << "-" << st.wDay << "_" 
              << st.wHour << "-"  << st.wMinute 
              << "_" << baseStr << ".txt";
  fstream logstream;
  try {
    logstream.open(logfilename.str().c_str(), fstream::out | fstream::app);
  } catch (...) {
    cerr << "Error opening log stream." << endl;
    exit(1);
  }

  logstream << "Root string: " << rootStr << endl;
  logstream << "Base string: " << baseStr << endl;
  logstream << "dB String: " << dbStr << endl;

  // Setup MySQL connection object
  string dbHost = "uberwalton.urmc-sh.rochester.edu";
  string dbUser = "joe";
  string dbPass = "joepass";
  MySQLpp* mysql = new MySQLpp();
  try {
    mysql->connectToServer(dbHost,dbUser,dbPass,dbStr,0);
  } catch (char* c) {
    ERROR_OUT("Error connecting to MySQL server: " << c << endl);
  }
  mysql->initializeDebugging("db_util","cpp_upload_log","str");
  mysql->initializeLogging(&logstream);

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Format and open input file stream
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  stringstream txtfilename;
  txtfilename << rootStr << "blockTemp_" << baseStr << ".txt";

  logstream << "Opening " << txtfilename.str() << " for reading." << endl;

  fstream fp;
  try {
    fp.open(txtfilename.str().c_str(), fstream::in);
  } catch (...) {
    ERROR_OUT("Error opening text file for reading");
  }

  if (!fp.is_open()) {
    ERROR_OUT("Error: data file is not available for reading");
  }

  // Read general tank parameters from the txt file
  int tankID;
  string tankName;
  string suDir;
  string pooledFileBase;

  fp >> tankID;
  fp >> tankName;

  getline(fp,suDir); // Crops out the first '\n'
  getline(fp,suDir);
  if (fp.fail()) { ERROR_OUT("Error reading SU directory"); }

  getline(fp,pooledFileBase);
  if (fp.fail()) { ERROR_OUT("Error reading pooled file base"); }

  logstream << "tankID: " << tankID << " tankName: " << tankName << endl;
  logstream << "SU directory: " << suDir << endl;
  logstream << "Pooled File base: " << pooledFileBase << endl;

  // Now read in channel information
  int numChannels;
  fp >> numChannels;

  logstream << "numChannels: " << numChannels << endl;

  int i;
  int* channels = new int[numChannels];
  string* channelNames = new string[numChannels];

  for (i=0;i<numChannels;i++) {
    fp >> channels[i] >> channelNames[i];
    logstream << "  (" << i << ") Channel " << channels[i] 
              << " name: " << channelNames[i] << endl;
  }

  t->print((ostream*)&logstream,
      new string("Finished gathering tank properties"));
  t->reset();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Initialize PooledSort object and load
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  stringstream ps_filename;
  ps_filename << suDir << pooledFileBase;

  logstream << "Attempting to load pooledSort file (" << ps_filename.str() 
            << ")" << endl;

  PooledSort* ps;
  try {
    ps = new PooledSort(ps_filename.str());
  } catch (char* c) {
    ERROR_OUT("Error: " << c << endl);
  } catch (...) {
    ERROR_OUT("Error loading PooledSort file!" << endl);
  }

  if (ps->isLoaded()) {
    logstream << "  - Loaded PooledSort file." << endl;
  } else {
    ERROR_OUT("Error: PooledSort file is not loaded!");
  }

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Initialize SpikeList array and objects
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  SpikeList** s = new SpikeList*[numChannels];

  stringstream chanFileName;
  int numSorts;
  int* sorts;
  for (i=0;i<numChannels;i++) {
    numSorts = ps->getNumSorts(channels[i]);
    sorts = new int[numSorts];
    
    if (ps->getSorts(channels[i],sorts,numSorts) != 0) {
      ERROR_OUT("Error: getSorts returned error on chan " << channels[i] << endl);
    }

    chanFileName.str("");
    chanFileName << suDir << channelNames[i];

    try {
      s[i] = new SpikeList(channels[i],chanFileName.str(),sorts,numSorts);
    } catch (char* c) {
      ERROR_OUT("Error creating SpikeList[" << i << "]: " << c << endl);
    } catch (...) {
      ERROR_OUT("Error creating SpikeList[" << i << "]" << endl);
    }

    if (s[i]->isLoaded()) {
      logstream << "  - Loaded SpikeList for channel " << channels[i] << endl;
    } else {
      ERROR_OUT("Error loading SpikeList for channel " << channels[i] << endl);
    }

    delete [] sorts;
  }

  t->print((ostream*)&logstream,
      new string("Finished gathering pooling and spike lists"));
  t->reset();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Declare arrays & matrices
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  int numBlocks;
  fp >> numBlocks;

  logstream << "Number of blocks: " << numBlocks << endl;

  int *blockNumber = new int[numBlocks];
  int *blockIDs = new int[numBlocks];
  int *numEventCodes = new int[numBlocks];
  int *numSweps = new int[numBlocks];

  int **eventCodeID = new int*[numBlocks];
  int **swepVals = new int*[numBlocks];
  double **swepTimes = new double*[numBlocks];
  int **swepID = new int*[numBlocks];

  int ***codeVal = new int**[numBlocks];

  t->print((ostream*)&logstream,
      new string("Init MYSQL, dynamically alloc'd arrays"));
  t->reset();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Iterate through block data, add conditions to database
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  int j,k;
  stringstream sql_buf, errorMsg;
  for (i=0;i<numBlocks;i++) {

    fp >> blockNumber[i] >> blockIDs[i] >> numEventCodes[i] >> numSweps[i];

    logstream << "Block-" << blockNumber[i] << " i: " << i << " numEventCodes: "
              << numEventCodes[i] << " numSweps: " << numSweps[i] << endl;

    // Grab the eventCodeID's and names next
    if (numEventCodes[i] == 0)
      eventCodeID[i] = NULL;
    else
      eventCodeID[i] = new int[numEventCodes[i]];

    for (j=0;j<numEventCodes[i];j++) 
      fp >> eventCodeID[i][j];

    if (numSweps[i] == 0) {
      swepVals[i] = NULL;
      swepTimes[i] = NULL;
      swepID[i] = NULL;
    } else {
      swepVals[i] = new int[numSweps[i]];
      swepTimes[i] = new double[numSweps[i]];
      swepID[i] = new int[numSweps[i]];
    }

    codeVal[i] = new int*[numEventCodes[i]];
    for (j=0;j<numEventCodes[i];j++)
      codeVal[i][j] = new int[numSweps[i]];

    // Now iterate over sweps, then over each eventCode, grabbing value
    for (j=0;j<numSweps[i];j++) {

      fp >> swepVals[i][j] >> swepTimes[i][j];

      if (swepVals[i][j] != -1) {

        sql_buf.str("");
        sql_buf << "INSERT INTO sweep_list (block_id,sweepNum) VALUES ("
                << blockIDs[i] << "," << swepVals[i][j] << ")";

        errorMsg.str("");
        errorMsg << "Error on swep (" << swepVals[i][j] << ") block (" 
                 << i << ") tankID (" << tankID << ") insert\n";

        try {
          swepID[i][j] = mysql->insertSingleItem(&(sql_buf.str()),&errorMsg);
        } catch (char* c) {
          ERROR_OUT("Error inserting swep data: " << c << endl);
        }

        for (k=0;k<numEventCodes[i];k++) {

          fp >> codeVal[i][k][j];

        } // end numEventCodes iteration

      } 

    } // end iteration over numSweeps

  } // end iteration over numBlocks    

  try {
    fp.close();
  } catch (...) {
    ERROR_OUT("Error closing file stream");
  }

  t->print((ostream*)&logstream,new string("Populated arrays from file"));
  t->reset();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Set up monster condition_list insert
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  sql_buf.str("");
  sql_buf << "INSERT INTO condition_list (sweep_list_id,param_id,param) VALUES ";

  int uploadCounts = 0;

  for (i=0;i<numBlocks;i++) {

    for (j=0;j<numSweps[i];j++) {

      if (swepVals[i][j] != -1) {

        for (k=0;k<numEventCodes[i];k++) {
 
          sql_buf << "(" << swepID[i][j] << "," << eventCodeID[i][k] << ","
                  << codeVal[i][k][j] << "),";
          uploadCounts++;

        } // end numEventCodes iteration

      }  // end if

    } // end numSweeps iteration

  } // end numBlocks

  string sql;
  sql = sql_buf.str();
  sql = sql.substr(0,sql.length()-1); // remove comma at end

  errorMsg.str("");
  errorMsg << "Error uploading condition list elements on tankID ("
           << tankID << ")\n";

  try {
    mysql->insertMultiItem(&sql,&errorMsg);
  } catch (char* c) {
    ERROR_OUT("Error inserting condition elements: " << c << endl);
  }

  t->print((ostream*)&logstream,new string("Finished uploading condition lists"));
  t->reset();

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

    logstream << "  - Starting on channel " << channels[i] 
              << " maxSort: " << maxSort << endl;

    if (maxSort > 2) {
      ERROR_OUT("Error on tankID " << tankID << " channel " << channels[i] << 
                " maxSort > 2 (value is " << maxSort << ")" << endl);
    }

    logstream << "    numSpikes: " << s[i]->getNumSpikes() << endl;

    s[i]->getSpikesPerSort(spikesPerSort);

    for (j=1;j<=maxSort;j++) {

      if (spikesPerSort[j] == 0) continue;

      logstream << "   Starting sort: " << j << " spikesPerSort: " 
                << spikesPerSort[j] << endl;

      sql_buf.str("");
      sql_buf << "INSERT INTO unit (tank_id,channel,sort,num_spikes) VALUES ("
              << tankID << "," << channels[i] 
              << "," << j << "," << spikesPerSort[j] << ")";

      errorMsg.str("");
      errorMsg << "Could not complete MYSQL unit insert ("
               << channels[i] << ":" << j << ")\n";
      try {
        unitID = mysql->insertSingleItem(&(sql_buf.str()),&errorMsg);
      } catch (char* c) {
        ERROR_OUT("Error writing units to dB: " << c << endl);
      }

      numSpikes = s[i]->getNumSpikes();
      spikeTimes = s[i]->getSpikeTimes();
      spikeClass = s[i]->getSpikeClasses();

      spikeListNumBlocks = s[i]->getNumBlocks();

      logstream << "   Channel " << channels[i] << " Sort " << j << " # Spikes "
                << spikesPerSort[j] << " Total Spikes " << numSpikes 
                << " unit ID " << unitID << endl;

      sI = 0;

      for (k=0;k<numBlocks;k++) {

        bOffset = s[i]->getBlockNextOffset(blockNumber[k]);

        sI = (blockNumber[k] == 0) ? 0 : s[i]->getBlockOffset(blockNumber[k]);

        // Increments spikeIndex to get past any spikes that jumped the stimulus gun
        while (spikeTimes[sI] <= swepTimes[k][0]) sI++;

        logstream << "   -  Block-" << blockNumber[k] << " k: " << k << " sI: "
                  << sI << " bOffset: " << bOffset << endl;

        // Reset the String Buffer for this unit insert
        sql_buf.str("");
        sql_buf << "INSERT INTO unit_data (unit_id,sweep_list_id,spike_count,psth) VALUES ";

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

            sql_buf << "(" << unitID << "," << swepID[k][m] << "," << spikeCount
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

          sql_buf << "(" << unitID << "," << swepID[k][m] << "," << spikeCount
                  << ",'" << psthBuf.str() << "'),";
          anyInserts = true;

        }

        // If we have anything in the buffer, we need to do the insert
        if (anyInserts) {
          sql = sql_buf.str();

          // Remove the comma from the end
          sql = sql.substr(0,sql.length()-1);

          errorMsg << "Could not complete MYSQL full unit insert ("
                   << unitID << ") SQL Length: " << sql.length() << endl;

          try {
            mysql->insertMultiItem(&sql,&errorMsg);
          } catch (char* c) {
            ERROR_OUT("Error writing full unit data: " << c << endl);
          }
        }

      } // end loop over blocks

    } // end loop over sorts

  } // end loop over channels

  t->print((ostream*)&logstream,new string("Finished uploading spike times to dB"));

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Cleanup arrays and objects
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  for (i=0;i<numBlocks;i++) {

    for (j=0;j<numEventCodes[i];j++) {
      if (codeVal[i][j])  delete [] codeVal[i][j];
    }

    if (eventCodeID[i]) delete [] eventCodeID[i];
    if (swepVals[i])    delete [] swepVals[i];
    if (swepTimes[i])   delete [] swepTimes[i];
    if (swepID[i])      delete [] swepID[i];

    if (codeVal[i])     delete [] codeVal[i];
  }

  if (blockNumber)   delete [] blockNumber;
  if (blockIDs)      delete [] blockIDs;
  if (numEventCodes) delete [] numEventCodes;
  if (numSweps)      delete [] numSweps;

  if (channels)     delete [] channels;
  if (channelNames) delete [] channelNames;

  if (eventCodeID)  delete [] eventCodeID;
  if (swepVals)     delete [] swepVals;
  if (swepTimes)    delete [] swepTimes;
  if (swepID)       delete [] swepID;
  if (codeVal)      delete [] codeVal;

  if (ps) delete ps;
  for (i=0;i<numChannels;i++) {
    if (s[i]) delete s[i];
  }
  if (s) delete [] s;

  logstream.flush();
  logstream.close();
  if (mysql) delete mysql;

  exit(0);
}