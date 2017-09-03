#ifndef __ProcessElement_h__
#define __ProcessElement_h__

class ProcessElement 
{

public:
  virtual void process () = 0;
  virtual bool isProcessed () = 0;

private:

};

#endif