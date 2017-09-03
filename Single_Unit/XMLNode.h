#ifndef __xml_node__
#define __xml_node__

#include <iostream>
#include <string>

#include <comutil.h>
#include <msxml2.h>

#include "NodeList.h"
#include "UploadUtil.h"

using namespace std;

/** 
 * These libraries require linking using:
 *    msxml2.lib ole32.lib oleaut32.lib comsuppw.lib
 */

class XMLNode {

private:
  IXMLDOMNode *pNode;
  IXMLDOMNamedNodeMap* attribs;

  void updateAttributes ();

public:
  XMLNode ();
  XMLNode (IXMLDOMNode*);
  ~XMLNode ();

  bool isValid ();
  string getAttribute (char*);
  void getNodes (char*,NodeList*);
  void getSingleNode (char*,XMLNode*);
  int getNumberOfAttributes ();

  void setPointer (IXMLDOMNode*);
};

#endif