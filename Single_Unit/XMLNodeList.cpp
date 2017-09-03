#include "XMLNodeList.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

XMLNodeList :: XMLNodeList ()
{
  pNodes = NULL;
}

XMLNodeList :: XMLNodeList (IXMLDOMNodeList *n) 
{
  pNodes = NULL;

  setPointer(n);
}

XMLNodeList :: ~XMLNodeList ()
{
  if (pNodes) pNodes->Release();
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

bool XMLNodeList :: isValid ()
{
  return (pNodes);
}

int XMLNodeList :: length ()
{
  if (!isValid())
    throw "XMLNodeList is not valid!";

  HRESULT hr;
  long length;

  HRCALL(pNodes->get_length(&length),
      "HR CALL FAILED: nodeList->length");

  return (int)length;
}

void XMLNodeList :: getNode (int i, XMLNode *n)
{
  HRESULT hr;
  IXMLDOMNode *pNode = NULL;

  HRCALL(pNodes->get_item(i,&pNode),
      "HR CALL FAILED: XMLNodeList[i]");

  if (!pNode)
    throw "Error: get_item from nodeList failed";

  n->setPointer(pNode);
} 

void XMLNodeList :: setPointer (IXMLDOMNodeList *n) 
{
  pNodes = n;
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *