#include <string>
#include <iostream>
#include <sstream>
#include "Windows.h"
#include "Winbase.h"

#define DEBUG false

/**
 * Accepts arguments from the command line, and starts a forking program
 * which, at a specified interval, forks off a new process with the specified
 * path.  Continues until terminated.
 */
int main (int argc, char* argv[]) {

  // If at least one argument isn't included, exit
  if (argc < 3) {
    std::cout << "Usage: spawner <interval> <command> [<arg1> <arg2> ... <argN>]" << std::endl;
    exit(1);
  }

  // Debug output the command and argument
  if (DEBUG) {
    std::cout << "INT:\t" << argv[1] << std::endl;
    std::cout << "CMD:\t" << argv[2] << std::endl;
    if (argc > 3)
      std::cout << "ARG:\t" << argv[3] << std::endl;
  }

  // Copy the arguments into a path buffer
  std::stringstream path;
  path << argv[2];

  // Append arguments onto the path string
  for (int i = 3; i < argc; i++) {
    if (DEBUG) 
      std::cout << "i: " << i << " argc: " << argc << " argv: " << argv[i] << std::endl;
    path << " " << argv[i];
  }

  // Copies the string object into a new char array
  char* cstr = new char [path.str().length()+1];
  std::strcpy (cstr, path.str().c_str());

  // Output the path buffer
  if (DEBUG) 
    std::cout << "PATH:\t" << cstr << std::endl;
  
  // Declare the two necessary pointers for CreateProcess
  STARTUPINFO* sinfo = new STARTUPINFO();
  PROCESS_INFORMATION* pid = new PROCESS_INFORMATION();

  BOOL val;
  while (true) {

    // Create the process
    val = CreateProcess(NULL,cstr,NULL,NULL,
               FALSE,DETACHED_PROCESS,NULL,NULL,sinfo,pid);

    // Output the processID
    std::cout << "PID:\t" << pid->dwProcessId << std::endl;

    // Print out the return value, and the error code if necessary
    if (DEBUG) 
      std::cout << "RTN:\t" << val << std::endl;

    if (val == 0 && DEBUG)
      std::cout << "ERR:\t" << GetLastError() << std::endl;

    // Wait until the created process exits
    WaitForSingleObject( pid->hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pid->hProcess );
    CloseHandle( pid->hThread );

    // Sleep for a given interval
    Sleep(atoi(argv[1]));
  }

  // Delete the pointers
  delete sinfo;
  delete pid;

  exit(0);
}