#include <string>
#include <sstream>

#include "VideoUnit.h"

VideoUnit :: VideoUnit (int vtsNumber, int numChapters,
  int pgcNumber, string pgcDurationString, double pgcDurationMin)
{
  this->vtsNumber = vtsNumber;
  this->numChapters = numChapters;
  this->pgcNumber = pgcNumber;
  this->pgcDurationString = pgcDurationString;
  this->pgcDurationMin = pgcDurationMin;
}

int VideoUnit :: getVTS ()
{
  return vtsNumber;
}

int VideoUnit :: getPGC ()
{
  return pgcNumber;
}

double VideoUnit :: getDurMin ()
{
  return pgcDurationMin;
}

string VideoUnit :: toString ()
{
  stringstream res;
  res.str("");

  res << "VTS # " << vtsNumber;
  res << " PGC # " << pgcNumber;
  res << " # Chapters: " << numChapters;
  res << " Duration: " << pgcDurationString;
  res << " [" << pgcDurationMin << " min]";

  return res.str();
}

