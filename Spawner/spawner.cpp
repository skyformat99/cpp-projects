#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "Windows.h"
#include "Winbase.h"

#define DEBUG false
#define PATH_BUFF 1024

/**
 * Main function executed in this file.
 */
int main (int argc, char* argv[]) {

  // If at least one argument isn't included, exit
  if (argc < 2) {
    printf("Usage: spawner <command> [<arg1> <arg2> ... <argN>]\n");
    exit(1);
  }

  // Debug output the command and argument
  if (DEBUG) {
    printf("CMD:\t%s\n",argv[1]);
    printf("ARG:\t%s\n",argv[2]);
  }

  // Copy the arguments into a path buffer
  char path[PATH_BUFF];
  strcpy_s(path,argv[1]);

  int i = 2;
  while (i < argc) {
    if (DEBUG) printf("i: %d argc: %d argv: %s\n",i,argc,argv[i]);
    strcat_s(path," \0");
    strcat_s(path,argv[i]);
    i++;
  }

  // Output the path buffer
  if (DEBUG) printf("PATH:\t%s\n",path);
  
  // Declare the two necessary pointers
  STARTUPINFO* sinfo = new STARTUPINFO();
  PROCESS_INFORMATION* pid = new PROCESS_INFORMATION();

  // Create the process
  BOOL val = CreateProcess(NULL,path,NULL,NULL,
               FALSE,DETACHED_PROCESS,NULL,NULL,sinfo,pid);

  // Output the processID
  printf("PID:\t%d\n",pid->dwProcessId);

  // Print out the return value, and the error code if necessary
  if (DEBUG) printf("RTN:\t%d\n",val);

  if (val == 0 && DEBUG)
    printf("ERR:\t%d\n",GetLastError());

  exit(0);
}