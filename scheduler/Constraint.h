#ifndef __Constraint_h__
#define __Constraint_h__

#include <string>
#include <vector>

// Forward declarations
class ResidentPool; 
class Resident;
class Block;
class Rotation;
class ScheduleState;

using namespace std;

class Constraint
{

public:

  enum ConstraintType { TYPE_NULL = -1, 
                        TYPE_REQUIRED = 0, 
                        TYPE_REPEAT = 1,
                        TYPE_REQUIRED_SET = 2 };

  Constraint ();
  Constraint (ConstraintType);
  ~Constraint ();

  virtual bool isValid (Resident*, Block*, Rotation*, ScheduleState*) = 0;

//  Constraint& Constraint::operator= (Constraint const&);

  inline ConstraintType getType () { return type; }

  static void loadFromFile (string, ResidentPool*);
  string printType ();

protected:

  ConstraintType type;

private:

  static Constraint* resolveConstraint (vector<string>&);

};

#endif