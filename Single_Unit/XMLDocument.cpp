#include "XMLDocument.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

XMLDocument :: XMLDocument () 
{
  pXMLDom = NULL;
  initialize();
}

XMLDocument :: XMLDocument (char* c)
{
  pXMLDom = NULL;
  initialize();

  loadDocument(c);
}

XMLDocument :: ~XMLDocument ()
{
  if (pXMLDom) pXMLDom->Release();

  CoUninitialize();
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

/**
 * Finds the named root node and stores it
 * in the passed XMLNode object.
 */
void XMLDocument :: getRootNode (char* c, XMLNode* node) throw (char*)
{
  HRESULT hr;
  IXMLDOMNode *pNode = NULL;

  HRCALL(pXMLDom->selectSingleNode(_bstr_t(c), &pNode),
      "HR CALL FAILED: dom->selectSingleNode()");

  if (!pNode)
    throw "Error: document root node is null!";

  node->setPointer(pNode);
}

/**
 * Loads the XML document into the DOM object.
 */
void XMLDocument :: loadDocument (char* c)
{
  HRESULT hr;

  VARIANT var;
  VARIANT_BOOL status;

  VariantInit(&var);
  V_BSTR(&var) = SysAllocString(_bstr_t(c));
  V_VT(&var) = VT_BSTR;

  HRCALL(pXMLDom->load(var,&status),
      "HR CALL FAILED: dom->load()");

  if (&var) VariantClear(&var);

  if (status != VARIANT_TRUE)
    throw "Error: DOM document is not loaded";
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

/**
 * Initializes the COM interface and declares the 
 * XMLDOMDocument object.
 */
void XMLDocument :: initialize ()
{
  HRESULT hr;

  HRCALL(CoInitialize(NULL),
      "HR CALL FAILED: Failed to initialize COM object");

  HRCALL(CoCreateInstance(__uuidof(DOMDocument30),
    NULL, CLSCTX_INPROC_SERVER,
    __uuidof(IXMLDOMDocument),(void**)&pXMLDom),
     "HR CALL FAILED: Failed to create new DOMDocument");

  HRCALL(pXMLDom->put_async(VARIANT_FALSE),
      "HR CALL FAILED: should never fail");
  HRCALL(pXMLDom->put_validateOnParse(VARIANT_FALSE),
      "HR CALL FAILED: should never fail");
  HRCALL(pXMLDom->put_resolveExternals(VARIANT_FALSE),
      "HR CALL FAILED: should never fail");
}