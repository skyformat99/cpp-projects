#include "FileCSV.h"

// * * * * * * * * * * * * * * * *
// * CONSTRUCTORS
// * * * * * * * * * * * * * * * *

FileCSV::FileCSV (string* s)
{
  importFile(s);
}

FileCSV::~FileCSV ()
{
  if (geoID) delete geoID;

  if (data) {
    for (int i=0;i<numCols;i++)
      delete data[i];
    delete [] data;
  }
}

// * * * * * * * * * * * * * * * *
// * PUBLIC METHODS
// * * * * * * * * * * * * * * * *

/**
int main () 
{
  FileCSV* f = new FileCSV(new string("rich_houses.csv"));
//  FileCSV* f = new FileCSV(new string("test_three.csv"));

  f->displayHeaders(true);

  f->displayInfo();

  return 0;
} **/

/**
 * Returns the GeoID at the given index.
 */
string FileCSV::at (int i)
{
  return geoID->at(i);
}

/**
 * Returns the value at a given index, column.
 */
double FileCSV::at (int i, int col)
{
  return data[col]->at(i);
}

/**
 * Computes the min value on the specified
 * column. Columns are specified in 0 ... n format.
 */
void FileCSV::columnStats (int col, double* min, double* max,
  double* mean, double* stdev, int* nbins)
{
  if (col < 0 || col > numCols-2)
    throw "Error: column specifier out of bounds!";

  int i, n = data[col]->size();
  double v;
  *min = 1.7E308;
  *max = -1.7E308;
  *mean = 0.;
  *stdev = 0.;

  for (i=0; i<n; i++) {
    v = data[col]->at(i);

    if (v < *min) *min = v;
    if (v > *max) *max = v;
    *mean += v;
  }
  *mean /= (double)n;

  *nbins = (int)(log((double)n) / log(2.)) + 1;

  if (n == 1) return;

  for (i=0; i<n; i++) {
    *stdev += (data[col]->at(i) - *mean) * 
              (data[col]->at(i) - *mean);
  }
  *stdev /= (double)(n-1);
  
  *stdev = sqrt(*stdev);
}

/**
 * Displays the column headers, with optional numbers
 * in front of each header (running 1 ... numCols).
 */
void FileCSV::displayHeaders (bool numberp)
{
  for (int i = 0; i < numCols; i++) {
    if (numberp) cout << "(" << (i+1) << ") ";

    cout << columnLabels[i] << endl;
  }
}

/**
 * Display some basic info about this instance.
 */
void FileCSV::displayInfo ()
{
  cout << "::FileCSV::" << endl;
  cout << "numCols: " << numCols << endl;
  cout << "numElems: " << geoID->size() << endl;
}

/**
 * Returns the first two characters of the first geoID
 * element, which should correspond to the state code.
 */
string FileCSV::getStateCode ()
{
  if (geoID->at(0).length() == 10)
    return "0" + geoID->at(0).substr(0,1);
  else
    return geoID->at(0).substr(0,2);
}

/**
 * Returns the number of rows in the CSV file.
 */
int FileCSV::size ()
{
  return geoID->size();
}

// * * * * * * * * * * * * * * * *
// * PRIVATE METHODS
// * * * * * * * * * * * * * * * *

/**
 * Counts the number of columns in a single line
 * of data from the CSV file.
 */
int FileCSV::countColumns (string* s)
{
  int colCount = 1;

  size_t pos = s->find(",");

  while (pos != string::npos) {
    colCount++;
    pos = s->find(",",pos+1);
  }

  return colCount;
}

void FileCSV::extractLine (string* s)
{
  size_t last_pos;
  size_t pos = s->find(",");

  // Catch a dummy line at the end
  if (pos == string::npos) return;

  geoID->push_back(s->substr(0,pos));

#ifdef DEBUG1
  cout << s->substr(0,pos);
#endif

  for (int i=1;i<numCols;i++) {
    last_pos = pos;
    pos = s->find(",",last_pos+1);

    if (pos == string::npos) {
      data[i-1]->push_back(atof(s->substr(last_pos+1).c_str()));

#ifdef DEBUG1
      cout << "," << s->substr(last_pos+1) << endl;
#endif

    } else {
      data[i-1]->push_back(atof(s->substr(last_pos+1,pos-last_pos-1).c_str()));

#ifdef DEBUG1
      cout << "," << s->substr(last_pos+1,pos-last_pos-1);
#endif
    }
  }
}

/**
 * Gathers the column labels from the input row, a comma-delimited
 * string of column headers.
 */
void FileCSV::gatherColumnLabels (string* row, string* labels)
{
  size_t last_pos;
  size_t pos = row->find(",");

  if (pos == string::npos) return;

  labels[0] = row->substr(0,pos);

  for (int i=1; i < numCols; i++) {
    last_pos = pos;
    pos = row->find(",",last_pos+1);

    if (pos == string::npos) {
      labels[i] = row->substr(last_pos+1);
    } else {
      labels[i] = row->substr(last_pos+1,pos-last_pos-1);
    }
  }
}

/**
 * Since no header row is present, this generates the colum
 * headers in the format "Column X", where X runs from 1 ...
 * numCols.
 */
void FileCSV::gatherColumnLabels (string* labels)
{
  stringstream ss;

  for (int i = 0; i < numCols; i++) {
    ss.str("");

    ss << "Column " << (i+1);

    labels[i] = ss.str();
  }
}

/**
 * Opens the file, init's the private fields, and
 * loads the file data into the class instance.
 */
void FileCSV::importFile (string* filename)
{
  ifstream f;
  f.open(filename->c_str());

  if (!f.is_open())
    throw "Error: could not open CSV file!";

  geoID = new vector<string>;
  char* buf = new char[CSV_BUF_LEN];
  string* s;

  // Read the first line to determine how many columns (beyond GeoID) we have
  f.getline(buf,CSV_BUF_LEN);
  s = new string(buf);

  this->numCols = countColumns(s);

#ifdef DEBUG
  cout << "Num Cols: " << numCols << endl;
#endif
  
  // Init the data storage unit, an array of double vectors
  columnLabels = new string[numCols];
  data = new vector<double>*[numCols];
  for (int i=0;i<numCols;i++)
    data[i] = new vector<double>;

#ifdef DEBUG
  cout << "Is header? " << isHeader(s) << endl;
#endif

  // If the first row are headers, make labels, otherwise
  // make default labels
  if (isHeader(s)) {
    gatherColumnLabels(s,columnLabels);

    delete s;
    f.getline(buf,CSV_BUF_LEN);
    s = new string(buf);
  } else {
    gatherColumnLabels(columnLabels);
  }

#ifdef DEBUG
  cout << "Finished extracting header ..." << endl;
#endif

  extractLine(s);
  delete s;

#ifdef DEBUG1
  int i = 0;
#endif

  // Now read the rest of the data, through EOF
  while (!f.eof()) {
    f.getline(buf,CSV_BUF_LEN);
    s = new string(buf);

#ifdef DEBUG1
    cout << "(" << i << ") " << s << endl;
    i++;
#endif

    extractLine(s);
    delete s;
  }

  f.close();
}

/**
 * Attempts to determine if the first line is a header line. 
 * Since we are expecting a set of numbers, if the first block
 * can be converted character-wise to a string of numbers, and
 * if that string equals the original string, then it is not a 
 * header.  Otherwise it likely is.
 */
bool FileCSV::isHeader (string* line)
{
  size_t pos = line->find(",");

  if (pos == string::npos) return false;

  stringstream ss;
  char c;
  try {
    for (int i = 0; i < pos; i++) {
      c = line->at(i);

      ss << atoi(&c);
    }
  } catch (...) {
    return true;
  }

#ifdef DEBUG
  cout << "isHeader() stringstream: " << ss.str() << endl;
#endif

  if (line->substr(0,pos).compare(ss.str()) == 0)
    return false;
  else
    return true;
}



