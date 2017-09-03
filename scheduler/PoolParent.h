#ifndef __PoolParent_h__
#define __PoolParent_h__

using namespace std;

class PoolParent
{

public:

  int length;

  virtual void print () = 0;

protected:

  string fileName;

  virtual void loadFromFile () = 0;

};

#endif