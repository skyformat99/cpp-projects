#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

void printUsage();
string trim (string&);
string removeExtension (string&, string&);

/**
 * Program entry point.
 */
void main (int argc, char* argv[])
{
  /** Ensure that we have at least the flag present */
  if (argc < 2)
    printUsage();

  stringstream out;
  int optArg = 3;

  string* flag = new string(argv[1]);

  /** Gets the length of a string */
  if (flag->compare("-l") == 0) {
    if (argc < 3) printUsage();
    optArg = 4;

    string* s1 = new string(argv[2]);

    out << s1->length();
  }

  /** Return the position of a needle in the haystack */
  if (flag->compare("-f") == 0) {
    if (argc < 4) printUsage();
    optArg = 5;

    string* s1 = new string(argv[2]);
    string* s2 = new string(argv[3]);

    size_t pos = s1->find(*s2);

    if (pos == string::npos)
      out << -1;
    else
      out << pos;
  }

  /** Return the front or back surrounding delim */
  if (flag->compare("-F") == 0 || flag->compare("-B") == 0) {
    if (argc < 4) printUsage();
    optArg = 5;

    string* s1 = new string(argv[2]);
    string* s2 = new string(argv[3]);

    size_t pos = s1->find(*s2);
    if (pos == string::npos)
      out << *s1;
    else {
      if (flag->compare("-F") == 0)
        out << s1->substr(0,pos);
      else
        out << s1->substr(pos+1);
    }
  }

  /** Trims the result string, and displays w/ or w/o 'SET' info */
  string ext = ".mp3";
  string trimmedStr = removeExtension(trim(out.str()),ext);

  if (argc >= optArg) 
    cout << "SET " << argv[optArg-1] << "=\"" << trimmedStr << "\"" << endl;
  else
    cout << trimmedStr << endl;

  exit(0);
}

/**
 * Prints the usage of this function to the console, and exits.
 */
void printUsage ()
{
  cout << "Usage: stringOp -BfF <string-arg-1> ... <string-arg-n> [<set-env>]" << endl;
  cout << endl;
  cout << "   -l   Returns the length of the string" << endl;
  cout << "   -f   Find a needle in a haystack, return the position" << endl;
  cout << endl;
  cout << "   -F   Find a needle in a haystack, return trimmed text in front" << endl;
  cout << "   -B   Find a needle in a haystack, return trimmed text in back" << endl;
  cout << endl;
  cout << "   [<set-env>]   If present, prints 'SET <set-env>=' prior to result" << endl;
  exit(-1);
}

/** 
 * Returns a trimmed version of the passed string. Removes leading
 * and trailing white-space (' ' char only).
 */
string trim (string& s)
{
  int f = 0;
  while (s.at(f) == ' ') { f++; }

  int b = s.length() - 1;
  while (s.at(b) == ' ') { b--; }

#ifdef DEBUG
  cout << "s.length(): " << s.length() << " f: " << f << " b: " << b << endl;
#endif

  return s.substr(f,b-f+1);
}

/**
 * Removes the specified extension, if it exists
 */
string removeExtension (string &s, string& ext)
{
  if (s.length() <= 4)
    return s;

  if (s.substr(s.length()-4).compare(ext) == 0)
    return s.substr(0,s.length()-4);
  else
    return s;
}