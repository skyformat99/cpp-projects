#ifndef __Rotation_h__
#define __Rotation_h__

#include <string>

using namespace std;

class Rotation
{

public:
  Rotation ();
  Rotation (int, string, bool);
  ~Rotation ();

  inline int getID () { return id; }
  inline string getName () { return name; }

  friend std::ostream & operator<< (std::ostream & str, Rotation const & v) { 
    
    str << "[" << v.id << "]: " << v.name << " (isEmpty: " << (v.isEmpty ? "true" : "false") << ")";
    return str;
  }

  Rotation& Rotation::operator= (Rotation const&);

private:

  int id;
  string name;

  bool isEmpty;

};

#endif