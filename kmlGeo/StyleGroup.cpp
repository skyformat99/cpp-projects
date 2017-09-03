#include "StyleGroup.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

StyleGroup::StyleGroup (string begin, string end, int nbins, 
  double min, double max)
{
  this->min = min;
  this->max = max;
  this->nbins = nbins;

  this->step = (max - min) / (double)nbins;

  styles = new Style*[nbins];

  initGroup (begin,end,nbins);
}


StyleGroup::~StyleGroup ()
{
  if (styles) delete styles;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

/** int main ()
{
  StyleGroup* sg = new StyleGroup(string("60ffff00"),string("60000000"),10);
} **/

/**
 * Sets the Style element on the geoElem object based on the 
 * value 'd' passed.
 */
void StyleGroup::setElementStyle (double d, GeoElemCollection* s, int geoIndex)
{
  // Need to figure out which style group this jumps into ...
  int n;
  if (d <= min) {
    n = 0;
  } else if (d >= max) {
    n = nbins - 1;
  } else {
    n = (int)floor((d - min) / step);
  }

#ifdef DEBUG5
  cout << "d: " << d << " n: " << n << endl;
#endif

  s->setGeoStyle( geoIndex, styles[n] );
}

/** 
 * Access the underlying 'Style' element at the given index.
 */
Style* StyleGroup::operator[] (int n)
{
  if (n >= 0 && n < nbins)
    return styles[n];
  else
    throw "Error: index out of bounds!";
}

/**
 * Write the style declaration for the group.
 */
void StyleGroup::writeStyleDeclaration (ostream* fout)
{
  for (int i=0;i<nbins;i++)
    styles[i]->writeStyleDeclaration(fout);
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

void StyleGroup::initGroup (string begin, string end, int nbins)
{
  int bA, bR, bG, bB, eA, eR, eG, eB;
  char* p;
  bA = (int)strtol(begin.substr(0,2).c_str(),&p,16);
  bR = (int)strtol(begin.substr(2,2).c_str(),&p,16);
  bG = (int)strtol(begin.substr(4,2).c_str(),&p,16);
  bB = (int)strtol(begin.substr(6,2).c_str(),&p,16);

  eA = (int)strtol(end.substr(0,2).c_str(),&p,16);
  eR = (int)strtol(end.substr(2,2).c_str(),&p,16);
  eG = (int)strtol(end.substr(4,2).c_str(),&p,16);
  eB = (int)strtol(end.substr(6,2).c_str(),&p,16);

#ifdef DEBUG3
  cout << "Begin: (" << bA << "," << bR << "," << bG << "," << bB << ")" << endl;
  cout << "End: (" << eA << "," << eR << "," << eG << "," << eB << ")" << endl;
#endif
  
  int i;
  double aStep, rStep, gStep, bStep;
  
  aStep = (double)(eA - bA) / (double)(nbins-1);
  rStep = (double)(eR - bR) / (double)(nbins-1);
  gStep = (double)(eG - bG) / (double)(nbins-1);
  bStep = (double)(eB - bB) / (double)(nbins-1);

#ifdef DEBUG3
  cout << "Steps: " << aStep << ", " << rStep << ", " 
       << gStep << ", " << bStep << endl;
#endif

  for (i=0;i<nbins;i++) {
    styles[i] = new Style(i,(int)(bR+rStep*(double)i),(int)(bG+gStep*(double)i),
                  (int)(bB+bStep*(double)i),(int)(bA+aStep*(double)i));

#ifdef DEBUG3
    styles[i]->displayInfo();
#endif
  }
}


  