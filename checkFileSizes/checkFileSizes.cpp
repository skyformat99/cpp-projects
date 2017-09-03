#include <iostream>

using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

off_t getFileSize (char*);
void printUsage ();

void main (int argc, char* argv[])
{

  if (argc < 3) {
    printUsage();
    exit(-1);
  }

  double fudgeFactor = 0.;
  try {
    if (argc > 3)
      fudgeFactor = atof(argv[3]);
  } catch (...) {
    printf("Error converting fudgeFactor to a double value!\n");
    printUsage();
    exit(-1);
  }

  long fSize1, fSize2;
  fSize1 = getFileSize(argv[1]);
  fSize2 = getFileSize(argv[2]);

#ifdef DEBUG
  printf("fSize1: %d\tfSize2: %d\n",fSize1,fSize2);
#endif

  double highSize = (double)fSize1 + (double)fSize1 * fudgeFactor;
  double lowSize  = (double)fSize1 - (double)fSize1 * fudgeFactor;

#ifdef DEBUG
  printf("highSize: %f\tlowSize: %f\n",highSize,lowSize);
#endif

  if ((fSize2 <= highSize) &&
      (fSize2 >= lowSize))
    printf("true");
  else
    printf("false");

  exit(0);
}

/**
 * Returns the file size of a given file, or exits
 * with a thrown error.
 */
off_t getFileSize (char* file)
{
  struct stat filestatus;
  stat(file, &filestatus);

  return filestatus.st_size;

}

/**
 * Prints the usage for this program.
 */
void printUsage ()
{
  printf("Usage: checkFileSizes <file1> <file2> [<fudgeFactor>]\n\n");
  printf("  <file1>  The absolute file name of the first (reference) file\n");
  printf("  <file2>  The absolute file name of the second (comparison) file\n\n");
  printf("  <fudgeFactor>  Optional parameter; if the comparison file is within\n");
  printf("                 +- the fudgeFactor percent of the reference file \n");
  printf("                 size, then they are considered equal.\n\n");
  printf("                 For example, if the reference file size is 1000 bytes,\n");
  printf("                 the comparison file size is 900 bytes, and the fudge \n");
  printf("                 factor is 0.11 (or 11%) then since 890 < 900 < 1110, \n"); 
  printf("                 these two would be considered equal.\n\n");
  printf("  Prints 'true' if they are equal, 'false' if not.\n");

  return;
}