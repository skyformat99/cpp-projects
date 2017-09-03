#include "VectorUtilities.h"

/**
 * Given two string vectors, performs a merge; move all elements (sequentially) which
 * are not already in the dest vector from the src vector to the dest vector. If a src
 * item is already in the dest vector, it is not moved. Returns the total number of
 * elements added to the dest vector.
 */
int VectorUtilities::mergeUniqueElements (vector<string> *dest, vector<string> *src) 
{
  int numAdded = 0;
  bool isPresent = false;

  vector<string>::iterator it;
  for ( it = src->begin() ; it < src->end(); it++ ) {

    vector<string>::iterator dest_it;
    for ( dest_it = dest->begin() ; dest_it < dest->end(); dest_it++ ) {

      if (*it == *dest_it) {
        isPresent = true;
        break;
      }
    }

    if (!isPresent) {
      numAdded++;
      dest->push_back(*it);
    }

    isPresent = false;
  }

  return numAdded;
}