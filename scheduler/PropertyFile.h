#ifndef __PropertyFile_h__
#define __PropertyFile_h__

#include<vector>

using namespace std;

class PropertyFile 
{

public:
  PropertyFile (string, string);
  ~PropertyFile ();

  int length ();
  string lineAt (int);
  vector<string> tokenizeToVector (int);

  string& operator [] (int);

private:

  string filename;
  string separator;

  vector<string>* lines;

  void loadFile ();
};

#endif