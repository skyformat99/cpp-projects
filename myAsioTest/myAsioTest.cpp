//#include <stdio.h>
//#include <string.h>
#include <iostream>
#include <math.h>
#include "asiosys.h"
#include "asio.h"
#include "asiodrivers.h"

using namespace std;

// Function definitions for callbacks
ASIOTime* bufferSwitchTimeInfo (ASIOTime* , long , ASIOBool );
void bufferSwitch (long , ASIOBool );
void sampleRateDidChange (ASIOSampleRate );
long asioMessage (long , long , void* , double* );

// BufferVars struct
typedef struct BufferVars {
	ASIOBufferInfo *bufferInfos;
	int numInitedChannels; // length of bufferInfos array
	int bufferSize;
	ASIOCallbacks *callbacks;
	long samplePosition;
} BufferVars;
BufferVars bufferVars = {0};

#if NATIVE_INT64
	#define ASIO64toLong(a)  (a)
#else
	#define ASIO64toLong(a)  (((unsigned long long int) a.hi) << 32) | (unsigned long long int) a.lo
#endif

float* output;

float sndAmp;
float sndFreq;
float sndDur;
long sndBufferSize;
ASIOSampleRate sndSampleRate;
float sndRampUp;
float sndRampDown;

#define TwoPI 6.283185307
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

// Sets the max float values
float MAX_INT16 = (float) 0x00007FFF;
float MAX_INT18 = (float) 0x0001FFFF;
float MAX_INT20 = (float) 0x0007FFFF;
float MAX_INT24 = (float) 0x007FFFFF;
float MAX_INT32 = (float) 0x7FFFFFFF;

void main (int argc, char* argv[])
{
	if (argc < 4) {
		cerr << "Usage: myAsioTest amp freq dur rampStart rampEnd [bufferSize] [sampleRate]" << endl;
		exit(-1);
	}

	sndAmp  = (float)atof(argv[1]);
	sndFreq = (float)atof(argv[2]);
	sndDur  = (float)atof(argv[3]);

	if (argc >= 6) {
		sndRampUp = (float)atof(argv[4]);
		sndRampDown = (float)atof(argv[5]);
	}

	if (argc >= 7)
		sndBufferSize = (long)atol(argv[6]);
	else
		sndBufferSize = -1L;

	if (argc >= 8)
		sndSampleRate = (ASIOSampleRate)atof(argv[7]);
	else
		sndSampleRate = (ASIOSampleRate)48000.;

	cout << "Enter driver # to use:" << endl;

	CoInitialize(NULL);

	AsioDrivers* asioDrivers = new AsioDrivers();

	long maxNames = 32L;
	char** names = new char*[maxNames];
	for (int i = 0; i < maxNames; i++)
		names[i] = new char[32];

	long numNames = asioDrivers->getDriverNames(names, maxNames);

	for (int i = 0; i < numNames; i++)
		cout << "  " << i << ": " << names[i] << endl;

	int toUse;

	if (numNames == 0) {
		cerr << "Error: no drivers available!" << endl;
		exit(-1);
	} else if (numNames == 1) {
		toUse = 0;
	} else {
		cin >> toUse;
	}

	cout << "Using driver " << names[toUse] << endl;

	// Load the selected driver
	bool isLoaded = asioDrivers->loadDriver(names[toUse]);

	cout << "isLoaded? " << (isLoaded ? "true" : "false") << endl;

	for (int i = 0; i < maxNames; i++)
		delete names[i];
	delete[] names;

	// Init the selected driver
	ASIODriverInfo asioDriverInfo = {0};
	asioDriverInfo.asioVersion = 2L;

	ASIOError errorCode = ASIOInit(&asioDriverInfo);

	if (errorCode == ASE_OK) 
		cout << "Successfully initialized ASIO driver ..." << endl;
	else
		cerr << "Error initializing driver ... " << asioDriverInfo.errorMessage << endl;
	
	// Count the channels
	long numInputs;
	long numOutputs;
	errorCode = ASIOGetChannels(&numInputs, &numOutputs);

	if (errorCode == ASE_OK) 
		cout << "Input channels: " << numInputs << "\tOutput channels: " << numOutputs << endl;
	else
		cout << "Could not get input / output channels!" << endl;

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

	// Get sample rate
	ASIOSampleRate sampleRate;
	errorCode = ASIOGetSampleRate(&sampleRate);

	if (errorCode == ASE_OK)
		cout << "Original sample rate: " << sampleRate << endl;
	else if (errorCode == ASE_NoClock)
		cout << "Sample rate not present or unknown" << endl;
	else if (errorCode == ASE_NotPresent)
		cout << "No input or output present" << endl;

	// Can it be requested sample rate?
	errorCode = ASIOCanSampleRate(sndSampleRate);
	cout << "Can sample rate be " << sndSampleRate << " Hz? " << (errorCode == ASE_OK ? "yes" : "no") << endl;

	// Set it to requested, if it can be
	if (errorCode == ASE_OK) {
		errorCode = ASIOSetSampleRate(sndSampleRate);
		if (errorCode == ASE_OK)
			cout << "Sample rate set to " << sndSampleRate << " Hz!" << endl;
	} else 
		cerr << "Error setting sample rate to requested sndSampleRate" << endl;
		
    errorCode = ASIOGetSampleRate(&sndSampleRate);
	if (errorCode != ASE_OK)
		cerr << "Error getting sampleRate for class variable" << endl;

	long minSize;
	long maxSize;
	long preferredSize;
	long granularity;
	errorCode = ASIOGetBufferSize(&minSize, &maxSize, &preferredSize, &granularity);
	if (errorCode == ASE_OK)
		cout << "Buffer sizes: min: " << minSize << " max: " << maxSize 
		<< " pref: " << preferredSize << " granularity: " << granularity << endl;
	else
		cerr << "Error getting buffer sizes!" << endl;

	// Now it's time to play with the buffers ...
	bufferVars.numInitedChannels = 2; // number of channels to init
	bufferVars.bufferInfos = new ASIOBufferInfo[bufferVars.numInitedChannels];
	ASIOSampleType* sampleTypes = new ASIOSampleType[bufferVars.numInitedChannels];

	for (int i = 0; i < bufferVars.numInitedChannels; i++) {
		bufferVars.bufferInfos[i].isInput = ASIOFalse; // we're init'ing the output channels
		bufferVars.bufferInfos[i].channelNum = (long) i;
		sampleTypes[i] = channelInfo.type;
	}

	if (sndBufferSize == -1L)
		bufferVars.bufferSize = (int) preferredSize;
	else
		bufferVars.bufferSize = (int) sndBufferSize;

	bufferVars.callbacks = new ASIOCallbacks;
	bufferVars.callbacks->bufferSwitch         = &bufferSwitch;
	bufferVars.callbacks->sampleRateDidChange  = &sampleRateDidChange;
	bufferVars.callbacks->asioMessage          = &asioMessage;
	bufferVars.callbacks->bufferSwitchTimeInfo = &bufferSwitchTimeInfo;
	
	errorCode = ASIOCreateBuffers(bufferVars.bufferInfos, (long) bufferVars.numInitedChannels, (long) bufferVars.bufferSize, bufferVars.callbacks);

	if (errorCode == ASE_OK)
		cout << "Created buffers successfully!" << endl;
	else
		cerr << "Error creating buffers ... " << endl;

	// Set up output buffer
	output = new float[bufferVars.bufferSize];
	cout << "Initialized output buffer at size = " << bufferVars.bufferSize << endl;
	cout << "numInitedChannels = " << bufferVars.numInitedChannels << endl;

	// Start!
	errorCode = ASIOStart();
	if (errorCode == ASE_OK)
		cout << "Started ok!" << endl;
	
	// Sleep for dur (sec) * 1000 (for msec)
	Sleep((long)floor(sndDur*1000+0.5));

	// And stop ...
	errorCode = ASIOStop();
	if (errorCode == ASE_OK)
		cout << "Stopped ok!" << endl;
		
	// Free resources and dispose of buffers before quitting
	delete sampleTypes;
	delete bufferVars.bufferInfos;
	delete bufferVars.callbacks;

	errorCode = ASIODisposeBuffers();

	// Unload driver
	asioDrivers->removeCurrentDriver();

	// Exit ASIO
	//ASIOExit();

	exit(0);
}

ASIOTime* bufferSwitchTimeInfo(ASIOTime* asioTime, long bufferIndex, ASIOBool directProcess) {
	
	// Get the SystemTime and the SampePosition
	long systemTime = (long)ASIO64toLong(asioTime->timeInfo.systemTime);
	long samplePos = (long)ASIO64toLong(asioTime->timeInfo.samplePosition);

	// Generate Sin Wave
	for (int i = 0; i < bufferVars.bufferSize; i++)
		output[i] = (sndAmp)*(float) sin(TwoPI * (samplePos+i) * sndFreq / (double)sndSampleRate);

	int index = 0;
	long rampUpEndSamples = (long)floor(sndRampUp * (double)sndSampleRate + 0.5);
	for (int i = samplePos; i < MIN(samplePos+bufferVars.bufferSize,rampUpEndSamples); i++)
		output[index] = output[index++] * (float)i / (float)rampUpEndSamples;

	index = 0;
	long rampDownStartSamples = (long)floor(sndRampDown * (double)sndSampleRate + 0.5);
	long durSamples = (long)floor(sndDur * (double)sndSampleRate + 0.5);
	for (int i = MAX(samplePos,rampDownStartSamples); i < samplePos+bufferVars.bufferSize; i++)
		output[index] = output[index++] * (float)(MAX(durSamples,samplePos+bufferVars.bufferSize) - i) / 
		    (float)(MAX(durSamples,samplePos+bufferVars.bufferSize) - rampDownStartSamples);

	int intval;
	byte* fillBuffer;
	for (int j = 1; j < 2; j++) {

		fillBuffer = (byte*)bufferVars.bufferInfos[j].buffers[bufferIndex]; 

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

void bufferSwitch (long bufferIndex, ASIOBool directProcess) {
	ASIOTime asioTime = {0};
	ASIOError errorCode = ASIOGetSamplePosition(&asioTime.timeInfo.samplePosition, &asioTime.timeInfo.systemTime);
	if (errorCode == ASE_OK) {
		asioTime.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
	}
	bufferSwitchTimeInfo(&asioTime, bufferIndex, directProcess);
}

void sampleRateDidChange (ASIOSampleRate sampleRate) {
	cout << "CALLBACK: sampleRateDidChange fired" << endl;
}

long asioMessage (long selector, long value, void* message, double* opt) {	  
	cout << "CALLBACK: asioMessage fired" << endl;

	return 1L;
}
