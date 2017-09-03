#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <io.h>

using namespace std;

void ripDVD (int vts, int pgc, int chapter);

/** Initializes the error-writing code **/
#define INIT_ERR \
  stringstream err_ss; \
  err_ss.str("");

/** Throws an error **/
#define THROW_ERR(a) \
  err_ss.str(""); \
  err_ss << a << endl; \
  cerr << err_ss; \
  throw err_ss.str();

/**
 * Main function executed in this file.
 * 
 * @deprecated This is not used anymore now that we have a
 *             function to read the IFO files.
 */
/**
int main (int argc, char* argv[]) {
  
  if (argc < 4) {
    cerr << "Usage: ripDVD <VTS> <PGC> <CHAPTER>" << endl;
    exit(-1);
  }

  int vts, pgc, chapter;
 
  try {
    vts = atoi(argv[1]);
    pgc = atoi(argv[2]);
    chapter = atoi(argv[3]);
  } catch (...) {
    cerr << "Error: see usage!" << endl;
    exit(-1);
  }

#ifdef VERBOSE
  cout << "VTS: " << vts << " PGC: " << pgc << " Chapter: " << chapter << endl;
#endif

  try {
    ripDVD(vts,pgc,chapter);
  } catch (...) {
    cerr << "Error: could not complete" << endl;
    exit(-1);
  }

  exit(0);
}
**/

/** Main function to handle the ripping of the DVDs **/
void ripDVD (int vts, int pgc, int chapter) {

  INIT_ERR

  // Define some constants
  string DVDDECRYPT_PATH = "\"C:/Program Files (x86)/DVD Decrypter/DVDDecrypter.exe\"";
  string BESWEET_PATH = "\"C:/Program Files (x86)/BeSweetv1.5b31/Besweet.exe\"";
  string MPLEX_PATH = "\"C:/Program Files (x86)/BeSweetv1.5b31/mplex.exe\"";
  string ROOT_DIR = "C:\\Temp\\";
  
  // Clean up current files
  stringstream ss;

  ss.str(""); ss << "del " << ROOT_DIR << "*.ifo"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "*.ac3"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "*.txt"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "*.m2v"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "besweet.log"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "*.mp2"; system(ss.str().c_str());

#ifdef VERBOSE
  cout << "Deleted current files" << endl;
#endif

  // First use DVD Decrypter to get the files
  ss.str("");

  if (chapter == -1) {
 
    ss << "\"" << DVDDECRYPT_PATH 
#ifdef LDRIVE
       << " /mode ifo /src L:\\ /vts " << vts << " /pgc " << pgc 
#else
       << " /mode ifo /src D:\\ /vts " << vts << " /pgc " << pgc 
#endif
       << " /demux 0xe0 0x80 0xa0 0x80 /dest \"\"" << ROOT_DIR << "\"\""
       << " /split none /start /overwrite yes /close\"";

  } else {

    ss << "\"" << DVDDECRYPT_PATH 
#ifdef LDRIVE
       << " /mode ifo /src L:\\ /vts " << vts << " /pgc " << pgc 
#else
       << " /mode ifo /src D:\\ /vts " << vts << " /pgc " << pgc 
#endif
       << " /demux 0xe0 0x80 0xa0 0x80 /dest \"\"" << ROOT_DIR << "\"\""
       << " /split none /start /overwrite yes /chapters " << chapter
       << " /close\"";
  }

  system(ss.str().c_str());

#ifdef VERBOSE
  cout << "Executing DVD Decrypter" << endl;
#endif

  // Now read the stream info file and get the delay
  ss.str("");
  long iResult;
  struct _finddata_t fileinfo;

  ss << ROOT_DIR << "VTS_* - Stream Information.txt";

  iResult = _findfirst(ss.str().c_str(),&fileinfo);

  if (iResult == -1L) {
    THROW_ERR("Error finding Stream Information file!");
  }

  ss.str("");
  ss << ROOT_DIR << fileinfo.name;

#ifdef VERBOSE
  cout << "Found Stream Information: " << fileinfo.name << endl;
#endif

  fstream infile;

  try {
    infile.open(ss.str().c_str(),fstream::in);
  } catch (...) {
    THROW_ERR("Error opening \"" << ss.str() << "\" using fstream::in");
  }

  bool found = false;
  string s;
  while (getline(infile,s)) {
    if (s.find("0x80 - Audio") != string::npos) {
      found = true;
      break;
    }
  }

  infile.close();

  if (!found) {
    THROW_ERR("Could not find 0x80-Audio string in txt file!");
  }

  int delayPos = s.find("Delay: ");
  if (delayPos == string::npos) {
    cerr << "-1" << endl; 
    exit(1);
  }

  int msPos = s.find("ms",delayPos);
  if (msPos == string::npos) {
    THROW_ERR("Could not find delay position 'ms' string in txt file");
  }

  int delayMS;
  try {
    delayMS = atoi(s.substr(delayPos+7,s.length()-msPos+1).c_str());
  } catch (...) {
    THROW_ERR("Error finding delay!");
  }

#ifdef VERBOSE
  cout << "Found delay: " << delayMS << " ms" << endl;
#endif

  // Rename the audio stream to audio.ac3
  ss.str("");
  ss << ROOT_DIR << "VTS_* - 0x80 - Audio*";

  iResult = _findfirst(ss.str().c_str(),&fileinfo);

  if (iResult == -1L) {
    THROW_ERR("Error finding audio file!");
  }

  ss.str("");
  ss << "move \"" << ROOT_DIR << fileinfo.name 
     << "\" \"" << ROOT_DIR << "audio.ac3\"";

  system(ss.str().c_str());

#ifdef VERBOSE
  cout << "Renamed audio stream to audio.ac3" << endl;
#endif

  // Running audio delay processor
  ss.str("");
  ss << "\"" << BESWEET_PATH
     << " -core( -input \"" << ROOT_DIR << "audio.ac3\" -output \"" 
     << ROOT_DIR << "audio.mp2\" -logfilea \"" << ROOT_DIR 
     << "besweet.log\" ) -ota( -d " << delayMS << " ) " 
     << "-azid( -c normal -g 0.95 -l -3db ) -toolame( -m s -b 192 -e )\"";

  system(ss.str().c_str());

#ifdef VERBOSE
  cout << "Finished BeSweet Audio processing" << endl;
#endif

  // Rename the video stream to video.m2v
  ss.str("");
  ss << ROOT_DIR << "VTS_* - 0xE0 - Video*";

  iResult = _findfirst(ss.str().c_str(),&fileinfo);

  if (iResult == -1L) {
    THROW_ERR("Error finding video file!");
  }

  ss.str("");
  ss << "move \"" << ROOT_DIR << fileinfo.name 
     << "\" \"" << ROOT_DIR << "video.m2v\"";

  system(ss.str().c_str());

#ifdef VERBOSE
  cout << "Renamed video stream to video.m2v" << endl;
#endif

  // Specify output file
  ss.str("");
  ss << ROOT_DIR << "Output-VTS_" << vts << "-PGC_" << pgc;

  if (chapter == -1)
    ss << "-CHAP_%d.mpg";
  else
    ss << "-CHAP_" << chapter << ".mpg";

  string outputFile = ss.str();

#ifdef VERBOSE
  cout << "Output File: " << outputFile << endl;
#endif

  // Running mplex'ing
  ss.str("");
  ss << "\"" << MPLEX_PATH
     << " -f 8 -o \"" << outputFile << "\" \"" << ROOT_DIR 
     << "audio.mp2\" \"" << ROOT_DIR << "video.m2v\"\"";

  system(ss.str().c_str());

#ifdef VERBOSE
  cout << "Finished!" << endl;
#endif

  // Clean up current files again
  ss.str(""); ss << "del " << ROOT_DIR << "*.ifo"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "*.ac3"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "*.txt"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "*.m2v"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "besweet.log"; system(ss.str().c_str());
  ss.str(""); ss << "del " << ROOT_DIR << "*.mp2"; system(ss.str().c_str());

#ifdef VERBOSE
  cout << "Deleted current files" << endl;
#endif

}

  