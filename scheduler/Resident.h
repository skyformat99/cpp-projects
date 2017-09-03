#ifndef __Resident_h__
#define __Resident_h__

#include <string>
#include <vector>

// Forward declarations
class Constraint; 
class ConstraintRequiredSet;
class Block;
class Rotation;
class ScheduleState;

using namespace std;

class Resident
{

public:

  Resident ();
  Resident (int, string, string);
  ~Resident ();

  friend std::ostream & operator<< (std::ostream & str, Resident const & v) { 
    str << "[" << v.id << "]: " << v.fname << " " << v.lname;
    return str;
  }

  Resident& Resident::operator= (Resident const&);

  void addConstraint (Constraint*);
  string shortString ();
  string htmlString ();

  bool isRequired (int);

  inline int getID () { return id; }
  inline int getNumRequired () { return numRequired; }
  inline int* getRequiredRotations () { return requiredRotations; }
  inline int getNumRequiredSets () { return numRequiredSets; }

  int* getRequiredSetRotations (int, int*);

  bool meetsConstraints (Block*, Rotation*, ScheduleState*);

  void testConstraints ();

private:

  int id;
  string fname;
  string lname;

  vector<Constraint*> constraints;

  int* requiredRotations;
  int numRequired;
  
  int numRequiredSets;
  ConstraintRequiredSet* reqSetConstraint;
};

#endif