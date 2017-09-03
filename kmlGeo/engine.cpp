#include "FileCSV.h"
#include "State.h"

using namespace std;

/**
 * KML Generator from US Census Data
 *
 * This program will generate a KML file coding each US Census Tract 
 * with a different color based on a value assigned to that tract in a
 * CSV file - typically exported by Excel from US Census data.
 *
 * Some informational links for this data:
 *
 * http://code.google.com/apis/kml/documentation/kmlelementsinmaps.html 
 *   - KML 2.2 Reference
 * 
 * http://groups.google.com/group/kml-support-getting-started/browse_thread/thread/d6b966854e546cc8?pli=1 
 *   - Using styles on the maps
 *
 * http://www.census.gov/geo/www/cob/tr2000.html 
 *   - Census Tract Lat/Long Data Files - need Ungenerate ASCII format
 * 
 * http://factfinder.census.gov/home/saff/main.html?_lang=en 
 *   -> Download Center -> ACS 5-year Estimates -> All Census Tracts by State, Selected Detailed Tables
 *
 * http://www.census.gov/geo/www/cob/ascii_info.html 
 *   - ASCII data file format
 *
 * GEO_ID2 in the data tables consists of:
 *   <STATE_ID> <COUNTY_ID> <CENSUS_TRACT_ID>
 *   which can be found in the tr<state_id>_d00a.dat file of the ASCII format dataset. 
 *   Each entry in that file is in the following:
 *     <ID>
 *     <STATE_ID>
 *     <COUNTY_ID>
 *     <CENSUS_TRACT_ID>
 *     <CENSUS_TRACT_NAME>
 *     "TR"
 *
 * ***********************************
 * *** How to use this information ***
 * ***********************************
 *
 * - Download the data tables of interest from the American Factfinder Download
 *   center, for the state of interest.
 *
 *   - Most useful:
 *  
 *     B02001. Race
 *       Universe:  TOTAL POPULATION: Total (Estimate)
 *       Universe:  TOTAL POPULATION: White alone (Estimate)
 *       Universe:  TOTAL POPULATION: Black or African American alone (Estimate)
 *     B08303. Time Travel to Work
 *       Universe:  WORKERS 16 YEARS AND OVER WHO DID NOT WORK AT HOME: Total (Estimate)
 *       Universe:  WORKERS 16 YEARS AND OVER WHO DID NOT WORK AT HOME: Less than 5 minutes (Estimate)
 *       Universe:  WORKERS 16 YEARS AND OVER WHO DID NOT WORK AT HOME: 5 to 9 minutes (Estimate)
 *       Universe:  WORKERS 16 YEARS AND OVER WHO DID NOT WORK AT HOME: 10 to 14 minutes (Estimate)
 *       Universe:  WORKERS 16 YEARS AND OVER WHO DID NOT WORK AT HOME: 15 to 19 minutes (Estimate)
 *     B15002. Sex by Educational Attainment for the Population 25 Years and Over
 *       Universe:  POPULATION 25 YEARS AND OVER: Total (Estimate)
 *       Universe:  POPULATION 25 YEARS AND OVER: Male; Bachelor's degree (Estimate)
 *       Universe:  POPULATION 25 YEARS AND OVER: Male; Master's degree (Estimate)
 *       Universe:  POPULATION 25 YEARS AND OVER: Male; Professional school degree (Estimate)
 *       Universe:  POPULATION 25 YEARS AND OVER: Male; Doctorate degree (Estimate)
 *       Universe:  POPULATION 25 YEARS AND OVER: Female; Bachelor's degree (Estimate)
 *       Universe:  POPULATION 25 YEARS AND OVER: Female; Master's degree (Estimate)
 *       Universe:  POPULATION 25 YEARS AND OVER: Female; Professional school degree (Estimate)
 *       Universe:  POPULATION 25 YEARS AND OVER: Female; Doctorate degree (Estimate)
 *     B19013. Median Household Income
 *       Universe:  HOUSEHOLDS: Median household income in the past 12 months (in 2009 inflation-adjusted dollars) (Estimate)
 *     B19054. Interest, Dividends or Net Rental Income for Households
 *       Universe:  HOUSEHOLDS: Total (Estimate)
 *       Universe:  HOUSEHOLDS: With interest, dividends, or net rental income (Estimate)
 *     B19056. Supplemental Security Income (SSI) for Households
 *       Universe:  HOUSEHOLDS: Total (Estimate)
 *       Universe:  HOUSEHOLDS: With Supplemental Security Income (SSI) (Estimate)
 *     B25018. Median Number of Rooms
 *       Universe:  HOUSING UNITS: Median number of rooms (Estimate)
 *     B25035. Median Year Structure Built
 *       Universe:  HOUSING UNITS: Median year structure built (Estimate)
 *     B25076. Lower Value Quartile (Dollars) for Owner-Occupied Housing Units
 *       Universe:  OWNER-OCCUPIED HOUSING UNITS: Lower value quartile (dollars) (Estimate)
 *     B25077. Median Value (Dollars) for Owner-Occupied Housing Units
 *       Universe:  OWNER-OCCUPIED HOUSING UNITS: Median value (dollars) (Estimate)
 *     B25078. Upper Value Quartile (Dollars) for Owner-Occupied Housing Units
 *       Universe:  OWNER-OCCUPIED HOUSING UNITS: Upper value quartile (dollars) (Estimate)
 *
 * - Download the ASCII Census Tract Lat/Long data file for the state of interest.
 *   Unzip into the kmlGeo directory.
 *
 * - Open Excel and select the _data1.txt file you just downloaded.  Select 'Delimited', and
 *   delimit with a '|' character.
 *
 * - Values to compute:
 *   % Caucasian (# white / total population)
 *   % AA (# black / total population)
 *   % Workers < 20 min commute (sum of # with commute < 20 / total workers)
 *   % Pop > 25 with Doctorate (sum males, females with doctorate / population)
 *   % Households w Interest Div Income (# w/ int div inc / total households)
 *   % Households w SSI (# w/ SSI / total households)
 *
 * - Copy second column (GEO_ID2) into new worksheet, along with all columns
 *   of interest and computed columns. Delete the top row header, but keep the
 *   header with the column labels. Save as a new .xlsx file (ACS 2009 Data - XX.xlsx).
 *
 * - Delete the top header row, and save as a .csv. Move the .csv into the kmlGeo folder.
 *
 * - Run 'engine.exe' and pass it this .csv file, the output file name (output.kml), and the
 *   color bounds.
 *
 *   - Good color bounds: FF000066 FF0000FF (dark red -> light red)
 *
 *   - Specify the column of interest by counting (including the Geo_ID column) in the .xlsx
 *     file.  Choose the min, max and # bins.  The output file (outfile.kml) will be written.
 *
 * - Open the resulting .kml file in Google Earth.  The transparency can be adjusted using 
 *   the slider bar. 
 *
 * @author Adam Dziorny
 * @version 01.03.2011
 */
int main (int argc, char* argv[]) 
{
  if (argc < 3) {
    cerr << "Usage: engine <csv_file> <outfile> [<begin_color> <end_color>]" << endl;
    return -1;
  }

  string* fn = new string(argv[1]);
  string* outfile = new string(argv[2]);

  FileCSV* f = new FileCSV(fn);

#ifndef QUIET
  f->displayInfo();

  cout << "Columns::" << endl;
  f->displayHeaders(true);
#endif

  int col;
#ifndef QUIET
  cout << "Enter the desired column: ";
#endif
  cin >> col;

  // To adjust for the GeoID column and the 0/1 offset
  col -= 2;

  int nbins;
  double min, max, mean, stdev;
  f->columnStats(col,&min,&max,&mean,&stdev,&nbins);
#ifndef QUIET
  cout << "Min: " << min << " Max: " << max 
       << " Mean: " << mean << " Stdev: " << stdev 
       << " nBins: " << nbins << endl;
#endif

  int numBins;
#ifndef QUIET
  cout << "Enter minimum value: ";
#endif
  cin >> min;

#ifndef QUIET
  cout << "Enter maximum value: ";
#endif
  cin >> max;

  if (max <= min) {
    cerr << "Error: maximum value <= minimum value!" << endl;
    exit(-1);
  }

#ifndef QUIET
  cout << "Enter number of bins desired: ";
#endif
  cin >> numBins;

  string stateCode = f->getStateCode();

#ifndef QUIET
  cout << "State code: " << stateCode << endl;
#endif

  StyleGroup* sg;

  string begin;
  string end;
  if (argc >= 5) {
    begin = string(argv[3]);
    end = string(argv[4]);
  } else {
    begin = string("FFFF0000");
    end = string("FF0000FF");
  }

#ifndef QUIET
  cout << "Colors:: Begin: " << begin << " End: " << end << endl;
#endif

  sg = new StyleGroup(begin,end,numBins,min,max);  

  State* s;

  if (!State::does_exist(&stateCode)) {
    cerr << "Error: State data files do not exist for this csv file data!" << endl;
    return -1;
  }

  s = new State(&stateCode);

  s->setStyleGroup(sg);

#ifndef QUIET
  s->displayInfo();
#endif

  // Now we have to mesh the data in column 'col' of FileCSV
  // with the State's GeoElem vector, simply by setting
  // the appropriate 'Style' element to each GeoElem instance.
  
  // Now for each row in FileCSV (maybe using [] operator?), 
  //   call findGeoElem in state (using a binary search algorithm
  //   in the <algorithm> library and a comparator function
  //   to compare the FileCSV element with the GeoElem.

  int numRows = f->size();
  int geoIndex;
  int res;

  for (int i=0; i<numRows; i++) {

#ifndef QUIET
    if (i % 50 == 0) cout << "** INDEX == " << i << " **" << endl;
#endif

    res = s->findGeoElem(&geoIndex,f->at(i));

    if (res == 0) {
#ifndef QUIET
      cerr << "Could not find GeoID " << f->at(i) << endl;
#endif

    } else {

      s->setMetaData(geoIndex,f->at(i,col));
      sg->setElementStyle(f->at(i,col),s,geoIndex);

#ifdef DEBUG4
      s->displayElem(geoIndex);
#endif
    }
  }

  // Then these can be written out correctly.
#ifndef QUIET
  cout << "Writing file " << (*outfile) << endl;
#endif
  s->writeTestFile((char*)outfile->c_str());

#ifndef QUIET
  cout << "Successfully wrote file!" << endl;
#endif

/**
  try {
    delete fn;
    delete outfile;
    delete f;
    delete sg;
    delete s;
  } catch (...) {}

  exit(0); **/
}
