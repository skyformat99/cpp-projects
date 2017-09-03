#include "Style.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

Style::Style (int val, int r, int g, int b, int a)
{
  this->val = val;
  this->rgba = convertToHex(r,g,b,a);

  stringstream ss;
  ss << "sty" << val;

  this->sVal = ss.str();
}

Style::Style (int val, string sVal, int r, int g, int b, int a)
{
  this->val = val;
  this->sVal = sVal;
  this->rgba = convertToHex(r,g,b,a);
}

Style::Style (int val, string sVal, string rgba)
{
  this->val = val;
  this->sVal = sVal;
  this->rgba = rgba;
}

Style::Style ()
{
  this->val = 0;
  this->sVal = string("a");
  this->rgba = string("00ffffff");
}

Style::~Style ()
{

}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

/** int main ()
{
  Style* s = new Style(0,0,0,0,96);

  s->displayInfo();

  return 0;
} **/

/**
 * Print some display info on this instance.
 */
void Style::displayInfo ()
{
  cout << "::Style::" << endl;
  cout << "Val: " << val << " sVal: " << sVal << endl;
  cout << "RGBA String: " << rgba << endl;
}

/**
 * Writes the style declaration for this instance.
 */
void Style::writeStyleDeclaration (ostream* fout)
{
  *fout << "  <Style id=\"" << sVal << "\">" << endl;
  *fout << "    <PolyStyle>" << endl;
  *fout << "      <color>" << rgba << "</color>" << endl;
  *fout << "      <colorMode>normal</colorMode>" << endl;
  *fout << "      <fill>1</fill>" << endl;
  *fout << "      <outline>0</outline>" << endl;
  *fout << "    </PolyStyle>" << endl;
  *fout << "  </Style>" << endl;
}

/**
 * Writes the StyleURL tag out to the outstream.
 */
void Style::writeStyleURL (ostream* fout)
{
  *fout << "    <styleUrl>#" << sVal << "</styleUrl>" << endl;
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

/**
 * Converts the individual r, g, b and a values into a 
 * hex string.
 */
string Style::convertToHex (int r, int g, int b, int a)
{
  if (r < 0 || r > 255 ||
      g < 0 || g > 255 ||
      b < 0 || b > 255 ||
      a < 0 || a > 255)
    throw "Error: r, g, b, a out of bounds!";

  stringstream ss;
  ss.str("");

  ss << hex << setw(2) << setfill('0') << a 
     << setw(2) << setfill('0') << b 
     << setw(2) << setfill('0') << g 
     << setw(2) << setfill('0') << r;

  return ss.str();
}