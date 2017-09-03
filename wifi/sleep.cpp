#include <stdio.h>      /* printf */
#include <Windows.h>

int main ()
{
  int i;
  printf ("Counting for 1 second ...\n");
  
  Sleep(1000);

  printf ("Returned from sleep ...\n");

  return 0;
}