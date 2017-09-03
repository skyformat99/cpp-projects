#ifndef __tankServer__
#define __tankServer__

#include "ServerSocket.h"
#include "TankCollection.h"

class TankServer {

private:

  TankCollection *tc;
  ServerSocket *ss;

  void processConnection (ServerSocket *, TankCollection *);

public:
  TankServer ();
  ~TankServer ();

  run (int port);

};

#endif