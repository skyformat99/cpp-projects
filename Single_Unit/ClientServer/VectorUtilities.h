#ifndef __vectorUtilities
#define __vectorUtilities

#include <vector>
#include <string>

using namespace std;

class VectorUtilities {

  private:

    VectorUtilities();
    ~VectorUtilities();

  public:

    static int mergeUniqueElements (vector<string>*, vector<string>*);

};

#endif