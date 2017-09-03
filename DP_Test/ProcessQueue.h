#ifndef __ProcessQueue_h__
#define __ProcessQueue_h__

#include "ProcessElement.h"

class ProcessQueue
{

public:
  ProcessQueue (int, HANDLE);
  ~ProcessQueue ();
  int addElement (ProcessElement*);
  void run ();

private:
  HANDLE runMutex;
  ProcessElement **elements;
  int numElements;
  int maxNumElements;

  int doneProcessing;
  
};

#endif
