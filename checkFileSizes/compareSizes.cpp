#include <iostream>

using namespace std;

void main (int argc, char* argv[])
{
  double fs1 = atof(argv[1]);
  double fs2 = atof(argv[2]);

  double ff = atof(argv[3]);

  if ((fs2 <= fs1 + fs1 * ff) &&
      (fs2 >= fs1 - fs1 * ff))
    printf("SET %s=true",argv[4]);
  else
    printf("SET %s=false",argv[4]);

  exit(0);
}