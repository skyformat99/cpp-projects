#ifndef __node_list__
#define __node_list__

//#include <comutil.h>
#include <msxml2.h>

/**
 * Interface for NodeList, which allows the cross-reference
 * of XMLNode and XMLNodeList.
 */
class NodeList {

public:
  virtual void setPointer (IXMLDOMNodeList*) = 0;

};

#endif