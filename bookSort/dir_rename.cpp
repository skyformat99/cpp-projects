#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void appendStringSegment (stringstream*, string, bool);
string trim (string);

int main (int argc, char* argv[])
{
  string prefix = "";
  string suffix = "";

  if (argc > 2)
    prefix = argv[2];

  if (argc > 3)
    suffix = argv[3];

  string full = argv[1];

  stringstream* output = new stringstream();

  unsigned f_dash = full.find("&");

  while (f_dash != string::npos) {

    appendStringSegment(output,full.substr(0,f_dash),true);

    full = full.substr(f_dash+1);

    f_dash = full.find("&");
  }

  appendStringSegment(output,full,false);

  cout << prefix << output->str() << suffix;

  return 0;
}

/**
 * Reverse a string segment if there exists a single space, and
 * append the reversed segment to the stringstream.
 */
void appendStringSegment (stringstream* output, string segment, bool amp)
{
  segment = trim(segment);

  string lastName, firstName;
  unsigned last_space = segment.rfind(" ");

  // If no spaces, append this string
  if (last_space == string::npos) {
    (*output) << segment;

  // Otherwise, flip it to front and append
  } else {
    lastName = segment.substr(last_space);
    lastName = trim(lastName);

    firstName = segment.substr(0,last_space);
    firstName = trim(firstName);

    (*output) << lastName << ", " << firstName;
  }    

  if (amp)
    (*output) << " & ";
}

/**
 * Trim a string of white-space and return the trimmed string
 */
string trim (string toTrim)
{
  // Trim back
  while (toTrim.at(toTrim.length()-1) == ' ')
    toTrim = toTrim.substr(0,toTrim.length()-1);

  // Trim front
  while (toTrim.at(0) == ' ')
    toTrim = toTrim.substr(1);

  return toTrim;
}