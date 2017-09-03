#include "WAP.h"
#include <iostream>

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

WAP::WAP () 
{
  this->rssi = new vector<int>();
}

WAP::WAP (string name, string mac, int r)
{
  this->name = name;
  this->mac = mac;

  this->rssi = new vector<int>();
  this->rssi->push_back(r);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

bool WAP::equals (WAP& w)
{
  return (this->mac.compare(w.mac) == 0);
}

void WAP::update (WAP& w)
{
  for (vector<int>::iterator it = w.rssi->begin(); it != w.rssi->end(); ++it)
    this->rssi->push_back(*it);
}

void WAP::print ()
{
  cout << this->name.c_str() << "," << this->mac.c_str() << ",";

  for (vector<int>::iterator it = this->rssi->begin(); it != this->rssi->end(); ++it) {
    cout << *it;
    if (it+1 != this->rssi->end()) cout << ",";
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *