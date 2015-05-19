#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "cache.h"

using namespace std;

static Core c;
static int cycles = 0, references = 0, basecycles = 0;

extern void tune_parameters(Core *c);

/* Runs in two modes:
   -t (test mode) : Tests different configurations of the cache.
   -r (real mode) : Uses tuned parameters to evaluate the cache design/strategy proposed.
*/
int main(int argc, char *argv[])
{
  if (argc < 2 || string(argv[1]) == "-h" || string(argv[1]) == "--help") {
    cout << "Usage: cache-sim < -t <L1Size> <L1Assoc> <L1BlockSize>"
            "<victimSize> <VictimAssoc> <VictimBlockSize> <L2Size> <L2Assoc> <L2BlockSize> | -r >\n";
    return -1;
  }

	/* Read args. */
	if (string(argv[1]) == "-t") {
		c.setL1Attributes(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
		c.setVictimAttributes(atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
		c.setL2Attributes(atoi(argv[8]), atoi(argv[9]), atoi(argv[10]));
		c.setEvictionPolicy(0); //default - LRU for all caches.
	} else {
		/* Tune parameters. */
		tune_parameters(&c);
		c.finalize();
	}
	/* Check size. */
	if (c.size() > 300*1024) {
	  cout << "Budget exceeded\n";
	  return -2;
	  
	}

	/* Read traces and record cycles. */
  int address, loadstore, icount;
  char marker;
  while (scanf("%c %d %x %d\n",&marker,&loadstore,&address,&icount) != EOF) {
    if (marker == '#')
      references++;
    else { 
      continue;
    }
    cycles += icount;
    basecycles += icount;
    cycles += c.accessCache(loadstore, address, cycles);
  }
  c.printcacheconfig();
  printf(" Total size of all caches %d bytes\n", c.size());
  printf("Total CPI=%.2f\n", (float) 1.0 + (cycles - basecycles) / (float) basecycles);
  printf("Average memory access time=%.2f\n",  (float) (cycles - basecycles) / (float) references);
  printf("L1 Miss Rate=%.2f\n", c.getL1MissRate());
  printf("Victim Local Miss Rate=%.2f\n", c.getVictimLocalMissRate());
  printf("L2 Local Miss Rate=%.2f\n", c.getL2LocalMissRate());
  printf("L2 Global Miss Rate=%.2f\n", c.getL2GlobalMissRate());
}
