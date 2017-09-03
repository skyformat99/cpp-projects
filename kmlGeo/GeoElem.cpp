#include "GeoElem.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

/** 
 * Construct a new GeoElement with data from the
 * attributes ('a') file.
 */
GeoElem::GeoElem (int id, string* stateID, string* countyID,
    string* tractID, string* tractName)
{
  this->id        = id;
  this->stateID   = trimQuotes(stateID);

  if (this->stateID.substr(0,1) == "0")
    this->stateID = this->stateID.substr(1,1);

  this->countyID  = trimQuotes(countyID);
  this->tractID   = fillTractID(trimQuotes(tractID));
  this->tractName = trimQuotes(tractName);

  this->metaData  = "";
  this->isSet     = false;

  style = new Style();
}

/**
 * Construct a new GeoElement by copying from a passed
 * GeoElement.
 */
GeoElem::GeoElem (GeoElem& geo)
{
  this->id        = geo.id;
  this->stateID   = geo.stateID;
  this->countyID  = geo.countyID;
  this->tractID   = geo.tractID;
  this->tractName = geo.tractName;

  this->metaData  = geo.metaData;
  this->isSet     = geo.isSet;

  this->style     = geo.style;

  if (geo.coords)
    this->coords = geo.coords;
}

/** 
 * Deconstruct the GeoElem element.
 */
GeoElem::~GeoElem ()
{
  if (coords) delete coords;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

/**
 * Add the coordinates to this element.
 */
void GeoElem::addCoords (Coords* c)
{
  coords = new Coords(c);
}

/**
 * Displays the GeoElement.
 */
void GeoElem::displayElem ()
{
  cout << "ID: " << id << " StateID: " << stateID << " CountyID: " 
       << countyID << endl;
  cout << "  TractID: " << tractID << " TractName: " << tractName 
       << endl;

  style->displayInfo();
}

/**
 * True if the geoElement ID is equal to the str, false otherwise.
 */
bool GeoElem::equals (string& str)
{
  return (string(stateID + countyID + tractID) == str);
}

/** 
 * Sets a bit of meta-data to be displayed with the name.
 */
void GeoElem::setMetaData (string& str)
{
  metaData = str;
  isSet = true;
}

/**
 * Sets the appropriate style element for this GeoElem.
 */
void GeoElem::setStyle (Style* s)
{
  this->style = s;
}

/**
 * Write the place mark for this element, in KML format.
 */
void GeoElem::writePlacemark (ostream* fout)
{
  if (!this->isSet) return;
  if (this->id <= 0) return;

  *fout << "  <Placemark>" << endl;

  if (this->metaData.compare("") != 0)
    *fout << "    <name>" << this->tractName << " " 
          << this->metaData << "</name>" << endl;
  else
    *fout << "    <name>" << this->tractName << "</name>" << endl;

  style->writeStyleURL(fout);

  *fout << "    <Polygon>" << endl;
  *fout << "      <extrude>0</extrude>" << endl;
  *fout << "      <altitudeMode>clampToGround</altitudeMode>" << endl;
  *fout << "      <outerBoundaryIs>" << endl;
  *fout << "        <LinearRing>" << endl;
  *fout << "          <coordinates>" << endl;

  coords->writeCoordinates(fout);

  *fout << "          </coordinates>" << endl;
  *fout << "        </LinearRing>" << endl;
  *fout << "      </outerBoundaryIs>" << endl;
  *fout << "    </Polygon>" << endl;
  *fout << "  </Placemark>" << endl;
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

string GeoElem::fillTractID (string& s)
{
  if (s.length() == 4)
    return string(s + "00");
  else
    return s;
}

/**
 * Trims the quotes off a string by finding the first, 
 * last, and returning a copy of the string in between.
 */
string GeoElem::trimQuotes (string* s)
{
  int sPos = s->find_first_of("\"");
  int ePos = s->find_last_of("\"");

  return s->substr(sPos+1,ePos-sPos-1);
}
