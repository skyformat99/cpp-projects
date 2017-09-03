#include "XMLNode.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

XMLNode :: XMLNode ()
{
  pNode = NULL;
  attribs = NULL;
}

XMLNode :: XMLNode (IXMLDOMNode* node)
{
  pNode = NULL;
  attribs = NULL;

  setPointer(node);
}

XMLNode :: ~XMLNode ()
{
  if (pNode) pNode->Release();
  if (attribs) attribs->Release();
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

bool XMLNode :: isValid ()
{
  return (!pNode);
}

string XMLNode :: getAttribute (char* c) 
{
  IXMLDOMNode* attrib = NULL;
  VARIANT var;
  HRESULT hr;
  
  HRCALL(attribs->getNamedItem(_bstr_t(c),&attrib),
        "HR CALL FAILED: Get attrib in node");

  if (!attrib) 
    throw "Could not find attribute from node";

  VariantInit(&var);
  HRCALL(attrib->get_nodeValue(&var), 
      "HR CALL FAILED: get_nodeValue from attribute");

  _bstr_t bt = _bstr_t(var.bstrVal,true);

  if (&var) VariantClear(&var);
  if (attrib) attrib->Release();

  return (char*)bt;
}

void XMLNode :: getSingleNode (char* c, XMLNode* n)
{
  HRESULT hr;
  IXMLDOMNode *node = NULL;

  HRCALL(pNode->selectSingleNode(_bstr_t(c), &node),
      "HR CALL FAILED: node->selectSingleNode()");

  if (!node) 
    throw "node->selectSingleNode() failed";
  
  n->setPointer(node);
}

void XMLNode :: getNodes (char* c, NodeList* nList)
{
  HRESULT hr;
  IXMLDOMNodeList *pNodes = NULL;

  HRCALL(pNode->selectNodes(_bstr_t(c), &pNodes),
      "HR CALL FAILED: node->selectNodes()");

  if (!pNodes)
    throw "selectNodes call returned NULL pointer";

  nList->setPointer(pNodes);
}

int XMLNode :: getNumberOfAttributes () 
{
  HRESULT hr;

  long nAttribs;
  HRCALL(attribs->get_length(&nAttribs),
      "HR CALL FAILED: Get length of attributes from node");

  return (int)nAttribs;
}

void XMLNode :: setPointer (IXMLDOMNode* node)
{
  pNode = node;

  updateAttributes();
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

void XMLNode :: updateAttributes ()
{
  HRESULT hr;

  if (pNode) {
    if (attribs) attribs->Release();
    HRCALL(pNode->get_attributes(&attribs),
        "HR CALL FAILED: Gather attributes from node");
  }
}