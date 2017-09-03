#ifndef __xml_document__
#define __xml_document__

#include <iostream>
#include <string>

#include <comutil.h>
#include <msxml2.h>

#include "XMLNode.h"
#include "UploadUtil.h"

using namespace std;

/** 
 * These libraries require linking using:
 *    msxml2.lib ole32.lib oleaut32.lib comsuppw.lib
 */

class XMLDocument {

private:
  IXMLDOMDocument* pXMLDom;

  void initialize ();

public:
  XMLDocument ();
  XMLDocument (char*);
  ~XMLDocument ();

  void loadDocument (char*);

  void getRootNode (char*, XMLNode*);
};

#endif