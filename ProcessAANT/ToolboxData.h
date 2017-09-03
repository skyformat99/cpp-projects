#ifndef __TOOLBOX_DATA_H__
#define __TOOLBOX_DATA_H__

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

/** Trial-by-trial data */
struct trial_t 
{
  int num;
  int level;
  double volt;
  int latency;
  string dir;
  int rev;
  int stepsize;
  int result;
};

/** Data structure holding information for a single freq-ear combo **/
struct freq_ear_t 
{
  string freq;
  string ear;
  int threshold;
  string catch_trial;
  int qa_final;
  int qa_num;
  int fa_num;
  string fa_rate;
  int num_catch;
  vector<trial_t*> trials;
};



/**
 * Class holding all of the data associated with a subject's AANT Toolbox run.
 * Includes built-in methods to read the data from a text file, and to 
 * write out to a .csv file for Excel importing.
 *
 * ACD, 09-Feb-2012
 *
 * :CHANGELOG:
 *
 * 09-Feb-2012 - Created initial file and integrated into processAANT.cpp.
 *
 */
class ToolboxData 
{
private:
  bool _isLoaded;
  string inFileName;
  string outFileName;

  string subjID;
  string pin;

  vector<freq_ear_t*> freq_ears;

  bool getNextLine (fstream* f, string* str); 
  bool loadFromTextFile ();
  void printFreqEarData (freq_ear_t* fe);

  bool readFreqEar (fstream* f, freq_ear_t* fe);

  template <class T, class U>
  bool readLineData (fstream* f, T* s1, U* s2);

  template <class T>
  bool readLineData (fstream* f, T* s);

  bool readTrialsLine (fstream* f, freq_ear_t* fe);

  template <class T>
  bool readTrialsSegment (string* str, size_t startPos,
    size_t* colon, size_t* space, bool catchSpace, T* val);

  void writeFreqEarToCSV (fstream* out, freq_ear_t* fe);
  void writeTrialToCSV (fstream* out, trial_t* t);

public:
  ToolboxData ();
  ToolboxData (string s);

  ~ToolboxData ();

  bool isLoaded ();
  void printData ();

  void writeToCSV (string* s);
};

#endif