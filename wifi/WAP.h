#ifndef __WAP_h__
#define __WAP_h__

#include <ostream>
#include <vector>

using namespace std;

class WAP
{

public:
  WAP ();
  WAP::WAP (string name, string mac, int rssi);

  bool equals (WAP&);
  void update (WAP&);

  void print ();

private:

  string name;
  string mac;

  vector<int>* rssi;

};

#endif