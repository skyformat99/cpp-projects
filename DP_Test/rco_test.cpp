//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <iostream>
#include "Transducers.h"

#import "progid:TTank.X" 
//#import "progid:RPco.X" no_namespace

//_DRPcoXPtr RP;
TTANKXLib::_DTTankXPtr RP;

//using namespace std;

int main(int argc, char* argv[])
{
  //Initialize ActiveX object
  HRESULT hr;
  hr = CoInitialize(NULL);
  if (FAILED(hr)) {
    printf("Failed to initialize COM!\n");
  }

//  const char* appId = "{d323a625-1d13-11d4-8858-444553540000}"; //"RPco.X"
  const char* appId = "{670490CE-57D2-4176-8E74-80C4C6A47D88}"; //"TTank.X"
//  const char* appId = "{EC05FCDE-300D-4CE3-9774-A4C377507BD7}"; //"PA5.X"
  hr = RP.CreateInstance(appId);

  if (FAILED(hr)) {
    printf("CreateInstance for %s failed!\n", appId);
  } else {
    printf("Successfully initialized TDT ActiveX interface %s\n", appId);
  }

  if (0 == RP) return -1;

  _bstr_t sv1 = RP->GetEnumServer(0);

  cout << "Server 0: " << sv1 << endl;

  /** TDT ActiveX commands
  if (RP->ConnectRP2("GB",1)) {  
    printf("Connected to RP2\n");
  }
  if (!RP->ClearCOF()) {  
    printf("ClearCOF failed\n");
  }
  if (RP->LoadCOF(circuitPath)) {
    printf("%s Loaded\n", circuitPath);
  }
  if (RP->Run()) {  
    printf("Circuit running\n");
  } **/

//  Transducers t;
//  t.connectAndLoad(1,2,1,2,"C:/TDT/ActiveX/DPOAE_Stimulus.rcx");

  return 0;
}