//#include <sstream>

/**
 * Macro to assist with throwing errors back up the error
 * propagation chain. Includes the file and line number of
 * the error.
 */
//#ifndef __THROW_MACRO_
//#define __THROW_MACRO_
//
//static stringstream throwStream;
//#define THROW(a) \
//  throwStream.str(""); \
//  throwStream << "  " << __FILE__ << " (" << __LINE__ \
//              << "): " << a << endl; \
//  throw throwStream.str().c_str();
//
//#endif

/** Error output macro **/
#ifndef __ERR_OUT__
#define __ERR_OUT__

#define ERROUT(a) \
  (*logstream) << a; \
  logstream->flush(); \
  logstream->close(); \
  cerr << a; \
  exit(1);

#endif

/**
 * Assist with the handling of an HR system call
 * through the ActiveX system.
 */
#ifndef __HRCALL__
#define __HRCALL__

#define HRCALL(a, errmsg) \
  hr = (a); \
  if (FAILED(hr)) \
    throw errmsg;

#endif