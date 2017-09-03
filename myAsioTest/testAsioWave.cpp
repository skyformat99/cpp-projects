#include <iostream>
#include <string>
#include <stdlib.h>
#include "AsioWave.h"

#ifdef TXTDEBUG
  #include "AsioDebugger.h"
#endif

using namespace std;

void main (int argc, char* argv[])
{
  AsioWave* asio;

#ifdef TXTDEBUG
  AsioDebugger* mout;
#endif

  int count = 0, maxCount = 20250;
  while (count < maxCount) {

    asio = new AsioWave();
#ifdef TXTDEBUG
    mout = AsioDebugger::getAsioDebugger();

    mout->printTime();
    (*mout) << "Trial " << count++ << "\n";
    mout->flush();
#endif

    // isReady?
    bool isReady = asio->isReady();
//    cout << "isReady? " << ((isReady) ? "true" : "false") << endl;

    // asioDevice.registerThread();
    asio->registerThread();

    // Get the names, load the one with ASIO in the title
    long maxNames = 32L;
    string* devices = new string[maxNames];
    int numDevices = asio->getDriverNames(devices,maxNames);

//    cout << "numDevices: " << numDevices << endl;
    if (numDevices == 0) {
      cerr << "Error: no devices present!" << endl;
      exit(-1);
    }

    int i;
    size_t found;
    bool isLoaded;
    for (i = 0; i < numDevices; i++) {
//      cout << "  " << i << ": #" << devices[i] << "#" << endl;
      found = devices[i].find("ASIO");
      if (found != string::npos) {
        isLoaded = asio->loadDriver(new string(devices[i]));
        break;
      }
    }

    isLoaded = asio->loadDriver(new string(devices[i]));

    delete[] devices;

    if (!isLoaded)
      throw "Error: could not load specified driver!";

    // Display channel info
//    asio->displayChannelInfo();

    // Get sample rate
//    cout << "Sample rate: " << asio->getSampleRate() << endl;

    // Get buffer sizes, create preferred buffer
    long* bufSizes = new long[3];
    bool gotBufferSizes = asio->getBufferSizes(bufSizes);
//    if (gotBufferSizes)
//      cout << "Buffer sizes: min: " << bufSizes[0] << " max: "
//        << bufSizes[1] << " preferred: " << bufSizes[2] << endl;
  
    bool isCreated = asio->createOutputBuffers((int)bufSizes[2]);
//    if (isCreated)
//      cout << "Initialized output buffer at size = " << bufSizes[2] << endl;

    bool didPlay = asio->playSound(0.05,4000,1.0,0.05,0.95,true,true);

//    if (didPlay)
//      cout << "Presented tone!!" << endl;

    // Clean-up!
    delete asio;

    delete[] bufSizes;

#ifdef TXTDEBUG
    if (mout)
      delete mout;
#endif

    // A day of rest ...
    Sleep(125);

  }

  exit(0);
}