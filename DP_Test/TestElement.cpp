#include "TestElement.h"

TestElement::TestElement (int id) 
{
  this->id = id;
  this->result = 0.;

  this->processed = false;
}

void TestElement::process ()
{
  srand( (unsigned)time( NULL ) );

  int i;
  for (i=0;i<1000000;i++) 
    this->result = this->result + (rand() * pow(-1.,i));

  this->processed = true;

  std::cout << "(Processed " << this->id << ")" << std::endl;

  return;
}

int TestElement::getID ()
{
  return this->id;
}

double TestElement::getResult ()
{
  return this->result;
}

bool TestElement::isProcessed ()
{
  return this->processed;
}

