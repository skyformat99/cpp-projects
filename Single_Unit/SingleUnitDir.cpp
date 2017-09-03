#include "SingleUnitDir.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

SingleUnitDir :: SingleUnitDir ()
{
  root = NULL;
}

SingleUnitDir :: SingleUnitDir (string s)
{
  root = NULL;

  setDirectory(s);
}

SingleUnitDir :: ~SingleUnitDir ()
{
  if (root) delete root;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

/**
 * Does this SU dir contain the specified tank dir?
 */
bool SingleUnitDir :: contains (string tank)
{
  stringstream ss;
  ss << (*root) << tank << "*";

  return findSingleFile(ss.str(),(*root),NULL);
}

bool SingleUnitDir :: getChannelFileBase (string tank, 
    int chan, string *res)
{
  string tankDir;
  stringstream ss;
  ss << (*root) << tank << "*";

  if (!findSingleFile(ss.str(),(*root),&tankDir))
    return false;

  tankDir += FS;
  ss.str("");
  ss << tankDir << "Classed_Spikes_Tank" << tank 
     << "_Chan" << chan << "N=*.mat";

  string channelMatFile;
  if (!findSingleFile(ss.str(),tankDir,&channelMatFile)) 
    return false;
  
  (*res) = channelMatFile.substr(0,channelMatFile.length()-4);

  return true;
}

bool SingleUnitDir :: getPoolFile (string tank, string* res)
{
  string tankDir;
  stringstream ss;
  ss << (*root) << tank << "*";

  if (!findSingleFile(ss.str(),(*root),&tankDir))
    return false;

  tankDir += FS;
  ss.str("");
  ss << tankDir << "Tank" << tank << "_BlockAll_PooledSort.mat";

  return findSingleFile(ss.str(),tankDir,res);
}

void SingleUnitDir :: setDirectory (string s)
{
  root = new string(s);

  verifyTrailingSlash();
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

bool SingleUnitDir :: findSingleFile (string needle, 
    string base, string* res)
{
  stringstream ss;
  long iResult;
  struct _finddata_t fileinfo;

  iResult = _findfirst(needle.c_str(),&fileinfo);

  if (iResult == -1L) return false;

  ss.str("");
  ss << base << fileinfo.name;

  if (res) (*res) = ss.str();

  _findclose(iResult);

  return true;
}

/**
 * Verifies that the root directory has a trailing slash
 * or backslash on it.
 */
void SingleUnitDir :: verifyTrailingSlash ()
{
  int length = root->length();

  string lastStr = root->substr(length-1,1);
  if (lastStr.compare("\\") != 0 &&
      lastStr.compare("/") != 0)
    (*root) += FS;
}