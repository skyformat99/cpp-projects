#include "TTank.h"

using namespace std;

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

/**
 * Constructs a new TTank without connecting to a server.
 */
TTank :: TTank () 
{
  initialize();
}

/**
 * Constructs a new TTank and immediately connects to a server.
 */
TTank :: TTank (string server) 
{

  initialize();

  if (connectServer(server) != 0)
    throw "connectServer(): failed to connect to server";
}

TTank :: ~TTank ()
{
  CoUninitialize();
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

/**
 * Adds a tank to the TTank server (if this method exists!).
 */
int TTank :: addTank (string tank, string filename) 
{

  long lg = p->AddTank(
    _bstr_t(tank.c_str()),
    _bstr_t(filename.c_str()));

  if (lg == 0)
    return -1;
  else
    return 0;
}

/**
 * Releases the connection to a TTank server.
 */
int TTank :: closeServer () 
{
  HRESULT hr = p->ReleaseServer();

  if (FAILED(hr))
    return -1;
  else
    return 0;
}

/**
 * Closes the connection to the open tank.
 */
void TTank :: closeTank () 
{
  p->CloseTank();
}

/**
 * Connects the TTank object to a server.
 */
int TTank :: connectServer (string server) 
{

  long lg = p->ConnectServer(
    _bstr_t(server.c_str()),
    _bstr_t(L"Me"));

  if (lg == 0)
    return -1;
  else
    return 0;
}

/**
 * Returns true if the server contains the passed tank name,
 * or false otherwise.
 */
bool TTank :: containsTank (string tank) 
{

  vector<string> tanks = this->getTanks();

  vector<string>::iterator it;
  for (it = tanks.begin(); it != tanks.end(); ++it) {
    if (_stricmp((*it).c_str(),tank.c_str()) == 0) return TRUE;
  }

  return FALSE;
}

/**
 * Creates indexing on the individual block after it is opened.
 */
void TTank :: createEpocIndexing () 
{
  p->CreateEpocIndexing();
}

/** 
 * Gets all the servers from the TTank engine.
 */
vector<string> TTank :: getServers () 
{
  vector<string> servers;

  string* s;

  _bstr_t st;
  int i = 0;

  st = p->GetEnumServer(i++);

  while (st.length() > 0) {
    s = new string(st);
    servers.push_back(*s);

    st = p->GetEnumServer(i++);
  }

  return servers;
}

/**
 * Gets all the tanks from the TTank engine.
 */
vector<string> TTank :: getTanks () 
{
  vector<string> tanks;

  string* s;

  _bstr_t st;
  int i = 0;

  st = p->GetEnumTank(i++);

  while (st.length() > 0) {
    s = new string(st);
    tanks.push_back(*s);

    st = p->GetEnumTank(i++);
  }

  return tanks;
}

/**
 * Opens the given tank. Returns 0 on success, -1 on failure.
 */
int TTank :: openTank (string tank) 
{
  long result = p->OpenTank(tank.c_str(),"R");

  if (result == 0)
    return -1;
  else
    return 0;
}

/**
 * Removes the given tank from the TTank server.
 */
int TTank :: removeTank (string tank) 
{

  long lg = p->RemoveTank(
    _bstr_t(tank.c_str()));

  if (lg == 0)
    return -1;
  else
    return 0;
}

/**
 * Selects the given block by number.
 */
int TTank :: selectBlock (int number) 
{
  stringstream ss;
  ss.str("");

  ss << "Block-" << number;
  
  long lg = p->SelectBlock(
    _bstr_t(ss.str().c_str()));

  if (lg == 0)
    return -1;
  else
    return 0;
}

int TTank :: eventRead (string code, int maxRet) 
{
  long ct;

  try {
    ct = p->ReadEventsV (maxRet,_bstr_t(code.c_str()),
           0,0,0.0,0.0,_bstr_t("ALL"));
  } catch (...) {
    throw "Error reading events from TTank";
  }

  return (int)ct;
}

void TTank :: eventParseInfo (int count, int item, 
    double* data, int data_length) throw (char*)
{
  _variant_t v;
  try {
    v = p->ParseEvInfoV (0, count, item);
  } catch (...) {
    throw "Error parsing events read from TTank";
  }

  if (v.vt != (int)(VT_ARRAY | VT_R8))
    cerr << "Error: not real-8 type!" << endl;

  // Detach the variant object from the container
  VARIANT vr = v.Detach();

  double* d;

  HRESULT hr;
  hr = SafeArrayAccessData (vr.parray,(void HUGEP**)&d);
  if (hr == E_INVALIDARG)
    throw "SafeArrayAccessData failed - invalid argument";
  else if (hr == E_UNEXPECTED)
    throw "SafeArrayAccessData failed - array could not be locked";
  else if (hr != S_OK)
    throw "SafeArrayAccessData failed - unknown error";

  // Copy data from protected array into our passed array, up
  // to the max length of the array argument
  int i;
  for (i=0;i<data_length;i++)
    data[i] = d[i];

  hr = SafeArrayUnlock(vr.parray);
  if (hr == E_INVALIDARG)
    throw "SafeArrayUnlock failed - invalid argument";
  else if (hr == E_UNEXPECTED)
    throw "SafeArrayUnlock failed - unexpected error";
  else if (hr != S_OK)
    throw "SafeArrayUnlock failed - unknown error";

  hr = SafeArrayDestroy(vr.parray);
  if (hr == DISP_E_ARRAYISLOCKED)
    throw "SafeArrayDestroy failed - array is locked";
  else if (hr == E_INVALIDARG)
    throw "SafeArrayDestroy failed - argument is invalid";
  else if (hr != S_OK)
    throw "SafeArrayDestroy failed - unknown reason";

  return;
}

void TTank :: eventParse (int count, double* data, int data_length) throw (char*)
{
  _variant_t v;
  try {
    v = p->ParseEvV (0, count);
  } catch (...) {
    throw "Error parsing events read from TTank";
  }

  if (v.vt != (int)(VT_ARRAY | VT_R8))
    cerr << "Error: not real-8 type!" << endl;

  // Detach the variant object from the container
  VARIANT vr = v.Detach();

  double* d;

  HRESULT hr;
  hr = SafeArrayAccessData (vr.parray,(void HUGEP**)&d);
  if (hr == E_INVALIDARG)
    throw "SafeArrayAccessData failed - invalid argument";
  else if (hr == E_UNEXPECTED)
    throw "SafeArrayAccessData failed - array could not be locked";
  else if (hr != S_OK)
    throw "SafeArrayAccessData failed - unknown error";

  // Copy data from protected array into our passed array, up
  // to the max length of the array argument
  int i;
  for (i=0;i<data_length;i++)
    data[i] = d[i];

  hr = SafeArrayUnlock(vr.parray);
  if (hr == E_INVALIDARG)
    throw "SafeArrayUnlock failed - invalid argument";
  else if (hr == E_UNEXPECTED)
    throw "SafeArrayUnlock failed - unexpected error";
  else if (hr != S_OK)
    throw "SafeArrayUnlock failed - unknown error";

  hr = SafeArrayDestroy(vr.parray);
  if (hr == DISP_E_ARRAYISLOCKED)
    throw "SafeArrayDestroy failed - array is locked";
  else if (hr == E_INVALIDARG)
    throw "SafeArrayDestroy failed - argument is invalid";
  else if (hr != S_OK)
    throw "SafeArrayDestroy failed - unknown reason";

  return;
}


// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

/**
 * Initializes the TTank object.
 */
void TTank::initialize () {

  HRESULT hr = CoInitialize(NULL);

  if (FAILED(hr))
    throw "initialize(): failed to initialize COM";
  
  hr = this->p.CreateInstance("TTank.X");

  if (FAILED(hr)) 
    throw "Initialize(): failed to create TTank.X instance";
}