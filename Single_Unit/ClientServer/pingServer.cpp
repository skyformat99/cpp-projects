#include <stdio.h>

#include <string>
#include <iostream>

#include "ClientSocket.h"

/** The two wait-times, in ms - if first fails, wait for second, then FALSE */
#define FIRST_WAIT 100
#define SECOND_WAIT 500

using namespace std;

int main (int argc, char* argv[]) {

  // Print usage if incorrect
  if (argc < 3) {
    cout << "Usage: pingServer <host> <port#>" << endl;
    exit(-1);
  }

  // Get arg's
  string *host;
  try {
    host = new string(argv[1]);
  } catch (...) {
    cout << "Usage: pingServer <host> <port#>" << endl;
    exit(-1);
  }

  long port;
  try {
    port = atol(argv[2]);
  } catch (...) {
    cout << "Usage: pingServer <host> <port#>" << endl;
    exit(-1);
  }

  // Try once to connect to the socket with a sleep() of FIRST_WAIT in ms
  ClientSocket *cs;
  bool result;
  try {
    cs = new ClientSocket();
    result = cs->trySocket(*host,port,FIRST_WAIT);
    delete cs;

  } catch (char *c) {
    cout << "FALSE";
    exit(1);
  }

  // If we failed once, try one more time with a higher wait period
  if (!result) {
    try {
      cs = new ClientSocket();
      result = cs->trySocket(*host,port,SECOND_WAIT);
      delete cs;

    } catch (char *c) {
      cout << "FALSE";
      exit(1);
    }
  }

  if (!result)
    cout << "FALSE";
  else
    cout << "TRUE";

  // NOT SURE IF WE NEED THESE TESTS ...
/**
  // If our test conncet worked, we need to make sure the server is
  // active and is one of our servers - so try a full connect now
  if (result) {
    try {
      cs = new ClientSocket(*host,port);

    } catch (char *c) {
      cout << "FALSE";
      exit(1);
    } catch (...) {
      cout << "FALSE";
      exit(1);
    }
  }
  
  // If our full connect was successful, try pinging the server
  try {
    if (cs->ping())
      cout << "TRUE";
    else
      cout << "FALSE";
  } catch (...) {
    cout << "FALSE";
  }
**/

  // Clean up and return
  delete cs;
  delete host;

  exit(0);
}
