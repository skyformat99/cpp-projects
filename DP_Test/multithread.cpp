#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <process.h>
#include <time.h>
#include <math.h>
#include <iostream>

using namespace std;

void ThreadStart ( void* );

HANDLE hRunMutex;

struct DATA {
  int ThreadNr;
  double result;
};

int main () 
{
  DATA tData = {1, -1};
  hRunMutex = CreateMutex( NULL, TRUE, NULL );

  _beginthread( ThreadStart, 0, &tData);

  Sleep(500);

  cout << "Main execution thread releasing mutex ..." << endl;

  ReleaseMutex( hRunMutex );

  WaitForSingleObject( hRunMutex, INFINITE );

  cout << "Main execution ... result: " << tData.result << endl;

  CloseHandle( hRunMutex );

  return(0);
}

void ThreadStart (void *pMyID)
{
  DATA *MyID = (DATA*)pMyID;

  cout << "Thread " << MyID->ThreadNr << " waiting for Mutex ..." << endl;

  WaitForSingleObject( hRunMutex, INFINITE );

  cout << "Thread " << MyID->ThreadNr << " has mutex, calculating and then releasing ..." << endl;

  srand( (unsigned)time( NULL ) );

  int i;
  MyID->result = 0.;
  for (i=0;i<10000000;i++) 
    MyID->result = MyID->result + (rand() * pow(-1.,i));
  
  ReleaseMutex( hRunMutex );

  return;   
}