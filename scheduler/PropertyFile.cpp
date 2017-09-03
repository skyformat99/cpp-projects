#include "PropertyFile.h"
#include "Utilities.h"

#include <iostream>
#include <fstream>
#include <string>

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

PropertyFile::PropertyFile (string fn, string sep)
{
  this->filename = fn;
  this->separator = sep;

  lines = new vector<string>;

  loadFile();
}

PropertyFile::~PropertyFile ()
{
  delete lines;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

string PropertyFile::lineAt (int i)
{
  return lines->at(i);
}

vector<string> PropertyFile::tokenizeToVector (int i)
{
  return Utilities::tokenizeToStringVector (lines->at(i), this->separator);
}

int PropertyFile::length ()
{
  return lines->size();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void PropertyFile::loadFile ()
{
  string c;

  ifstream ifs (this->filename, ifstream::in);

  while (ifs.good()) {
    getline(ifs,c);

    if (!c.empty())
      lines->push_back(c);
  }  

  ifs.close();
}
