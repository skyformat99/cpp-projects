#include <stdio.h>
#include <stdlib.h>

#include "PooledSort.h"
#include "SpikeList.h"

using namespace std;

/**
 * Loads in the SpikeList .mat & .txt file.
 */
int main (int argc, char* argv[]) {

  // Verify input arguments
  if (argc < 4) {
    fprintf(stderr,"Usage: %s <channel> <filename-base> <pool_file>\n",argv[0]);
    exit(1);
  }

  int channel = atoi(argv[1]);

  char errbuf[256];

  // Initialize a new PooledSort object
  PooledSort* ps;

  try {
    ps = new PooledSort(argv[3]);
  } catch (char* c) {
    cerr << "Error loading PooledSort: " << c << endl;
    exit(1);
  } catch (...) {
    cerr << "Unknown error loading PooledSort!" << endl;
    exit(1);
  }

  int result, numSorts;
  int *sorts;

  numSorts = ps->getNumSorts(channel);
  sorts = new int[numSorts];

//  sorts = (int*)malloc(numSorts*sizeof(int));

  result = ps->getSorts(channel,sorts,numSorts);

  // Initialize a new SpikeList object
  SpikeList* s;

  try {
    s = new SpikeList(channel,argv[2],sorts,numSorts);
  } catch (char* c) {
    cerr << "Error loading SpikeList: " << c << endl;
    exit(1);
  } catch (...) {
    cerr << "Unknown error loading SpikeList!" << endl;
    exit(1);
  }

  int spikesPerSort[3];
  s->getSpikesPerSort(spikesPerSort);

  printf("Number of spikes: %d\n",s->getNumSpikes());

  printf("  Sort 0: %d\n  Sort 1: %d\n  Sort 2: %d\n",
    spikesPerSort[0],spikesPerSort[1],spikesPerSort[2]);

  int i, spikes, count = 0;
  for (i=0;i<s->getNumBlocks();i++) {
    spikes = s->getNumSpikes(i,1);
    count += spikes;
    printf("- Block %d: numSpikes: %d cumulative: %d\n",i+1,spikes,count);
  }

  if (s->isTimeMonotonic())
    printf("Spike times increase monotonically!\n");

  exit(0);
}