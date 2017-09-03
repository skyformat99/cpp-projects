#include "ToolboxData.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// CONSTRUCTORS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

/**
 * Initializes an unloaded, empty ToolboxData instance.
 */
ToolboxData::ToolboxData ()
{
  _isLoaded = false;
}

/**
 * Initializes a ToolboxData instance by loading from the 
 * given text file.
 */
ToolboxData::ToolboxData (string s)
{
  inFileName = s;
  _isLoaded = loadFromTextFile ();
}

/**
 * Deconstructs the ToolboxData element.  Deletes the
 * pointers within the freq_ear_t structure to the trial_t
 * structures, then deletes all of the freq_ear_t pointers.
 */
ToolboxData::~ToolboxData ()
{
  _isLoaded = false;

  // Delete trials in each freq_ear, then delete freq_ear elements
  freq_ear_t *fe;
  trial_t *t;

  while (!freq_ears.empty()) {
    fe = freq_ears.back();

    while (!fe->trials.empty()) {
      t = fe->trials.back();
      fe->trials.pop_back();
      delete t;
    }

    freq_ears.pop_back();
    delete fe;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

/** 
 * Returns true when the ToolboxData is loaded,
 * false otherwise.
 */
bool ToolboxData::isLoaded ()
{
  return _isLoaded;
}

/**
 * Prints the data stored in a loaded ToolboxData instance
 * to the console.
 */
void ToolboxData::printData ()
{
  if (!_isLoaded) {
    cout << "ToolboxData Not Loaded" << endl;
    return;
  }

  cout << "DataFileName: " << inFileName << endl;
  cout << "Subject ID: " << subjID << endl;
  cout << "Pin: " << pin << endl;

  for (int i = 0; i < freq_ears.size(); i++) {
    cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    printFreqEarData(freq_ears[i]);
  }
}

/**
 * Opens a CSV file at the given file location and writes
 * the data in the format specified by JW, 9-Feb-2012 email.
 */
void ToolboxData::writeToCSV (string* s)
{
  outFileName = (*s);

  fstream* out;
  try {
    out = new fstream(outFileName,ios_base::out);
  } catch (...) {
    cerr << "Error loading outstream csv file!" << endl;
  }

  if (!out->is_open()) {
    cerr << "Error opening outstream csv file!" << endl;
  }

  // Write header first
  (*out) << "DataFileName:," << inFileName << endl;
  (*out) << "Subject ID:," << subjID << endl;
  (*out) << "Test Date:," << endl;
  (*out) << "PIN:," << pin << endl;
  (*out) << endl;

  // Now write each freq/ear, write out data
  for (int i = 0; i < freq_ears.size(); i++) 
    writeFreqEarToCSV (out, freq_ears[i]);

  // Close the input file & delete
  try {
    out->close();
  } catch (...) {
    cerr << "Error closing outstream csv file" << endl;
  }
 
  delete out;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

/**
 * Gathers the next valid, non-empty line from the file stream
 * and stores it in the passed *str. Returns true if it finds a
 * string, or false if EOF is hit.
 */
bool ToolboxData::getNextLine (fstream* f, string* str)
{
  while (!f->eof()) {

    getline(*f,*str);

    if (str->empty()) {
      continue;
    }

    if (str->length() == 0) {
      continue;
    }

    if ((*str) == "") {
      continue;
    }

    return true;
  }

  return false;
}

/**
 * Reads the full ToolboxData in from the text file
 * identified by the inFileName private field. If it
 * loads appropriately it returns true, otherwise false.
 */
bool ToolboxData::loadFromTextFile ()
{
  fstream* infile;
  try {
    infile = new fstream(inFileName,ios_base::in);
  } catch (...) {
    cerr << "Error loading instream text file!" << endl;
    return false;
  }

  if (!infile->is_open()) {
    cerr << "Error opening instream text file!" << endl;
    return false;
  }

  // Process file header
  (*infile) >> subjID;
  (*infile) >> pin;

  // Loop until we've loaded all freq-ear combinations
  bool isValid;
  while (!infile->eof()) {

    freq_ear_t* fe = new freq_ear_t;

    isValid = readFreqEar(infile,fe);

    if (isValid) {
      freq_ears.push_back(fe);
      continue;
    } else {
      delete fe;
      break;
    }
  }

  // Close the input file & delete
  try {
    infile->close();
  } catch (...) {
    cerr << "Error closing instream text file" << endl;
  }
 
  delete infile;

  return true;
}

/**
 * Prints data from a freq_ear_t structure, including a summary of
 * all enclosed trial_t structures, to the console window.
 */
void ToolboxData::printFreqEarData (freq_ear_t* fe)
{
  cout << "Frequency: " << fe->freq << endl;
  cout << "Ear: " << fe->ear << endl;
  cout << "Threshold: " << fe->threshold << endl;
  cout << "Catch Trial Data: " << fe->catch_trial << endl;
  cout << "QA Final: " << fe->qa_final << endl;
  cout << "QA # Catch Trials: " << fe->qa_num << endl;
  cout << "# False Alarms: " << fe->fa_num << endl;
  cout << "False Alarm Rate: " << fe->fa_rate << endl;
  cout << "# Catch Trial Flag: " << fe->num_catch << endl;

  cout << "# Trials: " << fe->trials.size() << endl;
  for (int i = 0; i < fe->trials.size(); i++) {
    cout << "(" << fe->trials[i]->num << ") " 
         << fe->trials[i]->level << " dB, "
         << fe->trials[i]->volt << " v, lat="
         << fe->trials[i]->latency << ", dir="
         << fe->trials[i]->dir << ", rev="
         << fe->trials[i]->rev << ", step="
         << fe->trials[i]->stepsize << ", result="
         << fe->trials[i]->result << endl;
  }
}

/**
 * Reads the data associated with a given freq-ear combination from
 * the text file.  Returns true when the data read is successful, 
 * false otherwise.
 */
bool ToolboxData::readFreqEar (fstream* f, freq_ear_t* fe)
{
  if (f->eof())
    return false;

  bool isRead = true;
  string nextL;
  stringstream* ss;

  isRead &= readLineData( f, &(fe->freq), &(fe->ear) );
  isRead &= readLineData( f, &(fe->threshold) );
  isRead &= readTrialsLine( f ,fe);
  isRead &= readLineData( f, &(fe->catch_trial) );
  isRead &= readLineData( f, &(fe->qa_final) );
  isRead &= readLineData( f, &(fe->qa_num) );
  isRead &= readLineData( f, &(fe->fa_num) );
  isRead &= readLineData( f, &(fe->fa_rate) );
  isRead &= readLineData( f, &(fe->num_catch) );

  return isRead;
}

/**
 * Reads two (possibly unique typed) items from a line of data.
 */
template <class T, class U>
bool ToolboxData::readLineData (fstream* f, T* s1, U* s2)
{
  string nextL;
  stringstream* ss;
  bool isRead = getNextLine(f,&nextL);
  if (!isRead) return false;

  ss = new stringstream(nextL);
  (*ss) >> (*s1) >> (*s2);
  delete ss;

  return true;
}

/**
 * Reads a single item from a line of data.
 */
template <class T>
bool ToolboxData::readLineData (fstream* f, T* s)
{
  string nextL;
  stringstream* ss;
  bool isRead = getNextLine(f,&nextL);
  if (!isRead) return false;

  ss = new stringstream(nextL);
  (*ss) >> (*s);
  delete ss;

  return true;
}

/**
 * Processes the Trials line from the text file and populates
 * the trial_t vector in the provided freq_ear_t.
 */
bool ToolboxData::readTrialsLine (fstream* f, freq_ear_t* fe)
{
  string* str = new string;
  bool isRead = getNextLine(f,str);
  if (!isRead) return false;

  bool isValid;
  int index = 0;
  size_t pos = (size_t)0;
  size_t found, colon, space;
  stringstream* ss;
  while ( (found = str->find("|",pos)) != string::npos) {

    pos = found + 1;
    index++;

    trial_t* trial = new trial_t;
    trial->num = index;

    // Level
    isValid = readTrialsSegment (str,found+1,&colon,&space,true, &(trial->level) );
    if (!isValid) continue;

    // Voltage
    isValid = readTrialsSegment (str,space+1,&colon,&space,true, &(trial->volt) );
    if (!isValid) continue;

    // Result
    isValid = readTrialsSegment (str,space+1,&colon,&space,true, &(trial->result) );
    if (!isValid) continue;

    // RespTime
    isValid = readTrialsSegment (str,space+1,&colon,&space,true, &(trial->latency) );
    if (!isValid) continue;

    // Direction
    isValid = readTrialsSegment (str,space+1,&colon,&space,true, &(trial->dir) );
    if (!isValid) continue;

    // RevCount
    isValid = readTrialsSegment (str,space+1,&colon,&space,true, &(trial->rev) );
    if (!isValid) continue;

    // Stepsize
    isValid = readTrialsSegment (str,space+1,&colon,&space,false, &(trial->stepsize) );
    if (!isValid) continue;

    fe->trials.push_back(trial);
  }

  return true;
}

/**
 * Template class to read a single segment of data from the trials line.
 */
template <class T>
bool ToolboxData::readTrialsSegment (string* str, size_t startPos,
  size_t* colon, size_t* space, bool catchSpace, T* val)
{
  (*colon) = str->find(":",startPos);
  if ( (*colon) == string::npos) {
    cerr << "Error: found '|' but no ':'!" << endl;
    return false;
  }

  (*space) = str->find(" ",(*colon)+1);
  if ( ((*space) == string::npos) && catchSpace) {
    cerr << "Error: found ':' but no ' '!" << endl;
    return false;
  }

  stringstream* ss;

  if (catchSpace)
     ss = new stringstream(str->substr( (*colon)+1, (*space)-(*colon)-1));
  else
     ss = new stringstream(str->substr( (*colon)+1 ));

  (*ss) >> (*val);
  delete ss;

  return true;
}

/**
 * Write an individual freq-ear element to the CSV file stream.
 */
void ToolboxData::writeFreqEarToCSV (fstream* out, freq_ear_t* fe)
{
  int numTrials = fe->trials.size();

  (*out) << "Frequency," << fe->freq << ",,Trial #,Level,Voltage,Latency,Direction,Reversal,Stepsize,Result" << endl;

  (*out) << "Ear," << fe->ear << ",,";
  if (numTrials >= 1) writeTrialToCSV(out,fe->trials[0]);
  (*out) << endl;

  (*out) << "Threshold," << fe->threshold << ",,";
  if (numTrials >= 2) writeTrialToCSV(out,fe->trials[1]);
  (*out) << endl;

  (*out) << "Catch Trial Data," << fe->catch_trial << ",,";
  if (numTrials >= 3) writeTrialToCSV(out,fe->trials[2]);
  (*out) << endl;

  (*out) << "QA Final," << fe->qa_final << ",,";
  if (numTrials >= 4) writeTrialToCSV(out,fe->trials[3]);
  (*out) << endl;

  (*out) << "QA # Catch Trials," << fe->qa_num << ",,";
  if (numTrials >= 5) writeTrialToCSV(out,fe->trials[4]);
  (*out) << endl;

  (*out) << "# False Alarms," << fe->fa_num << ",,";
  if (numTrials >= 6) writeTrialToCSV(out,fe->trials[5]);
  (*out) << endl;

  (*out) << "False Alarm Rate," << fe->fa_rate << ",,";
  if (numTrials >= 7) writeTrialToCSV(out,fe->trials[6]);
  (*out) << endl;

  (*out) << "# Catch Trial Flag," << fe->num_catch << ",,";
  if (numTrials >= 8) writeTrialToCSV(out,fe->trials[7]);
  (*out) << endl;

  for (int i = 8; i < numTrials; i++) {
    (*out) << ",,,";
    writeTrialToCSV(out,fe->trials[i]);
    (*out) << endl;
  }

  (*out) << endl;
}

/**
 * Write an individual trial element to the CSV file stream.
 */
void ToolboxData::writeTrialToCSV (fstream* out, trial_t* t)
{
  (*out) << t->num << "," 
         << t->level << "," 
         << t->volt << ","
         << t->latency << "," 
         << t->dir << ","
         << t->rev << ","
         << t->stepsize << ","
         << t->result;
}

