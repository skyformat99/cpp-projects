#include <stdio.h>

#include <vector>
#include <string>
#include <iostream>

#include "ClientFarm.h"

#include "TankServerDefs.h"

using namespace std;

int main (int argc, char* argv[]) {

  if (argc <= 2) {
    cout << "Usage: findAllTanks <tank_prefix> <server_file>" << endl;
    exit(-1);
  }

  stringstream s;
  s.str("");

  ClientFarm *cf;
  try {
    cf = new ClientFarm(argv[2]);
  } catch (char *c) {
    cout << "Error creating ClientFarm: " << c << endl;
    exit(1);
  }

  vector<string> tanks;
  int numElements = cf->getTankList(argv[1],&tanks);

  if (numElements > 0) {

    vector<string>::iterator it;
    for ( it = tanks.begin() ; it < tanks.end(); it++ ) 
      s << *it << ";";

  } else {
    cout << "Error: No Tanks Found!" << endl;
  }

  cout <<  s.str().substr(0,s.str().size()-1);

  delete cf;

  exit(0);
}
