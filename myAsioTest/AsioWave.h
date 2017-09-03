#ifndef __ASIO_WAVE__
#define __ASIO_WAVE__

#include <string>
#include "asiosys.h"
#include "asio.h"
//#include "iasiodrv.h"
#include "asiodrivers.h"

#ifdef MYTXTDEBUG
  #include "AsioDebugger.h"
#endif

/** DEFINES **/
#if NATIVE_INT64
	#define ASIO64toLong(a)  (a)
#else
	#define ASIO64toLong(a)  (((unsigned long long int) a.hi) << 32) | (unsigned long long int) a.lo
#endif

#define TwoPI 6.283185307

/** Adds an extra 50 msec to the sleep call, to ensure the entire sound plays */
#define EXTRA_SLEEP 50L

/** Min and Max macros - don't use with var++ !! */
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

/**
* Struct to hold the buffer data, size, callbacks and 
* current position in the sample
**/
typedef struct BufferVars {
	ASIOBufferInfo *bufferInfos;
	int numInitedChannels; // length of bufferInfos array
	int bufferSize;
	ASIOCallbacks *callbacks;
	long samplePosition;
} BufferVars;

// Max float values for given bit-counts
const float MAX_INT16 = (float) 0x00007FFF;
const float MAX_INT18 = (float) 0x0001FFFF;
const float MAX_INT20 = (float) 0x0007FFFF;
const float MAX_INT24 = (float) 0x007FFFFF;
const float MAX_INT32 = (float) 0x7FFFFFFF;

/** Max amplitude, to prevent bit overflow and clipping **/
const float MAX_SND_AMP = (float) 0.99999997;

/**
 * Function definitions for callbacks. The callbacks necessary for the
 * ASIO system using the Steinberg AsioDriver class cannot be member
 * functions. Therefore we have four non-member functions, and a
 * global variable to access the appropriate member functions.
 */
ASIOTime* _bufferSwitchTimeInfo (ASIOTime* , long , ASIOBool );
void _bufferSwitch (long , ASIOBool );
void _sampleRateDidChange (ASIOSampleRate );
long _asioMessage (long , long , void* , double* );

/**
* This class provides the functionality necessary to present a pure-tone
* sin wave signal over the M-Audio Fast Track device, using the ASIO 
* device interface. We make use (and compile with) the ASIO driver
* provided by Steinberg Media Technologies. For more information, search 
* for Steinberg ASIO Driver on Google. 
*
* @author Adam Dziorny
*/
class AsioWave
{
private:
	static const byte UNLOADED = (byte)0;
	static const byte LOADED   = (byte)1;
	static const byte BUFFERED = (byte)2;

#ifdef MYTXTDEBUG
	AsioDebugger* mout;
#endif

	BufferVars bufferVars;

	float* output;
	float* fullOutput;

	long sndBufferSize; 

	float sndAmp;
	float sndFreq;
	float sndDurSec;
	float sndRampUp;
	float sndRampDown;
	float sndDurSamples;

	bool playR;
	bool playL;

	byte status;

	AsioDrivers* asioDrivers;

	void formFullWave (ASIOSampleRate);

	bool writeFullWave (std::string*);

public:
	AsioWave ();
	~AsioWave ();

	bool createOutputBuffers (int);

	void displayChannelInfo ();

	bool getBufferSizes (long*);
	int getDriverNames (std::string*, long);
	ASIOSampleRate getSampleRate ();

	bool isReady ();

	bool loadDriver (std::string*);
	
	ASIOSampleRate setSampleRate (ASIOSampleRate);

	bool playSound (float,float,float,float,float,bool,bool);

	void registerThread ();

	// Member call-back methods, called from functions on _instance
	void bufferSwitch (long, ASIOBool);
	void sampleRateDidChange (ASIOSampleRate);
	long asioMessage (long, long, void*, double*);
	ASIOTime* bufferSwitchTimeInfo (ASIOTime*, long, ASIOBool);
};

#endif // __ASIO_WAVE__