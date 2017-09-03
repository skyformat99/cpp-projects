#ifndef __Transducers_h__
#define __Transducers_h__

#include <iostream>

// Import the ActiveX controllers
#import "progid:RPco.X" no_namespace
#import "progid:PA5.x" no_namespace

using namespace std;

class Transducers {

public:
  Transducers ();
  ~Transducers ();

  int connectAndLoad (int, int, int, int, string);

private:
  HRESULT hr;

  _DRPcoXPtr RP_1;
  _DRPcoXPtr RP_2;
  _DPA5xPtr PA_1;
  _DPA5xPtr PA_2;

  void initialize ();
  template<class T> int initializeInstance (T, const char*);

};

#endif