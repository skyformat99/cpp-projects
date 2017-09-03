#include "Transducers.h"

Transducers::Transducers ()
{
  initialize();
}

Transducers::~Transducers ()
{
  // Uninitialize anything here
}

int Transducers::connectAndLoad (int rp1, int rp2, int pa1, int pa2, string circuit)
{
//  RP_1->ConnectRP2("GB",rp1);
//  PA_1->ConnectPA5("GB",pa1);
  
//  RP_1->ClearCOF();
//  RP_2->ClearCOF();

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

  return -1;
}

void Transducers::initialize ()
{
  //Initialize ActiveX object
  hr = CoInitialize(NULL);
  if (FAILED(hr)) {
    cout << "Failed to initialize COM!\n";
  }

  const char* rpoAppId = "{d323a625-1d13-11d4-8858-444553540000}"; //"RPco.X"
  const char* pa5AppId = "{EC05FCDE-300D-4CE3-9774-A4C377507BD7}"; //"PA5.X"

  initializeInstance(RP_1,rpoAppId);
  initializeInstance(RP_2,rpoAppId);
  initializeInstance(PA_1,pa5AppId);
  initializeInstance(PA_2,pa5AppId);

  return;  
}

template<class T>
int Transducers::initializeInstance (T ptr, const char* appId)
{
  hr = ptr.CreateInstance(appId);

  if (FAILED(hr))  {
    cout << "CreateInstance for " << appId << " failed!" << endl;
    return -1;
  }

  cout << "Successfully initialized ActiveX " << appId << endl;
  return 1;
}