#ifndef __xml_node_list__
#define __xml_node_list__

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

class XMLNodeList : public NodeList {

private:
  IXMLDOMNodeList *pNodes;

public:
  XMLNodeList ();
  XMLNodeList (IXMLDOMNodeList*);
  ~XMLNodeList ();

  bool isValid ();
  int length ();

  void getNode (int, XMLNode*);

  void setPointer (IXMLDOMNodeList*);
};

#endif