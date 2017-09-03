#include <iostream>
#include <string>

#include "XMLDocument.h"
#include "XMLNode.h"
#include "XMLNodeList.h"

/**
 * Link using: msxml2.lib ole32.lib oleaut32.lib comsuppw.lib
 */

using namespace std;

/**
 * Main entry into function. 
 */
int main (int argc, char *argv[]) {
  
  XMLDocument* doc = new XMLDocument("testDOM.xml");

  XMLNode* condSheet = new XMLNode();
  doc->getRootNode("condition_sheet",condSheet);

  XMLNodeList* exptList = new XMLNodeList();
  condSheet->getNodes("experiment",exptList);

  cout << "Num Experiments: " << exptList->length() << endl;

  XMLNode* expt = new XMLNode();
  exptList->getNode(0,expt);

  cout << "Num Attributes: " << expt->getNumberOfAttributes() << endl;
  cout << "  Date: " << expt->getAttribute("date") << endl;

  XMLNodeList* tankList = new XMLNodeList();
  expt->getNodes("tank",tankList);

  cout << "Num Tanks: " << tankList->length() << endl;

  delete tankList;
  delete expt;
  delete exptList;
  delete condSheet;
  delete doc;

  exit(0);
}