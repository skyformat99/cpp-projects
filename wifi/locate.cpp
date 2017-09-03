#define _OUT_REDIRECT_
#define UPDATE_WEB
#define DEBUG

#include <cstdlib>     
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <ctime>
#include <Windows.h>

#include "Grid.h"
#include "Fingerprint.h"

using namespace std;

string* loadMacList (string file, size_t& n);
void loadDataPoints (string fn, Grid* g);
Fingerprint* loadSingleFingerprint (string s, size_t n, string* macs);
string generateSystemCall (string exec, string fn, int numLoop);
string getTempFilename ();
size_t monitorSingleRSSI (string**, double**, string);
Fingerprint* captureSingleFingerprint (string, size_t, string*);
void captureMultiFingerprints(Grid*, string, string, size_t, string*, string*, string*);
void updateWebServer (string* host, string* name, double x, double y);

bool sendData (string* str_host, string* path, string* data);

int main (int argc, char* argv[])
{
  if (argc < 3) {
    cerr << "Usage: locate <MACfile> <DataFile> [ssid] [outFile] [comp-name] [web-host]" << endl;
    exit(1);
  }

  string macFile = argv[1];
  string dataFile = argv[2];

  string ssid = "chopnet";
  if (argc >= 4)
    ssid = argv[3];

  string outFile = "out.csv";
  if (argc >= 5)
    outFile = argv[4];

  string compName = "UNKNOWN-PC";
  if (argc >= 6)
    compName = argv[5];

  string webHost = "adam-gamer.noip.us";
  if (argc >= 7)
    webHost = argv[6];

  size_t numMacs;
  string* macs = loadMacList (macFile, numMacs);

  Grid* g = new Grid(numMacs,macs);

  loadDataPoints (dataFile, g);

#ifdef _OUT_REDIRECT_
  std::ofstream out(outFile);
  std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
  std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
#endif

  // Loop to solve for location
  string input;
  double x, y;
  Fingerprint* f = NULL;

  while (true) {
    cerr << "> ";
    getline (cin, input);

    // Load from file?
    if (input.substr(0,1).compare("f") == 0) {
      f = loadSingleFingerprint(input,numMacs,macs);

      if (g->calcDistance(f,x,y)) {
        cerr << "(" << x << "," << y << ") ";
#ifdef UPDATE_WEB
        updateWebServer(&webHost,&compName,x,y);
#endif
      } else {
        cerr << "(?,?) ";
      }

    // Get single RSSI capture from WiFi
    } else if (input.substr(0,1).compare("s") == 0) {
      f = captureSingleFingerprint(ssid,numMacs,macs);
 
      if (g->calcDistance(f,x,y)) {
        cerr << "(" << x << "," << y << ") ";
#ifdef UPDATE_WEB
        updateWebServer(&webHost,&compName,x,y);
#endif
      } else {
        cerr << "(?,?) ";
      }

    // Get single RSSI and weight based on values passed
    } else if (input.substr(0,1).compare("w") == 0) {
      f = captureSingleFingerprint(ssid,numMacs,macs);      

      size_t pos = input.find_first_of(",");
      double slope = stod(input.substr(2,pos-1));
      double intercept = stod(input.substr(pos+1));

      if (g->calcWeightedDistance(f,x,y,slope,intercept)) {
        cerr << "(" << x << "," << y << ") ";
#ifdef UPDATE_WEB
        updateWebServer(&webHost,&compName,x,y);
#endif
      } else {
        cerr << "(?,?) ";
      }

    // Get repeated RSSI captures
    } else if (input.substr(0,1).compare("m") == 0) {
      captureMultiFingerprints(g,ssid,input,numMacs,macs,&compName,&webHost);
      f = NULL;

    // Print params
    } else if (input.compare("p") == 0) {
      cerr << " MAC File: " << macFile << endl
           << " Data File: " << dataFile << endl
           << " SSID: " << ssid << endl
           << " OutFile: " << outFile << endl;
      f = NULL;

    // Quit
    } else if (input.compare("q") == 0) {
      break;

    // Get Help
    } else if ((input.compare("?") == 0) ||
               (input.compare("h") == 0)) {
      cerr << "Options:" << endl 
           << "  f <file.csv>    - Loads fingerprint from file" << endl
           << "  m <options>     - Multi-data-point sample" << endl
           << "    Format of <options>:" << endl
           << "      <slope>,<int>,<interval>,<num>,<sSlope2>,<sSlope1>,<sInt>" << endl
           << "        [Good starting points: 0.02,1.6,5000,-1,-0.0026,-0.0044,+1.04]" << endl
           << "        [Tighter: 0.02,1.6,5000,-1,-0.0107,0.0086,+1.06]" << endl
           << "        [Inverted: -0.1,-1,5000,-1,0.2321,-1.7357,3]" << endl
           << "        [Exp: -0.1,-1,5000,-1,1.0632,0.3279,0.]" << endl
           << "  s               - Single data-point sample, no weights" << endl
           << "  w <slope>,<int> - Single data-point sample, weighted" << endl
           << "     [Good starting points: 0.02,1.6]" << endl
           << "  p               - Prints parameters from argv" << endl
           << "  q               - Quits" << endl
           << "  h or ?          - Displays this help" << endl;
      f = NULL;

    // Invalid selection - try again      
    } else {
      f = NULL;
    }

    if (f) delete f;
  }

#ifdef _OUT_REDIRECT_
  std::cout.rdbuf(coutbuf); //reset to standard output again
#endif

  exit(0);
}

/**
 * Sends updated position to web server
 */
void updateWebServer (string* host, string* name, double x, double y)
{
  string path = "/wifi/update.php";

  stringstream ss;
  ss << "name=" << (*name) << "&x=" << x << "&y=" << y;
  string data = ss.str();

  sendData(host,&path,&data);

  return;
}

/**
 * Captures a set of fingerprints and computes estimated location. 
 */
void captureMultiFingerprints(Grid* g, string ssid, string line,
    size_t numMacs, string* macs, string* compName, string* webHost)
{
  // Parse line for properties
  // slope, intercept, interval (sec), tNumber, speedSlope, speedInt
  size_t pos1, pos2;
  double weightM, weightB, gInterval, speedM2, speedM1, speedB;
  int tNumber;

  pos1 = line.find_first_of(",");
  weightM = stod(line.substr(2,pos1-1));
    
  pos2 = line.find_first_of(",",pos1+1);
  weightB = stod(line.substr(pos1+1, pos2-pos1-1));
  pos1 = pos2;

  pos2 = line.find_first_of(",",pos1+1);
  gInterval = stod(line.substr(pos1+1, pos2-pos1-1));
  pos1 = pos2;

  pos2 = line.find_first_of(",",pos1+1);
  tNumber = stoi(line.substr(pos1+1, pos2-pos1-1));
  pos1 = pos2;

  pos2 = line.find_first_of(",",pos1+1);
  speedM2 = stod(line.substr(pos1+1, pos2-pos1-1));
  pos1 = pos2;

  pos2 = line.find_first_of(",",pos1+1);
  speedM1 = stod(line.substr(pos1+1, pos2-pos1-1));
  pos1 = pos2;

  pos2 = line.find_first_of(",",pos1+1);
  speedB = stod(line.substr(pos1+1, pos2-pos1-1));
  pos1 = pos2;

#ifdef DEBUG
  cerr << "Weight: " << weightM << "," << weightB << endl
       << "Interval (sec): " << gInterval << " Number: " << tNumber << endl
       << "Speed: " << speedM2 << "," << speedM1 << "," << speedB << endl;
#endif

  if (tNumber == -1) tNumber = INT_MAX;

  // Capture the first and compute just weighted distance
  Fingerprint* f = captureSingleFingerprint(ssid,numMacs,macs);
  if (!f) {
    cerr << "First captured fingerprint was NULL" << endl;
    return;
  }
 
  // Start the clock
  clock_t next, start = clock();

  double x, y, duration, interval = gInterval, FF = 0.4;
  if (!g->calcWeightedDistance(f,x,y,weightM,weightB)) {
    cerr << "First calculated weighted distance was FALSE" << endl;
    return;
  }

  cerr << "(" << x << "," << y << ")" << endl;
#ifdef UPDATE_WEB
  updateWebServer(webHost,compName,x,y);
#endif

  if (f) delete f;

  // Loop ... capture, sleep, get duration, get new x,y, 
  //          re-calculate interval, increment, repeat
  int count = 1;
  while (count < INT_MAX) {
    f = captureSingleFingerprint(ssid,numMacs,macs);

    Sleep(interval); 

    duration = ( (next = clock()) - start ) / (double) CLOCKS_PER_SEC;
    start = next;

    if (g->calcWeightedTimedDistance(f,x,y,weightM,weightB,
               speedM2,speedM1,speedB,duration,true)) {
      cerr << "(" << x << "," << y << ")" << endl;
#ifdef UPDATE_WEB
      updateWebServer(webHost,compName,x,y);
#endif
    } else {
      cerr << "(?,?)" << endl;
    }

#ifdef DEBUG
    cerr << "duration: " << duration << " interval: " << interval << " gInterval: " << gInterval;
#endif
    if (abs((duration*1000.) - gInterval) > 0.1) interval = interval - FF * ((duration*1000.) - gInterval);
#ifdef DEBUG
    cerr << " new interval: " << interval << endl;
#endif

    if (f) delete f;
    count++;
  }
}

/**
 * Executes the wifiinfoview executable and returns the 
 * resulting MAC / RSSI data in the passed pointers, as well
 * as the number of elements as a return value.
 */
size_t monitorSingleRSSI (string** m, double** d, string n)
{
  // Generates the file name, system call and makes call
  string exec = "wifiinfoview.exe";

  string fn = getTempFilename();

  string sc = generateSystemCall(exec,fn,1);

  system (sc.c_str());

  // Read in the file, parse to name, mac, rssi
  ifstream infile(fn);

  string line, name, mac;
  double rssi;

  // Declare temp arrays of size MAX_MAC - could be done
  // with vectors, but this is easier
  int MAX_MAC = 50;
  string* m_ = new string[MAX_MAC];
  double* d_ = new double[MAX_MAC];
  int i = 0, j;

  getline(infile, line); // pull off header line

  size_t pos1, pos2;
  while (getline(infile, line) && (i < MAX_MAC)) {
    pos1 = line.find_first_of(",");
    name = line.substr(0,pos1);
    
    pos2 = line.find_first_of(",",pos1+1);
    mac = line.substr(pos1+1, pos2-pos1-1);
    pos1 = pos2;

    pos2 = line.find_first_of(",",pos1+1);
    pos1 = pos2;

    pos2 = line.find_first_of(",",pos1+1);
    rssi = stod(line.substr(pos1+1,pos2-pos1-1));

    if (!n.empty() && n.compare(name) == 0) {
      m_[i] = mac;
      d_[i] = rssi;

      i++;
    }
  }

  infile.close();

  // Remove this CSV file
  stringstream ss;

  ss << "del " << fn;
  system(ss.str().c_str());

  // Now prune down to new arrays, and return size
  (*m) = new string[i];
  (*d) = new double[i];

  for (j=0;j<i;j++) {
    (*m)[j] = m_[j];
    (*d)[j] = d_[j];
  }

  delete[] m_;
  delete[] d_;

  return i;
}

/** 
 * Captures a single set of RSSIs using the monitorSingleRSSI
 * function and converts into a fingerprint.
 */
Fingerprint* captureSingleFingerprint (string ssid, size_t n, string* macs)
{
  string* m = NULL;
  double* d = NULL;

  size_t samples = monitorSingleRSSI (&m, &d, ssid);

  Fingerprint* fp = new Fingerprint(n,macs);

  for (int i=0; i<samples; i++)
    fp->update(m[i],d[i]);

  return fp;
}

/**
 * Loads a single footprint, saved in .csv format below, 
 * and returns the pointer to the data on the heap.
 *
 * - Format: N lines of format: MAC,RSSI
 */
Fingerprint* loadSingleFingerprint (string s, size_t n, string* macs)
{
  Fingerprint* fp = new Fingerprint(n,macs);

  string fn = s.substr(2);

  ifstream infile(fn);

  size_t pos;
  string line, mac;
  double rssi;

  while (getline(infile, line)) {
    pos = line.find_first_of(",");
    mac = line.substr(0,pos);
    rssi = stod(line.substr(pos+1));

    fp->update(mac,rssi);
  }

  infile.close();

  return fp;
}

/**
 * Loads the MAC data points, formatted as follows:
 * - Each line is a data element, containing:
 *     MAC Address, X, Y, RSSI
 */
void loadDataPoints (string fn, Grid* g)
{
  ifstream infile(fn);

  size_t pos1, pos2;
  string line, mac;
  double x, y, rssi;

  while (getline(infile, line)) {
    pos1 = line.find_first_of(",");
    mac = line.substr(0,pos1);
    
    pos2 = line.find_first_of(",",pos1+1);
    x = stod(line.substr(pos1+1, pos2-pos1-1));
    pos1 = pos2;

    pos2 = line.find_first_of(",",pos1+1);
    y = stod(line.substr(pos1+1, pos2-pos1-1));
    pos1 = pos2;

    pos2 = line.find_first_of(",",pos1+1);
    rssi = stod(line.substr(pos1+1,pos2-pos1-1));

    g->add(mac, x, y, rssi);
  }

  infile.close();
}

/**
 * Loads a file with the MAC address list used.
 * Format as follows:
 *  - First line contains number of unique MAC addresses, N
 *  - Each N subsequent lines contain the string of these 
 *    addresses
 */
string* loadMacList (string fn, size_t& n)
{
  ifstream infile(fn);

  string line;

  getline(infile, line); // pull off header line

  n = (size_t)stoi(line);
  string* m = new string[n];

  for (int i=0; i<n; i++) {
    getline(infile, line);

    m[i] = line;
  }

  infile.close();

  return m;
}

/** 
 * Returns the name of a temporary filename, named 
 * for "time" and a random number
 */
string getTempFilename ()
{
  stringstream ss;

  ss << "temp_" << time(NULL) << "_" << (rand() % 10000) << ".csv";

  return ss.str();
}

/**
 * Generates the executable call string for WifiInfoView
 */
string generateSystemCall (string exec, string fn, int numLoop)
{
  stringstream ss;

  ss << exec << " /scomma \"" << fn << "\" /NumberOfScans " << numLoop << " /UpdateRate 4";

  return ss.str();
}