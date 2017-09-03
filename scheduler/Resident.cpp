#include "Resident.h"

#include <iostream>
#include <string>
#include <sstream>

#include "Constraint.h"
#include "ConstraintRequired.h"
#include "ConstraintRequiredSet.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Resident::Resident ()
{
  // Default constructor
}

Resident::Resident (int id, string fn, string ln)
{
  this->fname = fn;
  this->lname = ln;
  this->id = id;

  this->numRequired = 0;
  this->requiredRotations = NULL;

  this->numRequiredSets = 0;
  this->reqSetConstraint = NULL;
}

Resident::~Resident ()
{
  // Destroy
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void Resident::addConstraint (Constraint* c)
{
  constraints.push_back(c);

  // If we are adding the required rotations constraint, grab them!
  if (c->getType() == Constraint::TYPE_REQUIRED) {
    requiredRotations = ((ConstraintRequired*)c)->getRequiredRotations();
    numRequired = ((ConstraintRequired*)c)->getNumRequired();
  }

  if (c->getType() == Constraint::TYPE_REQUIRED_SET) {
    numRequiredSets = ((ConstraintRequiredSet*)c)->getNumRequiredSets();
    reqSetConstraint = (ConstraintRequiredSet*)c;
  }
}

int* Resident::getRequiredSetRotations (int index, int* numVals)
{
  int cnt = reqSetConstraint->getSetRotationCount(index);
#ifdef DEBUG2
  cout << "----- getRequiredSetRotations: # in rotation: " << cnt << endl;
#endif
  (*numVals) = cnt;
  return reqSetConstraint->getSetRotations(index);
}

string Resident::htmlString ()
{
  stringstream ss;

  ss << lname << ",&nbsp;" << fname;

  return ss.str();
}

bool Resident::isRequired (int oID)
{
  for (int i=0; i<numRequired; i++) {
    if (requiredRotations[i] == oID) return true;
  }

  return false;
}

bool Resident::meetsConstraints(Block* b, Rotation* o, ScheduleState* state)
{
  bool r = true; 
  for (vector<Constraint*>::iterator it = constraints.begin() ; it != constraints.end(); ++it)
    r &= (*it)->isValid(this, b, o, state);

#ifdef DEBUG1
  cout << "---- Resident constraints: " << ((r) ? "yes" : "no") << endl;
#endif

  return r;
}

Resident& Resident::operator= (Resident const& r) 
{
  this->fname = r.fname;
  this->lname = r.lname;
  this->id = r.id;

  this->numRequired = r.numRequired;
  this->requiredRotations = r.requiredRotations;

  this->numRequiredSets = r.numRequiredSets;
  this->reqSetConstraint = r.reqSetConstraint;

  this->constraints = r.constraints;

  return *this;
}

string Resident::shortString ()
{
  stringstream ss;

  ss << lname << ", " << fname;

  return ss.str();
}

void Resident::testConstraints ()
{
  cout << "Num constraints: " << constraints.size() << endl;

  Constraint* c;
  for (int i=0; i<constraints.size(); i++) {
    c = constraints[i];

    cout << "  " << i << ": ";
    if (c == NULL) 
      cout << "NULL" << endl;
    else {
      cout << "NOT NULL (Type: " << c->printType() << ")" << endl;
    }
    
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *
