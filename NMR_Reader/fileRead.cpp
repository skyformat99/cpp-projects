#include <cstdlib>
#include <fstream>
#include <iostream>
#include <strstream>
#include <string>
#include <vector>

using namespace std;

/** Define custom structure and functions **/
struct restraint {
  int count;
  int* vals;
  string str;
};
struct dot_match {
  int dot_x;
  int dot_y;
  restraint* r;
};
string removeCharacter (string*, const char);
string loadSequence (const char*);
void loadRestraints (const char*, vector<restraint>*);
void recursiveMatch (restraint, int, int**, int, int, int, vector<dot_match>*);

/**
 * Main function executed in this file.
 */
int main (int argc, char* argv[]) {

  // Load a sequence given a filename
  // Can be converted into char* using seq.c_str() method
  string seq = loadSequence("seq.txt");

  cout << "Sequence: " + seq + "\n";

  // Declare a new vector, holding type 'restraint'
  // and run the loadRestraints method, passing the
  // pointer to the vector as well as the text of the filename.
  // By putting a '&' before the variable name, we're passing the
  // address of the variable. This turns the arguement into a 
  // pointer to the vector, rather than just the vector.
  vector<restraint> v;
  loadRestraints("nmr.txt",&v);

  // To prove that we have the vector of elements,
  // we will iterate through them (from 0 -> v.size) and
  // print them to the console. We print the length, the
  // string value stored, and then iterate through the int* 
  // array and print the integer values to make sure they
  // match the strings (by visual inspection).
  int i,j;
  restraint r;
  for (i=0;i<v.size();i++) {
    r = v[i];
    printf("(%d) Size: %d Str: %s Int Vals: ",i,r.count,r.str.c_str());

    for (j=0;j<r.count;j++)
      printf("%d",r.vals[j]);

    printf("\n");
  }

  // This is how we would call the findMatches method. Note that since
  // I don't have an example dotplot array here I can't acually call it,
  // so it's commented out. 
  vector<dot_match> dm;
  // findMatches(&v,convertedDotplot,sequenceLength,&dm);
}


/**
 * Find matches through the dotplot given the restraints vector.
 * Given the vector of the restraints, and the 2D integer array
 * which contains the dot plot information, plus a pointer to the
 * vector of match results (which initially is empty), this function 
 * will go through the entire dot plot and look for initial matches.
 * Once an initial match is found, it will pass off processing to 
 * the recursiveMatch function, which will populate the dot_match results
 * vector on successful full matches.
 */
void findMatches (
  vector<restraint>* v_r, 
  int** convertedDotplot,
  int sequenceLength, 
  vector<dot_match>* results
) {

  int i,j,k;
  restraint r;

  // Loop through all restraints present
  for (k=0;k<v_r->size();k++) {

    // Get the current restraint structure
    r = v_r->at(k);
    
    // Now through x,y of dot plot
    for (j=0;j<sequenceLength;j++) {
      for (i=0;i<sequenceLength;i++) {

        // Check the first value only here - if this value matches,
        // then we call the recursive method, passing it the proper
        // parameters
        if (r.vals[0] == convertedDotplot[i][j]) 
          recursiveMatch(r,0,convertedDotplot,sequenceLength,i,j,results);
      }
    }
  }
}


/**
 * Recursively match the restraints on an angle through
 * the dotplot. 
 */
void recursiveMatch (
  restraint r, 
  int currentRestraintIndex,
  int** convertedDotplot,
  int sequenceLength,
  int last_dot_x,
  int last_dot_y,
  vector<dot_match>* results
) {

  // If the count of the restraint is equal to the next
  // currentRestraintIndex + 1, then we've won! We need to add
  // this match to the dot_match vector and return.
  if (r.count == currentRestraintIndex+1) {
    dot_match* dm = new dot_match;

    dm->dot_x = last_dot_x;
    dm->dot_y = last_dot_y;
    dm->r = &r;

    results->push_back(*dm);

    return;
  }

  // Now check if we're still in-bounds on the next one down.
  // If we're not, simply return.
  if (last_dot_x - 1 < 0 || last_dot_y + 1 >= sequenceLength)
    return;

  // Lastly, check to see if the next one down will match the next
  // value. If it does, then call again after updating parameters.
  if (r.vals[currentRestraintIndex+1] == 
          convertedDotplot[last_dot_x-1][last_dot_y+1]) {

    recursiveMatch(r,currentRestraintIndex+1,convertedDotplot,
      sequenceLength,last_dot_x-1,last_dot_y+1,results);
  }

  // If not a match, just return
  return;
}


/**
 * Given the filename to the restraints file and the address
 * of the vector of restraint elements, this function will
 * open the file, load in line-by-line the restraints, and
 * add them to the vector. 
 *
 * We require the address of the vector here because this function
 * does not return anything; the vector itself is populated, so as 
 * long as the calling function has the address of the vector it
 * will have access to the data it contains.
 *
 * Note we must also be careful about how that data is put into the
 * vector. On every line of the 'while' loop below we have to allocate
 * a new 'restraint' structure, so that the element inserted is new and
 * not simply a copy of the previous elements.
 */
void loadRestraints (
  const char* restFilename, 
  vector<restraint>* vec
) {

  ifstream inRestFile;
  inRestFile.open(restFilename);

  // Check to make sure restraints file is open
  if (!inRestFile) {
    cerr << "Unable to open restriants file";
    exit(1);
  }

  int strLen, i;
  string s;
  restraint* r;

  // Loop through each line of the file
  while (getline(inRestFile,s)) {
    strLen = s.length();

    // Allocate memory for a new restraint, returning the pointer
    r = new restraint;
    r->count = s.length();
    r->str = s;

    // Convert each string element into a number and put in array
    // Use the 'atoi' function, which takes a char*. In order to 
    // get a char* and not just a char (which we could've used
    // the method s.at(i) to get), we have to pull the substring
    // of one character, and convert to a c_str.
    r->vals = (int*)malloc(sizeof(int)*r->count);
    for (i=0;i<r->count;i++) {
      r->vals[i] = atoi(s.substr(i,1).c_str());
    }

    // 'Push' or add the element (not the pointer) to the vector
    vec->push_back(*r);
  }

  return;
}


/**
 * Loads the sequence from a file given by seqFilename
 * into a single string. Removes all whitespace and linebreaks
 * from the file. Returns the string representation.
 */
string loadSequence (
  const char* seqFilename
) {

  ifstream inSeqFile;
  inSeqFile.open(seqFilename);  

  // Check to make sure file's open
  if (!inSeqFile) {
    cerr << "Unable to open sequence file";
    exit(1);
  }

  // First load the sequence line-by-line into a single string,
  // retaining all whitespaces
  string s;
  string seqTemp;
  while (getline(inSeqFile,s)) {
    seqTemp.append(s);
  }

  inSeqFile.close();

  // Removes all spaces and tab's
  // (Can add ability to remove other characters here too)
  string seq;
  seq = removeCharacter(&seqTemp,' ');
  seq = removeCharacter(&seq,'\t');

  // Return string
  return seq;
}


/**
 * Removes characters from the input string (passed by pointer).
 * Loops through the string, and removes each character c by
 * appending everything before and after to a new string.
 * Returns the character-removed string.
 */
string removeCharacter (
  string* inStr, 
  const char c
) {

  string outStr;
  int lastInd = 0;
  int ind;

  // Loop through and find index of next character, and
  // remove it
  while ((ind = inStr->find_first_of(c,lastInd)) != -1) {
    outStr.append(inStr->substr(lastInd,ind-lastInd));
    lastInd = ind+1;
  }

  // Append the last segment to the return string
  outStr.append(inStr->substr(lastInd));

  return outStr;
}