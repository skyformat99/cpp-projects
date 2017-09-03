#ifndef __WAP_Pool_h__
#define __WAP_Pool_h__

#include "WAP.h"

#include <ostream>
#include <vector>

using namespace std;

class WAP_Pool
{

public:
  WAP_Pool ();

  void addWAP (WAP);
  bool contains (WAP&);

  void print ();

private:

  vector<WAP>* pool; 

};

#endif