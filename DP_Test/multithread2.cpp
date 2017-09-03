#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <process.h>
#include <iostream>
#include "ProcessQueue.h"
#include "TestElement.h"

using namespace std;

HANDLE hRunMutex;

void wrapperThread (void *);

int main () 
{
  hRunMutex = CreateMutex( NULL, FALSE, NULL );

  ProcessQueue* pq = new ProcessQueue(5,hRunMutex);

  _beginthread( wrapperThread, 0, pq );

  TestElement* t1 = new TestElement(1);
  pq->addElement(t1);

  TestElement* t2 = new TestElement(2);
  pq->addElement(t2);

  cout << "Main thread going to sleep ..." << endl;

  Sleep(500);

  TestElement* t3 = new TestElement(3);
  pq->addElement(t3);

  TestElement* t4 = new TestElement(4);
  pq->addElement(t4);

  while (!(t4->isProcessed())) {
    cout << "#";
    Sleep(50);
  }

  cout << "Results: 1) " << t1->getResult() <<
          " 2) " << t2->getResult() <<
          " 3) " << t3->getResult() <<
          " 4) " << t4->getResult() << endl;

  CloseHandle( hRunMutex );

  return(0); 
}

void wrapperThread (void *ptr) 
{
  ProcessQueue* pq = (ProcessQueue*)ptr;

  pq->run();
}