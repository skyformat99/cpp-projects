#define _CRT_RAND_S

#include <stdio.h>
#include <stdlib.h>

// REMOVED MACRO definition
// #define PLAYLIST "E:/Downloads/Mp3/Playlists/Christmas Random.m3u"

#define MAX_SONGS 3000
#define MAX_FILENAME_LEN 256

#define DEBUG false

/** Determines if a number is present in the array (bounds specified) */
bool numInArray (int, int*, int, int);

int main (int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: randomizePlaylist \"c:/path to playlist/playlist.m3u\"\n");
    exit(1);
  }

  // Declare var's
  int i, numFiles = 0;
  char** filenames;
  FILE * pFile;
  char headerString[256];
  char* result;
  int *mapping, *mapping_i;
  errno_t err;
  unsigned int number;
  int newNumber;

  // Declare char array for filenames
  filenames = (char**)malloc((size_t)MAX_SONGS*sizeof(char*));
  for (i=0;i<MAX_SONGS;i++)
    filenames[i] = (char*)malloc((size_t)MAX_FILENAME_LEN*sizeof(char));

  // Open file, read in names
  fopen_s(&pFile,argv[1],"r");
  if (pFile == NULL) {
    perror("Error opening file");
    exit(1);
  }

  result = fgets(headerString, 256, pFile);
  while (result != NULL)
    result = fgets(filenames[numFiles++],MAX_FILENAME_LEN,pFile);
  numFiles--;

  fclose (pFile);

  if (DEBUG) 
    fprintf(stdout,"Number of files read in: %d\n",numFiles);

  mapping = (int*)malloc((size_t)numFiles*sizeof(int));
  mapping_i = (int*)malloc((size_t)numFiles*sizeof(int));
  
  // Generate the random mappings to move old files to new ones
  for (i=0;i<numFiles;i++) {

    err = rand_s(&number);
    newNumber = (int)((double)number/(double)UINT_MAX * (double)numFiles);

    while (numInArray(newNumber,mapping,0,i)) {
      err = rand_s(&number);
      newNumber = (int)((double)number/(double)UINT_MAX * (double)numFiles);
    }

    mapping[i] = newNumber;
    mapping_i[newNumber] = i;
  }

  if (DEBUG) {
    fprintf(stdout,"New Mapping:\n");
    for (i=0;i<numFiles;i++) 
      fprintf(stdout,"  [%0.3d]: %0.3d (%0.3d)\n",i,mapping[i],mapping_i[mapping[i]]);
  }

  // Now re-open file to write
  // Open file, read in names
  fopen_s(&pFile,argv[1],"w");
  if (pFile == NULL) {
    perror("Error opening file");
    exit(1);
  }

  fputs(headerString,pFile);
  for (i=0;i<numFiles;i++)
    fputs(filenames[mapping_i[i]],pFile);

  fclose (pFile);

  // Close up and free up stuff
  for (i=0;i<MAX_SONGS;i++)
    free((char*)filenames[i]);
  free((char*)filenames);
  free((char*)mapping);
  
  return 0;
}

bool numInArray (int val, int* mapping, int l, int h) {
  int i;
  for (i=l;i<h;i++) {
    if (mapping[i] == val) return true;
  }

  return false;
}