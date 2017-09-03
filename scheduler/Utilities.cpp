#include <iostream>
#include <fstream>

#include "Utilities.h"
#include "BlockPool.h"
#include "ScheduleState.h"
#include "ResidentPool.h"
#include "RotationPool.h"
#include "Block.h"
#include "Resident.h"
#include "Rotation.h"

vector<string> Utilities::tokenizeToStringVector (string s, string sep)
{
  vector<string> v;

  size_t pos = s.find(sep);

  int offset = 0, j = 0;
  while (pos != string::npos) {
    v.push_back( s.substr(offset,pos-offset) );

    offset = (int)pos + 1;
    pos = s.find(sep, offset);
  }

  v.push_back( s.substr(offset) );

  return v;
}

vector<int> Utilities::tokenizeToIntVector (string s, string sep)
{
  vector<int> v;

  size_t pos = s.find(sep);

  int offset = 0, j = 0;
  while (pos != string::npos) {
    v.push_back( atoi( s.substr(offset,pos-offset).c_str() ) );

    offset = (int)pos + 1;
    pos = s.find(sep, offset);
  }

  v.push_back( atoi (s.substr(offset).c_str() ) );

  return v;
}

void Utilities::toResidentWebTable (ScheduleState* state, ResidentPool* rp, 
         BlockPool* bp, RotationPool* op, string fn, bool append)
{
  ofstream fs; int i, j;

  if (append)
    fs.open (fn, ios::out | ios::app);
  else
    fs.open (fn, ios::out);

  // Write file header
  fs << "<HTML><HEAD><TITLE>" << "Block Schedule" << "</TITLE>" 
     << "<STYLE TYPE='text/css'>" << endl
     << "body { font-size: 75%; font-family:arial; font-variant:normal; }" << endl
     << "td { font-size: 75%; font-family:arial; font-variant:normal; }" << endl
     << "pre { font-family:arial; font-variant:normal }" << endl
     << "em { font-style: normal; color: blue }" << endl
     << "td.gc { color: gray; text-align: center }" << endl
     << "@media screen {" << endl
     << "tr.grbg { background:#f8f8f8 }" << endl
     << "td.hl1  { background:#8cffff }" << endl 
     << "}" << endl
     << "@media print {" << endl
     << "tr.grbg { background:#f0f0e6 }" << endl
     << "td.hl1  { background:#28ffff }" << endl
     << "}" << endl
     << "a.plain	  { COLOR: #000000; text-decoration: none }" << endl
     << "a.plain:hover  { COLOR: #000077; text-decoration: underline }" << endl
     << "#dek {position:absolute;visibility:hidden;z-index:200;}" << endl
     << "</STYLE><style type='text/css' media=print> .hideprint { display:none; } </style></HEAD>" << endl
     << "<body link=#000080 vlink=#000080 bgcolor=white text=black topmargin=8 leftmargin=8>" 
     << "<table cellpadding=0 cellspacing=0><tr>" 
     << "<td valign=top style='line-height:135%; font-size: 100%; text-align:left'>" << endl;

  fs << "<font size=+2>Block Schedule</font><br>" 
     << "<br style='line-height:10px;'>" << endl;

  // Write table header
  fs << "<table cellspacing=0 border=1 cellpadding=2 style='border-color:#cccccc; border-collapse:collapse;'>" << endl;
  
  // Write top row header
  fs << "<TR BGCOLOR=#f6deac>" << endl;
  fs << "<td style='border-color:#cccccc; border-collapse:collapse;'></TD>" << endl;

  Block* b;
  for (i=0; i<bp->length; i++) {
    b = bp->getBlock(i);

    fs << "<td style='border-color:#cccccc; border-collapse:collapse;'>" << i+1 << "<br>"
       << b->getStartDate() << "&nbsp;-&nbsp;" << b->getEndDate() << "<br></TD>" << endl;
  }

  fs << "</tr>" << endl;

  // Now write rows for each resident
  Resident* r; Rotation* o;
  for (i=0; i<rp->length; i++) {
    r = rp->getResident(i);

    fs << ((i % 2 == 0) ? "<TR class=grbg>" : "<TR>")
       << "<td style='border-color:#cccccc; border-collapse:collapse;'>"
       << r->htmlString() << "</TD>" << endl;
    
    for (j=0; j<bp->length; j++) {
      o = state->getFilledRotation(i,j);

      fs << "<td style='border-color:#cccccc; border-collapse:collapse;'>";

      if (o == NULL) 
        fs << "EMPTY";
      else
        fs << o->getName();

      fs << "</TD>" << endl;
    }

    fs << "</tr>" << endl;
  }
  fs << "</table></td></tr></table></BODY></HTML>" << endl;

  fs.close();
}