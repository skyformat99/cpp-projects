#ifndef __single_unit_dir_h__
#define __single_unit_dir_h__

#include <string>
#include <iostream>
#include <sstream>

#include <io.h> // Needed for find-file functions

#include "UploadUtil.h"

#define FS "/"

using namespace std;

class SingleUnitDir {

private:
  string* root;

  bool findSingleFile (string,string,string*);
  void verifyTrailingSlash ();

public:
  SingleUnitDir ();
  SingleUnitDir (string);
  ~SingleUnitDir ();

  bool contains (string);
  bool getChannelFileBase (string,int,string*);
  bool getPoolFile (string,string*);

  void setDirectory (string);
};

#endif