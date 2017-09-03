#include <iostream>
#include <string>

using namespace std;

int main (int argc, char* argv[])
{
  string prefix = "";
  string middle = "";
  string suffix = "";

  bool hasMiddle = false;

  if (argc > 2)
    prefix = argv[2];

  if (argc > 3)
    suffix = argv[3];

  if (argc > 4) {
    hasMiddle = true;

    middle = argv[3];
    suffix = argv[4];
  }

  string full = argv[1];

  unsigned f_dash = full.find("-");
  if (f_dash == string::npos) {
    cout << prefix << full << suffix;

  } else {
    string author = full.substr(0,f_dash);

    while (author.at(author.length()-1) == ' ')
      author = author.substr(0,author.length()-1);    

    if (hasMiddle)
      cout << prefix << author << middle << author << suffix;
    else
      cout << prefix << author << suffix;
  }

  return 0;
}