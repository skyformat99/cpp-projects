#include <iostream>
#include <sstream>
#include <string>

#include "Constraint.h"
#include "ConstraintRequired.h"
#include "ConstraintRequiredSet.h"
#include "ConstraintRepeat.h"
#include "PropertyFile.h"
#include "ResidentPool.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// CONSTRUCTION / DESTRUCTION
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Constraint::Constraint ()
{
  this->type = TYPE_NULL;
}

Constraint::Constraint (ConstraintType type)
{
  this->type = type;
}

Constraint::~Constraint ()
{
  // Do nothing
}

// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

void Constraint::loadFromFile (string fn, ResidentPool* vp)
{
  PropertyFile* p = new PropertyFile(fn,",");

  ResidentPool* rp = static_cast<ResidentPool*>(vp);

  Resident* r; Constraint* c;
  vector<string> v; int i;
  for (i=0; i<p->length(); i++) {
    v = p->tokenizeToVector(i);

    try {
      r = rp->getResident(atoi(v[0].c_str()));
    } catch (...) {
      throw exception("Constraint file contains invalid resident identifier");
    }

    try {
      c = Constraint::resolveConstraint(v);
    } catch (...) {
      throw exception("Could not resolve constraint from constraints file");
    }

//    cout << "loadFromFile bottom: " << c.printType() << endl;

    r->addConstraint(c);
  }
}

string Constraint::printType ()
{
  stringstream ss;

  switch (this->type) {

    case Constraint::TYPE_NULL: 
      ss << "TYPE_NULL";
      break;

    case Constraint::TYPE_REQUIRED:
      ss << "TYPE_REQUIRED";
      break;

    case Constraint::TYPE_REPEAT:
      ss << "TYPE_REPEAT";
      break;

    case Constraint::TYPE_REQUIRED_SET:
      ss << "TYPE_REQUIRED_SET";
      break;

    default:
      ss << "Unknown Type! [" << this->type << "]";
      break;
  }

  return ss.str();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * *
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * *

Constraint* Constraint::resolveConstraint (vector<string>& v) 
{
  Constraint* c = NULL;
  switch (atoi(v[1].c_str())) {
 
    case Constraint::TYPE_REQUIRED:
      c = (Constraint*)(new ConstraintRequired(Constraint::TYPE_REQUIRED, v[2]));
      break;

    case Constraint::TYPE_REPEAT:
      c = (Constraint*)(new ConstraintRepeat(Constraint::TYPE_REPEAT));
      break;

    case Constraint::TYPE_REQUIRED_SET:
      c = (Constraint*)(new ConstraintRequiredSet(Constraint::TYPE_REQUIRED_SET, v));
      break;

    default:
      throw exception("Invalid constraint type in constraint file");
  }

  return c;
}