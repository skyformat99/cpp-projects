#include <stdio.h>
#include <process.h>
//#include <stdlib.h>
//#include <math.h>
//#include <float.h>
#include <iostream>
//#include "Timer.h"

#include <tchar.h>
#include "TTankX.tlh"
#include "TTankX.tli"
//#import "progid:TTank.X" no_namespace

#define BUFFER_SIZE 4096

using namespace std;

/** Move one file to another */
void copyFile (char*, char*);

/**
 * Loads in the SpikeList .mat & .txt file.
 */
int main (int argc, char* argv[]) {

/**
  Timer *t = new Timer();

  double tester = 0.3240583958355;
  long double tester2;
  int codeVal;

  sscanf_s("0.3240583958355","%lf",&tester2);

  sscanf_s("NaN","%d",&codeVal);

  printf("Tester 0.f: %.0f Tester2: %0.11f\n",floor(tester*1000000),tester2);

  if (_isnan(codeVal))
    printf("Is NaN!\n");
  else
    printf("Is not NaN!\n");

  int i;
  double d;

  for (i=0;i<9000000;i++)
    d = (i ^ 2) / 500.;
**/

  HRESULT hr;

  hr = CoInitialize(NULL);

  if (FAILED(hr)) {
    cout << "Failed initialization!\n";
    exit(1);
  }
  
  _DTTankXPtr p;

  hr = p.CreateInstance("TTank.X");

/**
  if (hr == S_OK)
    cerr << "S_OK\n";
  else if (hr == REGDB_E_CLASSNOTREG)
    cerr << "REGDB_E_CLASSNOTREG\n";
  else if (hr == CLASS_E_NOAGGREGATION)
    cerr << "CLASS_E_NOAGGREGATION\n";
  else if (hr == E_NOINTERFACE)
    cerr << "E_NOINTERFACE\n";
**/

  if (FAILED(hr)) {
    cerr << "Failed creation!\n";
    exit(1);
  }

  long lg = p->ConnectServer(
    _bstr_t(L"Local"),
    _bstr_t(L"Me"));

  if (lg == 0) {
    cerr << "Failed connecting to server!\n";
    exit(1);
  }

  _bstr_t st;
  int i = 0;

  cout << "Tanks:: \n";

  st = p->GetEnumTank(i++);

  while (st.length() > 0) {
//    cout << "  " << st << "\n";
    st = p->GetEnumTank(i++);
  }

  hr = p->ReleaseServer();

  if (FAILED(hr))
  {
    cout << "Failed to release server!\n"; 
    exit(1);
  }

//  copyFile("\\\\adam-gamer\\c$\\vlc-help.txt","vlc-help.txt");

//  system("copy Timer.cpp Timer2.cpp");

//  system("time /T > time.txt");


/**
  t->print();

  t->reset();

  for (i=0;i<5000000;i++)
    d = (i ^ 2) / 500.;

  t->print("You suck!");
**/

  exit(0);
}

/** 
 * Copy contents of one file to another buffer-by-buffer. 
 */
void copyFile (char* src, char* dest) {

  FILE *f_src, *f_dst;

  byte buf[BUFFER_SIZE];
  int actualSize;

  // Open the file streams
  f_src = fopen(src,"r");
  f_dst = fopen(dest,"w");

  // Copy buffer-by-buffer 
  actualSize = (int)fread(buf,sizeof(byte),BUFFER_SIZE,f_src);
  while (actualSize > 0) {

    fwrite(buf,sizeof(byte),actualSize,f_dst);
    actualSize = (int)fread(buf,sizeof(byte),BUFFER_SIZE,f_src);
  }

  // Close the file streams
  fclose(f_src);
  fclose(f_dst);
}
