#include "stdio.h"
#include "windows.h"
#include "time.h"

#define DEBUG false

/** Define the helper functions ahead of time **/
void getFileName (char*,int,char*);
void playWave (char*,int,int);
void recordWave (char*,int);

/**
 * Main function executed in this file.
 */
int main (int argc, char* argv[]) {

  char fileName[256];

  /** Get prefix from arguments, or use default "" **/
  if (argc < 2)
    getFileName(fileName,256,"");
  else
    getFileName(fileName,256,argv[1]);

  printf("Saved wave to %s\n",fileName);

//  playWave ("c:/testWave.wav",1000,2000);
  recordWave (fileName,5000);

  exit(0);
}

/**
 * Fills the passed character buffer with the filename
 * to be used. Prefixed by the appropriate prefix.
 */
void getFileName (char* buf, int bufSize, char* prefix) {

  char tmpbuf[128], ampm[] = "AM";
  time_t ltime;
  errno_t err;
  struct tm today;

  // Get UNIX time
  time( &ltime );

  // Convert to local time structure
  err = _localtime64_s( &today, &ltime );
  if (err)
  {
    printf("_localtime64_s failed due to an invalid argument.");
    exit(1);
  }

  // Adjust for AM/PM, and adjust if midnight hour
//  if ( today.tm_hour >= 12 ) {
//    strcpy_s( ampm, sizeof(ampm), "PM" );
//    today.tm_hour -= 12;
//  }
//  if ( today.tm_hour == 0 )  // Adjust if midnight hour.
//    today.tm_hour = 12;

  // Use strftime to build a customized time string
  strftime( tmpbuf, 128, "aae_%Y-%m-%d_%H-%M.wav", &today );

  // And add on prefix
  sprintf_s(buf,bufSize,"%s%s",prefix,tmpbuf);
}

/**
 * Plays a given file from start time to end time in ms.
 */
void playWave (char* waveFile, int startMS, int endMS) {

  MCIERROR err;
  char errStrBuf[256];
  char sendStrBuf[256];

  sprintf_s(sendStrBuf,256,
    "open \"%s\" alias audio1",waveFile);
  err = mciSendString(sendStrBuf, 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'open', err = %d\n",err);
  if (err != 0) {
    mciGetErrorString(err,errStrBuf,256);
    printf("Error: %s\n",errStrBuf);
  }

  err = mciSendString("set audio1 time format milliseconds", 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'set', err = %d\n",err);

  sprintf_s(sendStrBuf,256,
    "play audio1 from %d to %d wait",startMS,endMS);  
  err = mciSendString(sendStrBuf, 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'play', err = %d\n",err);
  if (err != 0) {
    mciGetErrorString(err,errStrBuf,256);
    printf("Error: %s\n",errStrBuf);
  }

  err = mciSendString("close audio1", 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'close', err = %d\n",err);

  return;
}

/**
 * Record from the default wave source for a given length (in ms),
 * and save the results in the specified filename.
 */
void recordWave (char* filename, int lengthMS) {

  MCIERROR err;
  char errStrBuf[256];
  char sendStrBuf[256];

  err = mciSendString("open new type waveaudio alias audio1 buffer 6", 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'open', err = %d\n",err);
  if (err != 0) {
    mciGetErrorString(err,errStrBuf,256);
    printf("Error on open:\n\t%s\n",errStrBuf);
  }

//  err = mciSendString("set audio1 bytespersec 88200", 
//          NULL,0,NULL);
//  err = mciSendString("set audio1 channels 1", 
//          NULL,0,NULL);
//  err = mciSendString("set audio1 samplespersec 44100", 
//          NULL,0,NULL);

  err = mciSendString("set audio1 format tag pcm", 
          NULL,0,NULL);

  err = mciSendString("set audio1 time format milliseconds", 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'set', err = %d\n",err);

  sprintf_s(sendStrBuf,256,
    "record audio1 from 0 to %d wait",lengthMS);
  err = mciSendString(sendStrBuf, 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'record', err = %d\n",err);
  if (err != 0) {
    mciGetErrorString(err,errStrBuf,256);
    printf("Error on record:\n\t%s\n",errStrBuf);
  }

  sprintf_s(sendStrBuf,256,
    "save audio1 \"%s\" wait",filename);
  err = mciSendString(sendStrBuf, 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'save', err = %d\n",err);
  if (err != 0) {
    mciGetErrorString(err,errStrBuf,256);
    printf("Error on save:\n\t%s\n",errStrBuf);
  }

  err = mciSendString("close audio1", 
          NULL,0,NULL);

  if (DEBUG) printf("Sent 'close', err = %d\n",err);

  exit(0);
}