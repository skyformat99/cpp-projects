#include <iostream>
#include <string>

#include "ExperimentCollection.h"
#include "Experiment.h"
#include "Tank.h"
#include "Block.h"
#include "Module.h"

/**
 * Link using: msxml2.lib ole32.lib oleaut32.lib comsuppw.lib
 */

using namespace std;

/**
 * Asks the user the specified message, and then waits for
 * input. If the input matches either 'y', 'Y', 'yes', 'Yes', 
 * or 'YES', then true is returned; otherwise false is returned.
 */
bool verifyCorrect (string msg) 
{
  cout << endl << msg;

  string s;
  cin >> s;

  if (s.compare("y") == 0 ||
      s.compare("Y") == 0 ||
      s.compare("yes") == 0 ||
      s.compare("Yes") == 0 ||
      s.compare("YES") == 0) 
    return true;
  else
    return false;
}

/**
 * Main entry into function. 
 */
int main (int argc, char *argv[]) {

  ExperimentCollection* ec = new ExperimentCollection();

  if (argc < 1)
    ec->loadFromXML(argv[1]);
  else   
    ec->loadFromXML("testDOM.xml");

  ec->printSummary(&cout);

  bool isCorrect = verifyCorrect("Is this correct? (y/n) ");

  if (isCorrect) 
    cout << "Excellent!" << endl;
  else
    cout << "Boo ..." << endl;

  delete ec;

  exit(0);
}