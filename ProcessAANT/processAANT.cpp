#include <string>
#include <sstream>
#include "ToolboxData.h"

using namespace std;

/**
 * Main method for processing AANT output files into
 * .csv files.
 *
 * ACD, 09-Feb-2012
 */
int main (int argc, char* argv[]) 
{
  // Check number of arguments
  if (argc < 2) {
    cout << "Usage: processAANT <textFile.txt> [<outFile.csv>]" << endl;
    exit(-1);
  }
  
  // Determine what the output filename is
  stringstream ss;
  string infileName (argv[1]), outfileName;
  if (argc == 2) {
    if (infileName.substr(infileName.length()-4) == ".txt") {
      ss << infileName.substr(0,infileName.length()-4);
    } else {
      ss << infileName;
    }
    ss << ".csv";
    outfileName = ss.str();
  } else {
    outfileName = argv[2];
  }

  // Load the data here
  ToolboxData* data;
  try {
    data = new ToolboxData(infileName);
  } catch (...) {
    cerr << "Error loading ToolboxData from text file!" << endl;
    exit(-1);
  }

  if (!data->isLoaded()) {
    cerr << "Error: ToolboxData is not loaded!" << endl;
    exit(-1);
  }

  // Do something with it here
//  data->printData();

  data->writeToCSV(&outfileName);

  // Clean up and exit
  delete data;
  exit(0);
}
