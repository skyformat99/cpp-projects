#include <windows.h>

#include "ProcessQueue.h"
#include "ProcessElement.h"

#define WAIT_DUR_MS 100

ProcessQueue::ProcessQueue (int e, HANDLE m)
{
  this->maxNumElements = e;
  elements = new ProcessElement*[e];

  this->runMutex = m;

  this->numElements = 0;
  this->doneProcessing = 0;
}

ProcessQueue::~ProcessQueue ()
{
  delete elements;
}

int ProcessQueue::addElement (ProcessElement *e)
{
  WaitForSingleObject( runMutex, INFINITE );

  if (numElements+1 > maxNumElements)
    return -1;

  elements[numElements++] = e;

  ReleaseMutex( runMutex );

  return numElements;
}

void ProcessQueue::run ()
{
  ProcessElement* nextElement;

  while (doneProcessing < maxNumElements) {
    WaitForSingleObject( runMutex, INFINITE );

    if (numElements > doneProcessing)
      nextElement = elements[doneProcessing];

    ReleaseMutex( runMutex );

    nextElement->process();
    doneProcessing++;

    Sleep(WAIT_DUR_MS);
  }

  return;
}
