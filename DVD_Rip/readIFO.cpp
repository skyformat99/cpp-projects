#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "VideoUnit.h"

#include "stdio.h"
#include "io.h"

using namespace std;

/** Helper functions in this C++ file **/
long fileSize (FILE* fp);
int readBCD (long offset, FILE* fp, int* vals);
int readByte (long offset, FILE* fp);
int readShort (long offset, FILE* fp);
int readInt (long offset, FILE* fp);
bool findDriveWithDVD (string* s);
void parseIFOFile (string* drive, int* numTitles, vector<VideoUnit>* units);
void Tokenize (string& str, vector<int>& tokens, const string& delimiters  = ",");

/** Helper function in ripDVD.cpp file **/
void ripDVD (int vts, int pgc, int chapter); // Need to link w/ ripDVD.obj

// Bytes per sector
#define BPS 2048

// Minimum duration to worry about, in minutes
#define MIN_DURATION 5.0

/**
 * Notes:
 * 
 * Some good information on IFO structure can be found here:
 *   http://dvd.sourceforge.net/dvdinfo/
 * and here:
 *   http://www.dvd-replica.com/DVD/data-2.php
 *
 * Look in the Backup directory for an example C# program
 * that reads the IFO structure, as well as a GUI program (ifoEdit) 
 * which is useful for debugging the IFO structure.
 */

/**
 * Main function executed in this file. First we find
 * the drive with a DVD present (searching in alpha order
 * from C:\ -> Z:\). Then we parse the IFO file to get the
 * number of titles and chapters. Then we rip each title/chapter
 * into an output file.
 */
int main (int argc, char* argv[]) 
{
  /** 
   * If the user passes two arguments, assume they are the VTS and PGC
   * and go straight to ripping.
   */
  if (argc >= 3) {
    int VTS, PGC, CHAP;

    try {
      VTS = atoi(argv[1]);
      PGC = atoi(argv[2]);
    } catch (...) {
      throw "Error converting arguments to integers ... quitting";
    }

    if (argc >= 4) {

      try {
        CHAP = atoi(argv[3]);
      } catch (...) {
        throw "Error converting arguments to integers ... quitting";
      }

    } else {
      CHAP = -1;
    }

    cout << "Ripping VTS: " << VTS 
         << " PGC: " << PGC 
         << " CHAP: " << CHAP << endl;

    try {
      ripDVD(VTS,PGC,CHAP); 
    } catch (...) {
      cerr << "Error ripping VTS and PGC specified ... quitting ...";
    }

    exit(0);
  }

  string drive;
  if (!findDriveWithDVD(&drive)) {
    cerr << "Error: no drive has a DVD in it!" << endl;
    exit(1);
  }

#ifdef VERBOSE    
  cout << "Drive with DVD: " << drive << endl;
#endif

  string titleStr;
  int i,j,specifyTitle,numTitles;
  vector<VideoUnit> videoUnits;

  /** Try to parse the IFO file, but quit if we get an exception! **/
  try {
    parseIFOFile(&drive,&numTitles,&videoUnits);
  } catch (char* ex) {
    cerr << ex << endl;
    exit(-1);
  } catch (...) {
    cerr << "Unknown exception in parseIFOFile(...): Quitting" << endl;
    exit(-1);
  }

  /** Choose title to rip **/
  cout << "Please choose titles to be ripped (separate with commma):" 
       << endl;
  cout << "(Total number of titles: " << numTitles << ")" << endl;
  for (i=0;i<numTitles;i++) 
    cout << "  (" << i << ") " << videoUnits[i].toString() << endl;

  cin >> titleStr;

  /** If input str is -1, use manual entry **/
  if (titleStr == "-1") {
    int vts, pgc;

    cout << "Enter VTS:" << endl;
    cin >> vts;

    cout << "Enter PGC:" << endl;
    cin >> pgc;

    try {
      ripDVD(vts,pgc,-1);
    } catch (...) {
      cerr << "Error: could not rip vts " << vts << " pgc " << pgc << endl;
    }

    exit(0);
  }

  vector<int> titles;

  Tokenize(titleStr,titles);

  int titleInd;
  for (i=0;i<titles.size();i++) {
    titleInd = titles[i];

    try {
      ripDVD(videoUnits[titleInd].getVTS(),videoUnits[titleInd].getPGC(),-1);
    } catch (...) {
      cerr << "Error: could not rip " << videoUnits[i].toString()
           << endl;
    }
  }

  exit(0);
}

void Tokenize (string& str, vector<int>& tokens,
  const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(atoi(str.substr(lastPos, pos - lastPos).c_str()));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


/**
 * Searches each drive (beginning with 'C' and going through
 * 'Z') for the file \\VIDEO_TS\\VIDEO_TS.IFO. If this file is
 * found, this drive is assumed to have a DVD present. Returns true
 * if a file is found (and populates the argument with that drive),
 * or false otherwise.
 */
bool findDriveWithDVD (string* s) 
{
  long iResult;
  struct _finddata_t fileinfo;

  stringstream ss;
  ss.str("");

#ifdef LDRIVE
  char drive = 'L';
#else
  char drive = 'C';
#endif
  bool found = false;

  while (drive <= 'Z' && !found) {

#ifdef DEBUG
  cerr << "Checking drive " << drive << ": for DVD files ..." << endl;
#endif

    ss.str("");
    ss << drive << ":\\VIDEO_TS\\VIDEO_TS.IFO";

    iResult = _findfirst(ss.str().c_str(),&fileinfo);

    if (iResult == -1L) {
      drive++;
      continue;
    }

#ifdef DEBUG
  cerr << "Found DVD files! Drive " << drive << ":" << endl;
#endif

    found = true;
  }

  ss.str("");
  ss << drive << ":\\";

  *s = ss.str();

  return found;
}

/**
 * Parse the IFO file for the number of titles, and the number of 
 * chapters present. Return the number of titles in the passed int,
 * and populate the numChapters to size=numTitles with the number
 * of chapters in each title.
 */
void parseIFOFile (string* drive, int* numTitles, vector<VideoUnit>* units)
{
  FILE* fp;
  stringstream ss;
  ss.str("");

  ss << *drive << "VIDEO_TS\\VIDEO_TS.IFO";

  // First open the main file, VIDEO_TS.IFO, to get
  // the VTS and Chapter information
  fp = fopen(ss.str().c_str(),"r");
#ifdef VERBOSE
  cout << "File size: " << fileSize(fp) << endl;
#endif

  int numVTS = readShort(0x003E,fp);
#ifdef VERBOSE
  cout << "numVTS: " << numVTS << endl;
#endif

  // sector pointer to TT_SRPT (Addr = BytesPerSecter * Secter Ptr)
  int TT_SRPT_sp = readInt(0x00C4,fp); 
#ifdef VERBOSE
  cout << "TT_SRPT sector pointer: " << TT_SRPT_sp << endl;
#endif

  try {
    *numTitles = readShort(BPS*TT_SRPT_sp,fp);
  } catch (char* ex) {
    throw ex;
  } catch (...) {
    throw "Unknown exception reading numTitles short ...";
  }
    
#ifdef VERBOSE
  cout << "numTitles: " << *numTitles << endl;
#endif

  int i;
/**
  int nC,vtsN,vtsTTN;
  for (i=0;i<(*numTitles);i++) {
    nC = readShort(BPS*TT_SRPT_sp+8+(12*i)+2,fp);
//    numChapters->push_back(nC);

    vtsN = readByte(BPS*TT_SRPT_sp+8+(12*i)+6,fp);
//    vtsNumber->push_back(vtsN);

    vtsTTN = readByte(BPS*TT_SRPT_sp+8+(12*i)+7,fp);
**/

#ifdef VERBOSE
//    cout << "  Title " << (i+1) << " Chapters: " << nC 
//         << " VTS #: " << vtsN << " VTS_TTN: " << vtsTTN << endl;
#endif

//  }

  fclose(fp);

  VideoUnit* unit;
  int countTitles = 0;
  int* bcdTime = new int[8];
  int VTS_PGCI_sp, numPGC, fr, j, k, pgcOffset, numPrograms;
  stringstream dur;
  double durMin;

  int VTS_PTT_SRPT_sp, lastVTS_PTT_offset;
  int numVTSTitles;

  // Now we open each individual VTS_xx_0.IFO file to get the 
  // PGC information
  for (i=0;i<numVTS;i++) {
    ss.str("");
    if (i+1 <= 9)
      ss << *drive << "VIDEO_TS\\VTS_0" << i+1 << "_0.IFO";
    else
      ss << *drive << "VIDEO_TS\\VTS_" << i+1 << "_0.IFO";

#ifdef VERBOSE
    cout << "File to open: " << ss.str() << endl;
#endif

    fp = fopen(ss.str().c_str(),"r");

/**
    VTS_PTT_SRPT_sp = readInt(0x00C8,fp);
    numVTSTitles = readShort(BPS*VTS_PTT_SRPT_sp,fp);
    lastVTS_PTT_offset = readInt(BPS*VTS_PTT_SRPT_sp+4,fp);
**/

#ifdef VERBOSE
//    cout << "  VTS (" << i << ") VTS_PTT_SRPT_sp: " << VTS_PTT_SRPT_sp << endl;
//    cout << "  VTS (" << i << ") numVTSTitles: " << numVTSTitles  << endl;
//    cout << "  VTS (" << i << ") lastVTS_PTT_offset: " << lastVTS_PTT_offset << endl;
#endif

    VTS_PGCI_sp = readInt(0x00CC,fp);
    numPGC = readShort(BPS*VTS_PGCI_sp,fp);

#ifdef VERBOSE
    cout << "  VTS (" << i << ") VTS_PGCI_sp: " << VTS_PGCI_sp << endl;
    cout << "  VTS (" << i << ") numPGC: " << numPGC << endl;
#endif

    for (j=0;j<numPGC;j++) {

      try {
        pgcOffset = readInt(BPS*VTS_PGCI_sp+8+(8*j)+4,fp);

#ifdef VERBOSE
        cout << "    pgcOffset: " << pgcOffset << endl;
#endif

        numPrograms = readByte(BPS*VTS_PGCI_sp+pgcOffset+2,fp);

        fr = readBCD(BPS*VTS_PGCI_sp+pgcOffset+4,fp,bcdTime);
      } catch (...) {
        pgcOffset = -1;
        numPrograms = -1;
        for (k=0;k<8;k++) bcdTime[k] = 0;
      }

      dur.str("");
      dur << bcdTime[0] << bcdTime[1] << ":" 
          << bcdTime[2] << bcdTime[3] << ":" 
          << bcdTime[4] << bcdTime[5] << ":"
          << bcdTime[6] << bcdTime[7] 
          << " (" << fr << " fps)";

//      pgcNumber->push_back(j+1);
//      pgcDuration->push_back(dur.str());
      countTitles++;

      // Minutes (ignoring the # frames)
      durMin = (double)bcdTime[0]*600. +
               (double)bcdTime[1]*60. +
               (double)bcdTime[2]*10. +
               (double)bcdTime[3] +
               (double)bcdTime[4]*10/60 +
               (double)bcdTime[5]/60;

      unit = new VideoUnit(i+1,numPrograms,j+1,dur.str(),durMin);
      units->push_back(*unit);

#ifdef VERBOSE
      cout << "VTS: " << i+1 << " PGC: " << j+1 
           << " pgcAddr: 0x" << hex << (BPS*VTS_PGCI_sp+8+(8*j)+4) << dec
           << " pgcOffset: " << pgcOffset 
           << " # programs: " << numPrograms
           << " dur: " << bcdTime[0] << bcdTime[1] << ":" 
           << bcdTime[2] << bcdTime[3] << ":" 
           << bcdTime[4] << bcdTime[5] << ":"
           << bcdTime[6] << bcdTime[7] 
           << " (" << fr << " fps)" << endl; 
#endif
    
    }

    fclose(fp);
  }

  if (countTitles != *numTitles)
    cerr << "Warning! numTitles != count of PCGs across VTS files!" << endl;

  delete[] bcdTime;

  return;
}

/**
 * Determine the file size by seeking to the end and 
 * getting the location of the seek pointer.
 */
long fileSize (FILE* fp) {
  long lSize;

  fseek (fp,0,SEEK_END);
  lSize = ftell(fp);
  rewind(fp);

  return lSize;
}

/**
 * Read a 1-byte (byte) number into an integer, at the given
 * offset in the file specified by the file pointer.
 */
int readByte (long offset, FILE* fp) {

  if (fseek(fp,offset,SEEK_SET) != 0) {
    cerr << "Error seeking offset " << offset << endl;
    exit(1);
  }

  size_t result;
  unsigned char* c = new unsigned char[1];
  int i;
  if (fread(c,1,1,fp) != 1) {
    cerr << "Error reading byte at offset " << offset 
         << " (0x" << hex << offset << dec << ")";
    if (feof(fp))
      cerr << "  ... EOF reached!" << endl;
    if (ferror(fp))
      cerr << "  ... error returned!" << endl;
    throw "Error in readByte(...)";
  }

  i = c[0];
  delete[] c;

  return i;
}

/**
 * Read a 2-byte (short) number into an integer, at the given
 * offset in the file specified by the file pointer.
 */
int readShort (long offset, FILE* fp) {

  fpos_t pos;

  if (fseek(fp,offset,SEEK_SET) != 0) {
    cerr << "Error seeking offset " << offset << endl;
    exit(1);
  }

#ifdef VERBOSE
  cout << "Position: " << ftell(fp) << endl;
#endif

  unsigned char* c = new unsigned char[2];
  int i;
  if (fread(c,1,2,fp) != (size_t)2) {
    cerr << "Error reading short at offset " << offset 
         << " (0x" << hex << offset << dec << ")";
    if (ferror(fp))
      cerr << "  ... error returned!" << endl;
    if (feof(fp))
      cerr << "  ... EOF reached!" << endl;
    throw "Error in readShort(...)";
  }

  i = c[1] | (c[0] << 8);
  delete[] c;

  return i;
}

/**
 * Read a 4-byte (int) number into an integer, at the given
 * offset in the file specified by the file pointer.
 */
int readInt (long offset, FILE* fp) {

  if (fseek(fp,offset,SEEK_SET) != 0) {
    cerr << "Error seeking offset " << offset << endl;
    exit(1);
  }

  size_t result;
  unsigned char* c = new unsigned char[4];
  int i;
  if (fread(c,1,4,fp) != 4) {
    cerr << "Error reading int at offset " << offset 
         << " (0x" << hex << offset << dec << ")";
    if (feof(fp))
      cerr << "  ... EOF reached!" << endl;
    if (ferror(fp))
      cerr << "  ... error returned!" << endl;
    throw "Error";
  }

  i = c[3] | (c[2] << 8) | (c[1] << 16) | (c[0] << 24);
  delete[] c;

  return i;
}

int readBCD (long offset, FILE* fp, int* vals) {

  if (fseek(fp,offset,SEEK_SET) != 0) {
    cerr << "Error seeking offset " << offset << endl;
    exit(1);
  }

  int numBytes = 4;

  size_t result;
  unsigned char* c = new unsigned char[numBytes];
  int i;
  if (fread(c,1,numBytes,fp) != numBytes) {
    cerr << "Error reading int at offset " << offset 
         << " (0x" << hex << offset << dec << ")";
    if (feof(fp))
      cerr << "  ... EOF reached!" << endl;
    if (ferror(fp))
      cerr << "  ... error returned!" << endl;
    throw "Error";
  }

  for (i=0;i<numBytes-1;i++) {
    vals[2*i] = ((int)c[i] & (int)0x00f0) >> 4;
    vals[2*i+1] = (int)c[i] & (int)0x000f;
  }

  vals[6] = ((int)c[3] & (int)0x0030) >> 4;
  vals[7] = (int)c[3] & (int)0x000f;

  int fr_code = ((int)c[3] & (int)0x00C0) >> 6;
  delete[] c;

  if (fr_code == 3)
    return 30;
  else if (fr_code == 1)
    return 25;
  else
    return -1;
}
