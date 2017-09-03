#pragma once

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

class Style
{

private:
  int val;
  string sVal;
  string rgba;

  string convertToHex (int, int, int, int);

public:
  Style ();
  Style (int, int, int, int, int);
  Style (int, string, int, int, int, int);
  Style (int, string, string);
  ~Style ();

  void displayInfo ();
  void writeStyleDeclaration (ostream*);
  void writeStyleURL (ostream*);

};