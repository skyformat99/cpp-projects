#include <iostream>
#include <fstream>
#include <math.h>
#include "AsioWave.h"

#ifdef MYTXTDEBUG
  #include "AsioDebugger.h"
#endif

using namespace std;

// Global instance for wrapping the required function pointers
AsioWave* _instance;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// CONSTRUCTORS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

AsioWave::AsioWave ()
{
#ifdef MYTXTDEBUG
  mout = AsioDebugger::getAsioDebugger();
  mout->printTime();
  (*mout) << "AsioWave::AsioWave - before CoInitializes" << "\n";
  mout->flush();
#endif

	CoInitialize(NULL);

	asioDrivers = new AsioDrivers();

	status = UNLOADED;

	_instance = this;

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::AsioWave - after CoInitializes" << "\n";
  mout->flush();
#endif
}

AsioWave::~AsioWave ()
{
#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::~AsioWave - removing drivers, CoUninitializing" << "\n";
  mout->flush();
#endif
//	cout << "Disposing of AsioWave ..." << endl;

	if (status >= BUFFERED) {
//		cout << "Disposing of BUFFERED AsioWave" << endl;
		delete bufferVars.bufferInfos;
		delete bufferVars.callbacks;

		if (output) delete output;
//		if (fullOutput) delete fullOutput;

		ASIODisposeBuffers();
		status--;
	}

	if (status >= LOADED) {
		asioDrivers->removeCurrentDriver();
		status--;
	}

	CoUninitialize();

//	cout << "After CoUninitialize, before ASIOExit" << endl;

//	ASIOExit();

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::~AsioWave - done deconstructing" << "\n";
  mout->flush();
#endif
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

/**
 * Creates the output buffers in preparation for playing 
 * tones. This involves initializing the buffers for each
 * channel (x2 since we're double-buffering). The function
 * pointers are also set for call-backs.
 *
 * @return True if the output buffers are initialized, false otherwise
 */
bool AsioWave::createOutputBuffers (int bufferSize)
{
#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::createOutputBuffers (" << bufferSize << ") - starting method" << "\n";
  mout->flush();
#endif

	if (status < LOADED)
		throw "Exception: Must load before creating buffers";

	// Get the number of output channels first
	long nInput, nOutput;
	ASIOError errorCode = ASIOGetChannels(&nInput, &nOutput);
	
	if (errorCode != ASE_OK) {
#if MYDEBUG > 0
		cerr << "Error gathering number of output channels!";
#endif
		return false;
	}

	// Now it's time to play with the buffers ...
	bufferVars.numInitedChannels = nOutput; // number of channels to init
	bufferVars.bufferInfos = new ASIOBufferInfo[bufferVars.numInitedChannels];

	for (int i = 0; i < bufferVars.numInitedChannels; i++) {
		bufferVars.bufferInfos[i].isInput = ASIOFalse; // we're init'ing the output channels
		bufferVars.bufferInfos[i].channelNum = (long) i;
	}

	bufferVars.bufferSize = bufferSize;

	// Set the callbacks
	bufferVars.callbacks = new ASIOCallbacks;
	bufferVars.callbacks->bufferSwitch         = &_bufferSwitch;
	bufferVars.callbacks->sampleRateDidChange  = &_sampleRateDidChange;
	bufferVars.callbacks->asioMessage          = &_asioMessage;
	bufferVars.callbacks->bufferSwitchTimeInfo = &_bufferSwitchTimeInfo;

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::createOutputBuffers - before buffers created" << "\n";
  mout->flush();
#endif
	
	// And create the actual buffers
	errorCode = ASIOCreateBuffers(bufferVars.bufferInfos, (long) bufferVars.numInitedChannels, (long) bufferVars.bufferSize, bufferVars.callbacks);

	if (errorCode != ASE_OK) {
#if MYDEBUG > 0
		cerr << "Error creating buffers!" << endl;
#endif
		return false;
	}

	// Set the status appropriately
	status = BUFFERED;

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::createOutputBuffers - almost done (status=" << status << ")\n";
  mout->flush();
#endif

	return true;
}

/**
 * If the device is loaded, this method will gather info on
 * all of the input and output channels and display it. It is
 * useful for debugging purposes on the ASIO device.
 */
void AsioWave::displayChannelInfo ()
{
	if (status < LOADED)
		throw "Exception: Must load before displaying channels!";

	// Count the channels
	long numInputs;
	long numOutputs;
	ASIOError errorCode = ASIOGetChannels(&numInputs, &numOutputs);

	if (errorCode == ASE_OK) {
		cout << "Input channels: " << numInputs << "\tOutput channels: " << numOutputs << endl;
	} else {
		cout << "Could not get input / output channels!" << endl;
		return;
	}

	// Get the channel info for the channels - inputs first, then outputs
	ASIOChannelInfo channelInfo;

	for (int i = 0; i < numInputs; i++) {
		channelInfo.channel = (long) i;
		channelInfo.isInput = ASIOTrue;

		errorCode = ASIOGetChannelInfo(&channelInfo);

		if (errorCode == ASE_OK) {
			cout << "Input # " << i << " isActive: " << (channelInfo.isActive == ASIOTrue ? "yes" : "no")
				<< " group: " << channelInfo.channelGroup << " type: " << channelInfo.type
				<< " name: " << channelInfo.name << endl;
		} else {
			cerr << "Error getting channel info - input channel # " << i << endl;
		}
	}

	for (int i = 0; i < numOutputs; i++) {
		channelInfo.channel = (long) i;
		channelInfo.isInput = ASIOFalse;

		errorCode = ASIOGetChannelInfo(&channelInfo);

		if (errorCode == ASE_OK) {
			cout << "Output # " << i << " isActive: " << (channelInfo.isActive == ASIOTrue ? "yes" : "no")
				<< " group: " << channelInfo.channelGroup << " type: " << channelInfo.type
				<< " name: " << channelInfo.name << endl;
		} else {
			cerr << "Error getting channel info - input channel # " << i << endl;
		}
	}
}

/**
 * Returns the buffer sizes requested by this device, into the
 * array passed as an argument. The passed array must have a size
 * of >= 3. Into this array will be: [0] min-size, [1] max-size, 
 * [2] preferred-size. This method will return true if all of
 * that info is populated, or false otherwise.
 */
bool AsioWave::getBufferSizes (long* results)
{
	if (status < LOADED)
		throw "Exception: Must load before getting buffer sizes";

	long minSize;
	long maxSize;
	long preferredSize;
	long granularity;

	ASIOError errorCode = 
		ASIOGetBufferSize(&minSize, &maxSize, &preferredSize, &granularity);

	if (errorCode != ASE_OK) {
#if MYDEBUG > 1
		cerr << "Error: could not gather buffer sizes";
#endif 
		return false;
	}

	results[0] = minSize;
	results[1] = maxSize;
	results[2] = preferredSize;

	return true;
}	

/** 
 * Static method to return the names of all of the drivers
 * on the ASIO system. The names are stored in the passed string
 * array, from 0 ... maxNames (which should be the length of the
 * array). This method returns the number of driver names stored.
 */
int AsioWave::getDriverNames (string* strNames, long maxNames) 
{
	// Declare arrays for retrieving names
	char** names = new char*[maxNames];
	for (int i = 0; i < maxNames; i++)
		names[i] = new char[32]; // max LEN 32 characters

	long numNames = asioDrivers->getDriverNames(names, maxNames);

	// Convert to strings, store in arg array
	for (int i = 0; i < numNames; i++)
		strNames[i] = (string)names[i];

	// Clean up declared arrays
	for (int i = 0; i < maxNames; i++)
		delete names[i];
	delete[] names;

	return (int)numNames;
}

/**
 * Returns the current sample rate of the device, if
 * the device is loaded.
 */
ASIOSampleRate AsioWave::getSampleRate ()
{
	if (status < LOADED)
		throw "Exception: Must load before getting rate";

    ASIOSampleRate sampleRate;
	ASIOError errorCode = ASIOGetSampleRate(&sampleRate);

	if (errorCode == ASE_OK) {
		return sampleRate;
		
	} else if (errorCode == ASE_NoClock) {
#if MYDEBUG > 0
		cerr << "Sample rate not present or unknown" << endl;
#endif
		return NULL;

	} else if (errorCode == ASE_NotPresent) {
#if MYDEBUG > 0
		cerr << "No input or output present" << endl;
#endif
		return NULL;
	} else {
#if MYDEBUG > 0
		cerr << "No error code provided!" << endl;
#endif
		return NULL;
	}
}

/**
 * If the device is loaded and buffered, then the device
 * is considered 'ready.' 
 *
 * @return True if the device is loaded and buffered, false otherwise
 */
bool AsioWave::isReady ()
{
#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::isReady - start method (status=" << status << ")\n";
  mout->flush();
#endif
	return (status == BUFFERED);
}

/**
 * Loads the driver given by the parameter name. The driver is
 * both loaded from the AsioDrivers class, and initialized
 * by the AsioDriver methods.
 *
 * @return True if the init is successful, false otherwise
 */
bool AsioWave::loadDriver (string* driver)
{
#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::loadDriver (" << driver->c_str() << ") - start method" << "\n";
  mout->flush();
#endif
	bool isLoaded = asioDrivers->loadDriver((char*)driver->c_str());

	if (!isLoaded) {
#if MYDEBUG > 0
		cerr << "Error loading ASIO driver: " << *driver << endl;
#endif
		return false;
	}

	// Init the selected driver
	ASIODriverInfo asioDriverInfo = {0};
	asioDriverInfo.asioVersion = 2L;

	ASIOError errorCode = ASIOInit(&asioDriverInfo);

	if (errorCode == ASE_OK) {

#if MYDEBUG > 0
		cout << "Successfully initialized ASIO driver ..." << endl;
#endif

		status = LOADED;

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::loadDriver - almost done (status=" << status << ")\n";
  mout->flush();
#endif

		return true;

	} else {

#if MYDEBUG > 0
		cerr << "Error initializing driver ... "
			<< asioDriverInfo.errorMessage << endl;
#endif

		return false;
	}
}

/**
 * Plays the full sound, specified by the parameters passed to this
 * method (including amplitude, frequency, duration, rampUpStop,
 * rampDownStart, and out of which sides to play.
 *
 * @return True if the sound is played correctly, false otherwise
 */
bool AsioWave::playSound (float amp, float freq, float dur,
	float rampUpStop, float rampDownStart, bool playR, bool playL)
{

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::playSound - start method (amp=" << amp << ",freq=" 
          << freq << ",R=" << playR << ",L=" << playL << ",status=" << status << ")\n";
  mout->flush();
#endif

	if (status != BUFFERED)
		throw "Error: must buffer before we can play a tone!";

	// Set up output buffer
	output = new float[bufferVars.bufferSize];

	ASIOSampleRate sndSampleRate = this->getSampleRate();

	this->sndAmp      = amp;
	this->sndFreq     = freq;
	this->sndDurSec   = dur;
	this->sndRampUp   = rampUpStop;
	this->sndRampDown = rampDownStart;
	
	this->playR = playR;
	this->playL = playL;

	this->sndDurSamples = (long)floor(sndDurSec * (double)sndSampleRate + 0.5);
	
	// Create the full wave here, before beginning playback
	fullOutput = new float[this->sndDurSamples];
	formFullWave(sndSampleRate);

#if MYDEBUG > 3
	cout << "Writing full waveform to wave_output.txt" << endl;
	writeFullWave(new string("wave_output.txt"));
#endif

	// Start!
	ASIOError errorCode = ASIOStart();
	if (errorCode != ASE_OK) {
#if MYDEBUG > 0
		cerr << "Error starting playback!" << endl;
#endif
		return false;
	}

	// Sleep for dur (sec) * 1000 (for msec)
	Sleep((long)floor(sndDurSec*1000+0.5)+EXTRA_SLEEP);

	// And stop ...
	errorCode = ASIOStop();
	if (errorCode != ASE_OK) {
#if MYDEBUG > 0
		cerr << "Error stopping playback!" << endl;
#endif
		return false;
	}

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::playSound - done playing!" << "\n";
  mout->flush();
#endif

	// Clean up output buffers that we create
	delete fullOutput;
        delete output;

	return true;
}

/**
 * Registers the current thread on the COM system by calling
 * CoInitialize. This must be done for every thread that would
 * like access to the COM system. Calling this method
 * multiple times on the same thread has no effect.
 */
void AsioWave::registerThread ()
{

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::registerThread - before calling CoInitialize(NULL)" << "\n";
  mout->flush();
#endif

	CoInitialize(NULL);

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::registerThread - after calling CoInitialize(NULL)" << "\n";
  mout->flush();
#endif
}

/**
 * Sets the sample rate on the loaded ASIO device. First checks
 * to make sure this device is able to use that sample rate. If it
 * is, then the sample rate is switched. The rate being used following
 * the call of this method is the rate returned (new if change was 
 * successful, or old rate if change was not successful).
 */
ASIOSampleRate AsioWave::setSampleRate (ASIOSampleRate rate)
{
	if (status < LOADED)
		throw "Exception: Must load before setting rate";

	ASIOSampleRate oldRate = this->getSampleRate();

	ASIOError errorCode = ASIOCanSampleRate(rate);

	if (errorCode != ASE_OK) {
#if MYDEBUG > 0
		cerr << "Error: cannot use this sample rate!" << endl;
#endif 
		return oldRate;
	}

	errorCode = ASIOSetSampleRate(rate);

	if (errorCode != ASE_OK) {
#if MYDEBUG > 0
		cerr << "Error setting sample rate: failed" << endl;
#endif
		return oldRate;
	}

	return rate;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

/**
 * Generates the full wave into the fullOutput buffer. This way
 * of presenting the sound involves generating the full wave
 * a priori, and then during playback simply copying the relevant
 * sections of the wave into one of the buffers (in O(n) time). This 
 * yeilds a very efficient presentation which is not (as much) reliant
 * on a fast processor to compute sin(float) many times during the 
 * buffering process.
 */
void AsioWave::formFullWave (ASIOSampleRate sndSampleRate)
{
#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "AsioWave::formFullWave - start method" << "\n";
  mout->flush();
#endif

        // Make sure the amplitude doesn't exceed the max amp, to prevent clipping
        if (sndAmp > MAX_SND_AMP)
          sndAmp = MAX_SND_AMP;

	// Generate Sin Wave
	for (int i = 0; i < sndDurSamples; i++)
		fullOutput[i] = (sndAmp)*(float) sin(TwoPI * (float)i * sndFreq / (double)sndSampleRate);

	// Modify with ramp-up component
	long rampUpEndSamples = (long)floor(sndRampUp * (double)sndSampleRate + 0.5);
    for (int i = 0; i < rampUpEndSamples; i++)
		fullOutput[i] = fullOutput[i] * (float)i / (float)rampUpEndSamples;

	// Modify with ramp-down component
	long rampDownStartSamples = (long)floor(sndRampDown * (double)sndSampleRate + 0.5);
	for (int i = rampDownStartSamples; i < sndDurSamples; i++)
		fullOutput[i] = fullOutput[i] * (float)(sndDurSamples - i) / 
			(float)(sndDurSamples - rampDownStartSamples);
}

/**
 * Debugging method to write a full wave to the specified output
 * string. Returns boolean true when the wave is written.
 */
bool AsioWave::writeFullWave (string* file)
{
	fstream filestr (file->c_str(), fstream::out);

	for (int i = 0; i < sndDurSamples; i++)
		filestr << fullOutput[i] << endl;

	filestr.close();

	return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// CALLBACK MEMBER METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

ASIOTime* AsioWave::bufferSwitchTimeInfo (ASIOTime* asioTime, long bufferIndex, ASIOBool directProcess) 
{
//	cout << "CALLBACK: bufferSwitchTimeInfo fired" << endl;

	// Get the SystemTime and the SampePosition
	long systemTime = (long)ASIO64toLong(asioTime->timeInfo.systemTime);
	long samplePos = (long)ASIO64toLong(asioTime->timeInfo.samplePosition);

#if MYDEBUG > 3
	cout << "BufferSwitch: samplePos: " << samplePos << endl;
#endif
	
	// O(n) through buffer to zero it out
    for (int i = 0; i < bufferVars.bufferSize; i++)
		output[i] = 0.F;

	// And at most O(n) again through buffer, copying from full output
	int index = 0;
//	for (int i = samplePos-bufferVars.bufferSize; i < MIN(samplePos,sndDurSamples); i++)
//		output[index++] = fullOutput[i];
	for (int i = samplePos; i < MIN(samplePos+bufferVars.bufferSize, sndDurSamples); i++)
		output[index++] = fullOutput[i];

	// Now fill the buffers
	int intval;
	byte* fillBuffer;

	if (this->playL) {
		fillBuffer = (byte*)bufferVars.bufferInfos[0].buffers[bufferIndex]; 

		for (int i = 0; i < bufferVars.bufferSize; i++) {
			intval = (int)floor(MAX_INT32 * output[i] + 0.5); // floor(x + 0.5) == round(x)

			fillBuffer[4*i]   = (byte)(intval & 0xFF);
			fillBuffer[4*i+1] = (byte)((intval >> 8) & 0xFF);
			fillBuffer[4*i+2] = (byte)((intval >> 16) & 0xFF);
			fillBuffer[4*i+3] = (byte)((intval >> 24) & 0xFF);
		}
	}

	if (this->playR) {
		fillBuffer = (byte*)bufferVars.bufferInfos[1].buffers[bufferIndex]; 

		for (int i = 0; i < bufferVars.bufferSize; i++) {
			intval = (int)floor(MAX_INT32 * output[i] + 0.5); // floor(x + 0.5) == round(x)
			
			fillBuffer[4*i]   = (byte)(intval & 0xFF);
			fillBuffer[4*i+1] = (byte)((intval >> 8) & 0xFF);
			fillBuffer[4*i+2] = (byte)((intval >> 16) & 0xFF);
			fillBuffer[4*i+3] = (byte)((intval >> 24) & 0xFF);
		}
	}

	return asioTime;
}

void AsioWave::bufferSwitch (long bufferIndex, ASIOBool directProcess) {
//	cout << "CALLBACK: bufferSwitch fired" << endl;

	ASIOTime asioTime = {0};
	ASIOError errorCode = ASIOGetSamplePosition(&asioTime.timeInfo.samplePosition, &asioTime.timeInfo.systemTime);
	if (errorCode == ASE_OK) {
		asioTime.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
	}
	bufferSwitchTimeInfo(&asioTime, bufferIndex, directProcess);
}

void AsioWave::sampleRateDidChange (ASIOSampleRate sampleRate) 
{
#if MYDEBUG > 0
	cout << "CALLBACK: sampleRateDidChange fired" << endl;
#endif
}

long AsioWave::asioMessage (long selector, long value, void* message, double* opt)
{
#if MYDEBUG > 0
	cout << "CALLBACK: asioMessage fired" << endl;
#endif

	return 1L;
}

/** * * * * * * * * * * * * * * * * * * * * * * * 
* Non-Class Functions -- For Callbacks
* * * * * * * * * * * * * * * * * * * * * * * * **/

void _bufferSwitch (long bufferIndex, ASIOBool directProcess) 
{
	_instance->bufferSwitch(bufferIndex,directProcess);
}

void _sampleRateDidChange (ASIOSampleRate sampleRate) 
{
	_instance->sampleRateDidChange(sampleRate);
}

long _asioMessage (long selector, long value, void* message, double* opt) 
{	  
	return _instance->asioMessage(selector,value,message,opt);
}

ASIOTime* _bufferSwitchTimeInfo (ASIOTime* asioTime, long bufferIndex, ASIOBool directProcess) 
{
	return _instance->bufferSwitchTimeInfo(asioTime,bufferIndex,directProcess);
}

