#include "stdio.h"
#include "time.h"

/**
 * Main function executed in this file.
 */
int main (int argc, char* argv[]) {

  // Declare variables
  char tmpbuf[128], ampm[] = "AM";
  time_t ltime;
  errno_t err;
  struct tm today;
  char frmtStr[64];

  // If no parameters passed, use the default format
  // for the output string; else use the passed format.
  if (argc <= 1)
    strcpy_s( frmtStr, sizeof(frmtStr), "set logfile=log_%Y_%m_%d.txt\n");
  else
    strcpy_s( frmtStr, sizeof(frmtStr), argv[1] );

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
  if ( today.tm_hour >= 12 ) {
    strcpy_s( ampm, sizeof(ampm), "PM" );
    today.tm_hour -= 12;
  }
  if ( today.tm_hour == 0 )  // Adjust if midnight hour.
    today.tm_hour = 12;

  // Use strftime to build a customized time string
  strftime( tmpbuf, 128, frmtStr, &today );
  printf( tmpbuf );

}