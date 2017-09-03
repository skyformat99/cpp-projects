#include <jni.h>
#include <iostream>
#include "JAsioWave.h"
#include "AsioWave.h"

#ifdef MYTXTDEBUG
  #include "AsioDebugger.h"
#endif

using namespace std;

// global variables
AsioWave* asio;

#define JNI_VERSION JNI_VERSION_1_6

/**
 * Called on the loading of the JVM, to initialize the global 
 * AsioWave variable.
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *localJvm, void *reserved) {  

#ifdef MYTXTDEBUG
  AsioDebugger* mout = AsioDebugger::getAsioDebugger();
  mout->printTime();
  (*mout) << "JAsioWave::JNI_OnLoad - before creating asio" << "\n";
  mout->flush();
#endif

	asio = new AsioWave(); // set the global variable

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "JAsioWave::JNI_OnLoad - after creating asio" << "\n";
  mout->flush();
#endif

	return JNI_VERSION;
}

/**
 * Called on unloading of the JVM, to delete the asio object
 * cleanly. The delete command will run the destructor on the
 * object, so all resources are freed.
 */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved) {

#ifdef MYTXTDEBUG
  AsioDebugger* mout = AsioDebugger::getAsioDebugger();
  mout->printTime();
  (*mout) << "JAsioWave::JNI_OnUnload - before deleting asio" << "\n";
  mout->flush();
#endif

  if (asio) {
    delete asio;
  }

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "JAsioWave::JNI_OnUnload - after deleting asio" << "\n";
  mout->flush();
#endif
}

/**
 * Create the buffers to prepare this Asio line for sound
 * presentation.
 */
JNIEXPORT jboolean JNICALL Java_JAsioWave__1createBuffers
  (JNIEnv *env, jobject jobj, jint bufferSize) 
{ 

#ifdef MYTXTDEBUG
  AsioDebugger* mout = AsioDebugger::getAsioDebugger();
  mout->printTime();
  (*mout) << "JAsioWave::createBuffers - method start" << "\n";
  mout->flush();
#endif

	bool isCreated;
	try {
		isCreated = asio->createOutputBuffers((int)bufferSize);
	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception creating output buffers");
	}

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "JAsioWave::createBuffers - after creating buffers" << "\n";
  mout->flush();
#endif

	return isCreated ? JNI_TRUE : JNI_FALSE;
}

/**
 * Display information on the channels for this asio device.
 * This is useful for debugging purposes.
 */
JNIEXPORT void JNICALL Java_JAsioWave__1displayChannelInfo
  (JNIEnv *env, jobject obj)
{
	try {
		asio->displayChannelInfo();
	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception displaying Channel Info");
	}
}

/**
 * Returns the buffer sizes [min, max, preferred] for this
 * open device.
 */
JNIEXPORT jlong JNICALL Java_JAsioWave__1getBufferSizes
  (JNIEnv *env, jobject jobj, jint index)
{
    long* bufSizes = new long[3];

	bool result;

	try {
		result = asio->getBufferSizes(bufSizes);
	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception gathering buffer sizes");
	}

	if (result && index >= 0 && index < 3)
		return bufSizes[index];
	else
		return -1L;
}

/**
 * Static call to get all of the driver names for available
 * drivers.
 */
JNIEXPORT jint JNICALL Java_JAsioWave__1getDriverNames
  (JNIEnv *env, jclass clazz, jobjectArray jnames)
{
	long maxNames = (long) env->GetArrayLength(jnames);
	string* devices = new string[maxNames];
	
	int numDevices;
	
	try {
		numDevices = asio->getDriverNames(devices,maxNames);
	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception getting all driver names");
	}

	for (int i = 0; i < numDevices; i++) 
		env->SetObjectArrayElement(jnames, i, env->NewStringUTF(devices[i].c_str()));
	
	delete devices;

	return (jint) numDevices;
}

/**
 * Initialize the Asio subsystem with the Asio driver given.
 */
JNIEXPORT jboolean JNICALL Java_JAsioWave__1initAsioDriver
  (JNIEnv *env, jobject jobj, jstring jdriverName)
{
	char *driverName = (char *) env->GetStringUTFChars(jdriverName, NULL);

#ifdef MYTXTDEBUG
  AsioDebugger* mout = AsioDebugger::getAsioDebugger();
  mout->printTime();
  (*mout) << "JAsioWave::initAsioDriver - method start (driverName=" << driverName << "\n";
  mout->flush();
#endif

	bool isLoaded;
	try {
		isLoaded = asio->loadDriver(new string(driverName));
	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception loading drivers");
	}

	env->ReleaseStringUTFChars(jdriverName, driverName);

	return isLoaded ? JNI_TRUE : JNI_FALSE;
}

/**
 * Determines if the line is buffered and ready for writing.
 */
JNIEXPORT jboolean JNICALL Java_JAsioWave__1isReady
  (JNIEnv *env, jobject jobj)
{
	return asio->isReady() ? JNI_TRUE : JNI_FALSE;
}

/**
 * Presents a tone of a given amp, freq, dur from the R or L
 * headphones. Returns whether or not the tone played.
 */
JNIEXPORT jboolean JNICALL Java_JAsioWave__1playSound
  (JNIEnv *env, jobject jobj, jfloat amp, 
  jfloat freq, jfloat dur, jfloat rampUp, jfloat rampDown,
  jboolean playR, jboolean playL)
{
#ifdef MYTXTDEBUG
  AsioDebugger* mout = AsioDebugger::getAsioDebugger();
  mout->printTime();
  (*mout) << "JAsioWave::playSound - method start (amp=" << (float)amp 
          << ",freq=" << (float)freq << ")" << "\n";
  mout->flush();
#endif

	bool didPlay;
	try {
		didPlay = asio->playSound((float)amp, (float)freq,
			(float)dur, (float)rampUp, (float)rampDown,
			(playR == JNI_TRUE ? true : false),
			(playL == JNI_TRUE ? true : false));
	
	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception playing sound");
	}

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "JAsioWave::playSound - after playing (didPlay=" << didPlay << ")\n";
  mout->flush();
#endif
	
	return didPlay ? JNI_TRUE : JNI_FALSE;
}

/**
 * For each thread attempting to initialize on the ASIO 
 * subsystem, this method must be called. It initializes
 * the COM subsystem on this thread.
 */
JNIEXPORT void JNICALL Java_JAsioWave__1registerThread
  (JNIEnv *env, jobject jobj) 
{

#ifdef MYTXTDEBUG
  AsioDebugger* mout = AsioDebugger::getAsioDebugger();
  mout->printTime();
  (*mout) << "JAsioWave::registerThread - method start" << "\n";
  mout->flush();
#endif

	try {
		asio->registerThread();
	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception registering thread");
	}

#ifdef MYTXTDEBUG
  mout->printTime();
  (*mout) << "JAsioWave::registerThread - method end" << "\n";
  mout->flush();
#endif
}

/**
 * Deletes the old AsioWave, cleaning the buffers and resetting
 * the parameters. Then it creates a new one, ready for business.
 */
JNIEXPORT void JNICALL Java_JAsioWave__1resetSystem
  (JNIEnv *env, jobject jobj)
{

#ifdef MYTXTDEBUG
  AsioDebugger* mout = AsioDebugger::getAsioDebugger();
  mout->printTime();
  (*mout) << "JAsioWave::resetSystem - method start" << "\n";
  mout->flush();
#endif

	if (asio) {
		delete asio;
	}

	try {
		asio = new AsioWave();

	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception creating new instance during reset");
	}
}

/**
 * Sets the sample rate if possible, and returns the sample rate
 * in use at the end of the method call.
 */
JNIEXPORT jfloat JNICALL Java_JAsioWave__1setSampleRate
  (JNIEnv *env, jobject jobj, jfloat sampleRate)
{
	ASIOSampleRate newRate;
	
	try {
		newRate = asio->setSampleRate((ASIOSampleRate)sampleRate);
	} catch (...) {
		env->ThrowNew(
			env->FindClass("JAsioException"),
			"JNI exception gathering new sample rate");
	}

	return (jfloat)newRate;
}