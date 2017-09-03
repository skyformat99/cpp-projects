#ifndef __Block_h__
#define __Block_h__

#include <string>

using namespace std;

class Block
{

public:
  Block ();
  Block (int, string, string);
  ~Block ();

  inline int getID () { return id; }
  inline string getStartDate () { return startDate; }
  inline string getEndDate () { return endDate; }

  friend std::ostream & operator<< (std::ostream & str, Block const & v) { 
    str << "[" << v.id << "]: " << v.startDate << " - " << v.endDate;
    return str;
  }

  Block& Block::operator= (Block const&);

private:

  int id;
  string startDate;
  string endDate;

};

#endif