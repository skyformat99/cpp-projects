#define _OUT_REDIRECT_
#define _TIME_ME_

#include <cstdlib>     
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include "WAP_Pool.h"
#include "WAP.h"

using namespace std;

string generateSystemCall (string exec, string fn, int numLoop);
string getTempFilename ();
void monitorSingleRSSI (WAP_Pool* p, string n);

int main (int argc, char* argv[])
{
  /* Initialize random seed */
  srand (time(NULL));

  if (argc < 2) {
    cout << "Usage: test <SSID> [<numIter> [<outFile>]]" << endl;
    exit(1);
  }

  string ssid(argv[1]);

  int numIter = 1;
  if (argc >= 3)
    numIter = atoi(argv[2]);

  string outFile = "out.txt";
  if (argc >= 4)
    outFile = string(argv[3]);

#ifdef _OUT_REDIRECT_
  std::ofstream out(outFile);
  std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
  std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
#endif

  // Create empty WAP pool
  WAP_Pool* p;
  string input;
  double duration;  
  
  while (true) {
    cerr << "> ";
    getline (cin, input);

    if (input.compare("q") == 0) break;

#ifdef _TIME_ME_
    clock_t start = clock();
#endif

    p = new WAP_Pool();

    for (int i = 0; i < numIter; i++)
      monitorSingleRSSI(p, ssid);

    // Print it out
    if (!input.empty()) cout << input << endl;
    p->print();

    delete p;

#ifdef _TIME_ME_
  duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
  cerr << "(" << duration << " sec) ";
#endif
  }

#ifdef _OUT_REDIRECT_
  std::cout.rdbuf(coutbuf); //reset to standard output again
#endif
}

/**
 * Gathers one set of RSSI values and adds to WAP_Pool
 */
void monitorSingleRSSI (WAP_Pool* p, string n)
{
  // Generates the file name, system call and makes call
  string exec = "wifiinfoview.exe";

  string fn = getTempFilename();

  string sc = generateSystemCall(exec,fn,1);

  system (sc.c_str());

  // Read in the file, parse to name, mac, rssi
  ifstream infile(fn);

  string line, name, mac;
  int rssi;

  getline(infile, line); // pull off header line

  size_t pos1, pos2;
  while (getline(infile, line)) {
    pos1 = line.find_first_of(",");
    name = line.substr(0,pos1);

    pos2 = line.find_first_of(",",pos1+1);
    mac = line.substr(pos1+1, pos2-pos1-1);
    pos1 = pos2;

    pos2 = line.find_first_of(",",pos1+1);
    pos1 = pos2;

    pos2 = line.find_first_of(",",pos1+1);
    rssi = stoi(line.substr(pos1+1,pos2-pos1-1));

    if (!n.empty() && n.compare(name) == 0) {
      WAP w(name,mac,rssi);
      p->addWAP(w);
    }
  }

  infile.close();

  // Remove this CSV file
  stringstream ss;

  ss << "del " << fn;
  system(ss.str().c_str());
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