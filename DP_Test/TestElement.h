#ifndef __TestElement_h__
#define __TestElement_h__

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ProcessElement.h"

class TestElement : public ProcessElement
{

public:
  TestElement (int);

  void process ();

  int getID ();
  double getResult ();

  bool isProcessed ();

private:
  bool processed;
  int id;
  double result;

};

#endif