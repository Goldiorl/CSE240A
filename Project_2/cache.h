#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>

using namespace std;

class Cache {
	int size; // size in bytes
	int assoc; // associativity
	int blockSize; // cache block size
	int hitLatency; // hit latency

	public:
		int **tag; // maintains tags
		int **accessed; // maintains last access time, frequency etc
										// depends on the eviction policy you choose

		Cache() { }
		/* Initialize. */
		void setAttributes(int sz, int asc, int blk) {
			size = sz;
			assoc = asc;
			blockSize = blk;
			if (size < 2048) {
				hitLatency = ceil(sz*asc/2048);
			} else {
				hitLatency = ceil(sz/(16*1024)) + (asc-1) + ceil(log2(blk/16));
			}
			if (sz != 0 && blk != 0) {
				tag = new int*[sz/(blk*asc)]; 
				accessed = new int*[sz/(blk*asc)];
				for (int i=0; i<(sz/(blk*asc)); i++) {
					tag[i] = new int[asc];
					accessed[i] = new int[asc];
				}
			}
		}

		/* Getters. */
		int getSize() { return size;}
		int getAssociativity() { return assoc;}
		int getBlockSize() { return blockSize;}
		int getHitLatency() { return hitLatency; }

		// To be implemented
		bool isHit(int ldst, int addr, int cycles);
};

class Core {
	Cache L1;
	Cache L2;
	Cache Victim;
	int evictionPolicy; // Defaults to 0 (LRU). You may use your favorite 
                      // eviction policy in real mode (or use it for
                      // just book keeping or any other optimizations
                      // you think might be useful).
	int L1L2Transfer; // Time taken to go from L1 to L2.
	int memoryAccessTime; // Time taken to access main memory.
	bool finalized;
	int l1Accesses;
	int l2Accesses;
	int victimAccesses;
	int totalMemRef;
	int l1Misses;
	int l2Misses;
	int victimMisses;
	public:
		/* Initialization. */
		Core() {memoryAccessTime = 350; L1L2Transfer = 15; finalized = false;}

		/* No more changes to the configuration is allowed 
		 * after this gets called. */
		void finalize() { finalized = true; }

		/* Budget calculation. */
		int size() { return L1.getSize() + L2.getSize() + Victim.getSize(); }

		/* More initialization. */
		void setL1Attributes(int size, int assoc, int blk) {
			if (!finalized)
				L1.setAttributes(size, assoc, blk);
		}
		void setL2Attributes(int size, int assoc, int blk) {
			if (!finalized)
				L2.setAttributes(size, assoc, blk);
		}
		void setVictimAttributes(int size, int assoc, int blk) {
			if (!finalized)
				Victim.setAttributes(size, assoc, blk); 
		}
		void setEvictionPolicy(int epol) {
			evictionPolicy = epol;
		}
		void printcacheconfig() {
		  printf("Cache Config:\n");
		  printf("  L1 size=%d KB, assoc=%d, LS=%d bytes, lat=%d cyc\n",
			 L1.getSize()/1024, L1.getAssociativity(),
			 L1.getBlockSize(), L1.getHitLatency());
		  printf("  Victim size=%d bytes, assoc=%d, LS=%d bytes, lat=%d cyc\n",
			 Victim.getSize(), Victim.getAssociativity(),
			 Victim.getBlockSize(), Victim.getHitLatency());
		  printf("  L2 size=%d KB, assoc=%d, LS=%d bytes, lat=%d cyc\n",
			 L2.getSize()/1024, L2.getAssociativity(),
			 L2.getBlockSize(), L2.getHitLatency());

		}
		// To be implemented
		int accessCache(int ldst, int addr, int cycles);
		double getL1MissRate();
		double getVictimLocalMissRate();
		double getL2LocalMissRate();
		double getL2GlobalMissRate();
};
