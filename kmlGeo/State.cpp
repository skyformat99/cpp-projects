#include "State.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

State::State (string* code)
{
  initFromFile(code);
}

State::~State ()
{
  for (int i=0; i < numElems; i++)
    delete elems[i];

  delete [] elems;
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

/**
 * Finds the string GeoElem in the collection of elements.
 * Returns 1 on success, 0 on failure to find the string.
 */
int State::findGeoElem (int *geoIndex, string& str)
{
  for (int i=0; i < numElems; i++) {
    if (elems[i]->equals(str)) {
      *geoIndex = i;
      return 1;
    }
  }

  return 0;
}

/**
 * Displays the info for a particular geoElem based on the
 * index of that element.
 */
void State::displayElem (int geoIndex) 
{
  elems[geoIndex]->displayElem();
}

/**
 * Display some basic info about the State instance.
 */
void State::displayInfo ()
{
  cout << "::State::" << endl;
  cout << "Size of vector: " << numElems << endl;

#ifdef DEBUG1
  elems[100]->displayElem();
#endif

}

/**
 * Static method to determine if thee state code file exists.
 */
bool State::does_exist (string* code)
{
  int intStat; 
  struct stat stFileInfo; 
  stringstream ss;

  ss << DATA_FILE_DIR;
  ss << "tr" << *code << "_d00a.dat";

cout << "File: " << ss.str() << endl;

  intStat = stat(ss.str().c_str(),&stFileInfo);
  if (intStat != 0) return false;

  ss.str("");
  ss << DATA_FILE_DIR;
  ss << "tr" << *code << "_d00.dat";

  intStat = stat(ss.str().c_str(),&stFileInfo);
  if (intStat != 0) return false;

  return true;
}

/** int main () 
{
  cout << "Does exist 42: " << State::does_exist(new string("42")) << endl;

  State* s = new State(new string("42"));

  s->displayInfo();

  s->writeTestFile("tr42_d00.kml");

  return 0;
} **/

/**
 * Sets the style to a particular geoElem based on the index.
 */
void State::setGeoStyle (int geoIndex, Style* style)
{
  elems[geoIndex]->setStyle(style);
}

/**
 * Sets the CSV file data value as meta-data to the element.
 */
void State::setMetaData (int geoIndex, double d)
{
  stringstream ss;
  ss << "(val = " << d << ")";

  elems[geoIndex]->setMetaData(ss.str());
}

/**
 * Sets the style group for this state.
 */
void State::setStyleGroup (StyleGroup* sg)
{
  this->sg = sg;
}

/**
 * Write a test KML file, which paints each census tract
 * a random color.
 */
void State::writeTestFile (char* file)
{
  filebuf fb;
  fb.open (file, ios::out);
  ostream fout (&fb);

  fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  fout << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">" << endl;
  fout << "<Document>" << endl;
  fout << "  <name>Polygon.kml</name>" << endl;
  fout << "  <open>0</open>" << endl;

  if (sg)
    sg->writeStyleDeclaration(&fout);

/**
  fout << "  <Style id=\"a\">" << endl;
  fout << "    <PolyStyle>" << endl;
  fout << "      <color>6600ffff</color>" << endl;
  fout << "      <colorMode>random</colorMode>" << endl;
  fout << "      <fill>1</fill>" << endl;
  fout << "      <outline>0</outline>" << endl;
  fout << "    </PolyStyle>" << endl;
  fout << "  </Style>" << endl;
**/

  for (int i = 0; i < numElems; i++)
    elems[i]->writePlacemark(&fout);

  fout << "</Document>" << endl;
  fout << "</kml>" << endl;

  try {
    fb.close();
  } catch (...) {}
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

/**
 * Initialize the GeoElem vector from the two data files.
 */
void State::initFromFile (string* code)
{
  vector<GeoElem*>* tempElems = new vector<GeoElem*>;

  stringstream ss;

  ss.str("");
  ss << DATA_FILE_DIR << "tr" << *code << "_d00a.dat";

  readAttribFile(ss.str(),tempElems);

  this->numElems = tempElems->size();
  elems = new GeoElem*[numElems];

  for (int i = 0; i < numElems; i++) {
    elems[i] = new GeoElem(*(tempElems->at(i)));

//    delete tempElems->at(i);
  }

  delete tempElems;

  ss.str("");
  ss << DATA_FILE_DIR << "tr" << *code << "_d00.dat";

  readCoordFile(ss.str());
}

/** 
 * Read the attributes from the first data file,
 * which contains all the info about each census tract
 * (except for the coordinates).
 */
void State::readAttribFile (string file, vector<GeoElem*>* e)
{
  ifstream attribFile;
  attribFile.open(file.c_str());

  if (!attribFile.is_open())
    throw "Error: could not open attrib file!";

  char* buf = new char[BUF_LEN];

  int id;
  string *stateID, *countyID, *tractID, *tractName;

  while (!attribFile.eof()) {

    attribFile.getline(buf,BUF_LEN);
    id = atoi(buf);

    if (attribFile.eof()) break;

    attribFile.getline(buf,BUF_LEN);
    stateID = new string(buf);

    attribFile.getline(buf,BUF_LEN);
    countyID = new string(buf);

    attribFile.getline(buf,BUF_LEN);
    tractID = new string(buf);

    attribFile.getline(buf,BUF_LEN);
    tractName = new string(buf);

    attribFile.getline(buf,BUF_LEN);
    attribFile.getline(buf,BUF_LEN);
    attribFile.getline(buf,BUF_LEN);

    e->push_back(new GeoElem(id,stateID,countyID,tractID,tractName));

    delete stateID;
    delete countyID;
    delete tractID;
    delete tractName;
  }

  attribFile.close();

  delete buf;
}

/**
 * Now read the coordinates for each census tract, and pair
 * with the appropriate GeoElem.
 */
void State::readCoordFile (string file)
{
  ifstream coordFile;
  coordFile.open(file.c_str());

  if (!coordFile.is_open())
    throw "Error: could not open coord file!";

  vector<string>* vec = new vector<string>;

  char* buf = new char[BUF_LEN];
  string* s;
  Coords *c;

  // Loop until we hit the EOF
  while (!coordFile.eof()) {

    // Clear out the vector
    vec->clear();

    // Get the next line
    coordFile.getline(buf,BUF_LEN);
    s = new string(buf);

#ifdef DEBUG2
    cout << "ReadLine: #" << *s << "#" << endl;
#endif

    // If this is the first line of the group, its the EOF
    if (s->find("END") != string::npos)
      break;

    // Read lines until we reach the next "END" line, adding
    // lines to a string vector
    while (s->find("END") == string::npos) {

      vec->push_back(string(*s));

      delete s;
      coordFile.getline(buf,BUF_LEN);
      s = new string(buf);

    }

    // Declare a new Coords object, set to the proper element, delete
    c = new Coords(vec);

#ifdef DEBUG2
    cout << "  ID: " << c->getID() << endl;
#endif

    if (c->getID() > 0)
      elems[c->getID()]->addCoords(c);
//      (*elems)[c->getID()]->addCoords(c);

    delete c;
  }
  
  coordFile.close();
}

