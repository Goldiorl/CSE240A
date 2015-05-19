#include "cache.h"

/* Fill this in with parameters that will give you the
   best CPI and AMAT. */
void tune_parameters(Core *c) {
	c->setL1Attributes(0, 0, 0);
	c->setVictimAttributes(0, 0, 0);
	c->setL2Attributes(0, 0, 0);
	c->setEvictionPolicy(0);
}

/* Implement this function. 
 * Returns true if there is a cache hit.
 * Arguments: ldst - 0 if load, 1 if store.
 *            address - Memory address of the reference.
 *            cycle - Number of cycles executed so far.
 */
bool Cache::isHit(int ldst, int address, int cycle) {
	unsigned int b=log2(blockSize);
	unsigned int s=log2(size/(blockSize*assoc));
    unsigned long int temp = address >> b;
    unsigned long int mask = 0xFFFFFFFF >> (32-s);
    unsigned long int set = temp & mask;
    unsigned long int tagbits = temp >> s;
	 /* when direct map cache*/
	  if (assoc == 1) {
        if (accessed[set][0]!=0 && tag[set][0] == tagbits) {
            return true;
        }

			return false;
      }
	  /* case of associative caches */
	  else if(assoc>=1) {
	  for (int i = 0; i < assoc; i++) {
              if (accessed[set][i] !=0 && tag[set][i]== tagbits) {
                  return true;
              }
          }
		  return false;
	  }
}

/* Implement this function.
 * Returns the number of cycles taken to perform the memory reference.
 * Arguments: ldst - 0 if load, 1 if store.
 *            address - Memory address of the reference.
 *            cycle - Number of cycles executed so far.
 * Function calls you might make: isHit, getters in Cache.
 * Data structures you might need to read and/or update: 
 *           tags and accessed arrays in Cache.
 */
int Core::accessCache(int ldst, int address, int cycles) {
	unsigned int b_L1=log2(L1.getBlockSize());
	unsigned int s_L1=log2(L1.getSize()/(L1.getBlockSize()*L1.getAssociativity()));
    unsigned long int temp_L1 = address >> b_L1;
    unsigned long int mask_L1 = 0xFFFFFFFF >> (32-s_L1);
    unsigned long int set_L1 = temp_L1 & mask_L1;
    unsigned long int tagbits_L1 = temp_L1 >> s_L1;
	
	unsigned int b_L2=log2(L2.getBlockSize());
	unsigned int s_L2=log2(L2.getSize()/(L2.getBlockSize()*L2.getAssociativity()));
    unsigned long int temp_L2 = address >> b_L2;
    unsigned long int mask_L2 = 0xFFFFFFFF >> (32-s_L2);
    unsigned long int set_L2 = temp_L2 & mask_L2;
    unsigned long int tagbits_L2 = temp_L2 >> s_L2;
	
	unsigned int b_Victim=log2(Victim.getBlockSize());
	unsigned int s_Victim=log2(Victim.getSize()/(Victim.getBlockSize()*Victim.getAssociativity()));
    unsigned long int temp_Victim = address >> b_Victim;
    unsigned long int mask_Victim = 0xFFFFFFFF >> (32-s_Victim);
    unsigned long int set_Victim = temp_Victim & mask_Victim;
    unsigned long int tagbits_Victim = temp_Victim >> s_Victim;

	
	if(!L1.isHit(ldst, address, cycles) && !L2.isHit( ldst,  address, cycles)&&!Victim.isHit(ldst, address,cycles)){
		totalMemRef++;
		l1Accesses++;
		l2Accesses++;
		victimAccesses++;
		l1Misses++;
		l2Misses++;
		victimMisses++;
		
		int evicttag_L1;
		int evicttag_L1_Victim;
		int haveeviction=0;
	 if (L1.getAssociativity() == 1) {
		if (L1.accessed[set_L1][0]!=0) {
			 L1.accessed[set_L1][0]=cycles;
			 evicttag_L1=L1.tag[set_L1][0];
			 haveeviction=1;
			 L1.tag[set_L1][0]=tagbits_L1;
		}else if(L1.accessed[set_L1][0]==0 ){
			 L1.accessed[set_L1][0]=cycles;
			 L1.tag[set_L1][0]=tagbits_L1;
		}

	}
	  /* case of associative caches */
	  else if(L1.getAssociativity() >=1) {
	      int flag = 1; /* to set when eviction needed */
          int min = 0;  /* used to find the LRU line */
	  
	  for (int i = 0; i < L1.getAssociativity(); i++) {
              if (L1.accessed[set_L1][i]==0) {
                  L1.accessed[set_L1][i]=cycles;
				  L1.tag[set_L1][i]=tagbits_L1;
				  flag=0;
				  break;
              }
          }
		  
		  if(flag==1){
		  int i;
		  int getIndex=L1.accessed[set_L1][0];
		  for(i = 0; i < L1.getAssociativity(); i++) {
			  if (L1.accessed[set_L1][i] < getIndex) {
				   getIndex = L1.accessed[set_L1][i];
				   min = i;
			  }
		
		  }
		 evicttag_L1=L1.tag[set_L1][min];
		 haveeviction=1;
		 L1.tag[set_L1][min]=tagbits_L1;
		 L1.accessed[set_L1][min]=cycles;
		}
	  }
	 
	 
	 if (haveeviction==1){
	 evicttag_L1_Victim= ((evicttag_L1 << s_L1)+set_L1) >>s_Victim;
	 //victim踢掉或者插入一个
	 if (Victim.getAssociativity() == 1) {
		if (Victim.accessed[set_Victim][0]!=0) {
			 Victim.accessed[set_Victim][0]=cycles;
			 //evictag_L1=L1.tag[set_L1][0];
			 Victim.tag[set_Victim][0]=evicttag_L1_Victim;
		}else if(Victim.accessed[set_Victim][0]==0 ){
			 Victim.accessed[set_Victim][0]=cycles;
			 Victim.tag[set_Victim][0]=evicttag_L1_Victim;
		}

	}
	  /* case of associative caches */
	  else if(Victim.getAssociativity() >=1) {
	      int flag = 1; /* to set when eviction needed */
          int min = 0;  /* used to find the LRU line */
	  
	  for (int i = 0; i < Victim.getAssociativity(); i++) {
              if (Victim.accessed[set_Victim][i]==0) {
                  Victim.accessed[set_Victim][i]=cycles;
				  Victim.tag[set_Victim][i]=evicttag_L1_Victim;
				  flag=0;
				  break;
              }
          }
		  
		  if(flag==1){
		  int i;
		  int getIndex=Victim.accessed[set_Victim][0];
		  for(i = 0; i < Victim.getAssociativity(); i++) {
			  if (Victim.accessed[set_Victim][i] < getIndex) {
				   getIndex = Victim.accessed[set_Victim][i];
				   min = i;
			  }
		
		  }
		 //evicttag_L1=L1.tag[set_L1][i];
		 Victim.tag[set_Victim][min]=evicttag_L1_Victim;
		 Victim.accessed[set_Victim][min]=cycles;
		}
	  }
	 }
	 
	 
	 // For L2
	 
	 if (L2.getAssociativity() == 1) {
		if (L2.accessed[set_L2][0]!=0) {
			 L2.accessed[set_L2][0]=cycles;
			 //evictag_L1=L1.tag[set_L2][0];
			 L1.tag[set_L2][0]=tagbits_L2;
		}else if(L2.accessed[set_L2][0]==0 ){
			 L2.accessed[set_L2][0]=cycles;
			 L2.tag[set_L2][0]=tagbits_L2;
		}

	}
	  /* case of associative caches */
	  else if(L2.getAssociativity() >=1) {
	      int flag = 1; /* to set when eviction needed */
          int min = 0;  /* used to find the LRU line */
	  
	  for (int i = 0; i < L2.getAssociativity(); i++) {
              if (L2.accessed[set_L2][i]==0) {
                  L2.accessed[set_L2][i]=cycles;
				  L2.tag[set_L2][i]=tagbits_L2;
				  flag=0;
				  break;
              }
          }
		  
		  if(flag==1){
		  
		  int i;
		  int getIndex=L2.accessed[set_L2][0];
		  for(i = 0; i < L2.getAssociativity(); i++) {
			  if (L2.accessed[set_L1][i] < getIndex) {
				   getIndex = L2.accessed[set_L2][i];
				   min = i;
			  }
		
		  }
		// evicttag_L1=L1.tag[set_L1][i];
		 L2.tag[set_L2][min]=tagbits_L2;
		 L2.accessed[set_L2][min]=cycles;
		 
			}
	  }	  
		//=====Store L1, L2,UPdate LRU for both, any evicted to Victim;-- done
		
		return 350;

	}
	
	if(L1.isHit(ldst, address, cycles)){
	
	totalMemRef++;
	l1Accesses++;
	victimAccesses++;
	victimMisses++;
	
		 /* when direct map cache*/
	  if (L1.getAssociativity() == 1) {
        if (L1.accessed[set_L1][0]!=0 && L1.tag[set_L1][0] == tagbits_L1) {
				L1.accessed[set_L1][0]=cycles;
        }

      }
	  /* case of associative caches */
	  else if(L1.getAssociativity()>=1) {
	  for ( int i = 0; i < L1.getAssociativity(); i++) {
              if (L1.accessed[set_L1][i] !=0 && L1.tag[set_L1][i]== tagbits_L1) {
                  L1.accessed[set_L1][i]=cycles;
              }
          }
	  }
	  
	//=======update LRU  ----done
	
	return L1.getHitLatency();

	}
	
	if(Victim.isHit(ldst, address, cycles)){
	totalMemRef++;
	l1Accesses++;
	victimAccesses++;
	l1Misses++;
	
	int evicttag_L1;
	int swapflag=1;
	int min = 0;  /* used to find the LRU line */
	int location=0;
	int evicttag_L1_Victim;
	int evictag_Victim_L1;
	 if (L1.getAssociativity() == 1) {
		if (L1.accessed[set_L1][0]!=0) {
			 L1.accessed[set_L1][0]=cycles;
			 evicttag_L1=L1.tag[set_L1][0];
		}else if(L1.accessed[set_L1][0]==0 ){
			swapflag=0;
		}
	}
	
	
		  /* case of associative caches */
	  else if(L1.getAssociativity() >=1) {
	      int flag = 1; /* to set when eviction needed */
          //int min = 0;  /* used to find the LRU line */
		  //int location=0;
	  for (int i = 0; i < L1.getAssociativity(); i++) {
              if (L1.accessed[set_L1][i]==0) {
				  swapflag=0;
				  location=i;
				  break;
              }
          }
		  
		if(swapflag==1){
		  int i;
		  int getIndex=L1.accessed[set_L1][0];
		  for(i = 0; i < L1.getAssociativity(); i++) {
			  if (L1.accessed[set_L1][i] < getIndex) {
				   getIndex = L1.accessed[set_L1][i];
				   min = i;
			  }
		
		  }
		 evicttag_L1=L1.tag[set_L1][min];
		 L1.accessed[set_L1][min]=cycles;
		}
		}
	 
	 	 evicttag_L1_Victim= ((evicttag_L1 << s_L1)+set_L1) >>s_Victim;
		 evictag_Victim_L1= ((tagbits_Victim << s_Victim)+set_Victim) >>s_L1;
		 
// SWAP
		 /* when direct map cache*/
	  if (Victim.getAssociativity() == 1) {
		if(swapflag==1){
			L1.tag[set_L1][min]=evictag_Victim_L1;
			L1.accessed[set_L1][min]=cycles;
			Victim.tag[set_Victim][0]=evicttag_L1_Victim;
			Victim.accessed[set_Victim][0]=cycles;
		}else if (swapflag==0){
			L1.tag[set_L1][location]=evictag_Victim_L1;
			L1.accessed[set_L1][location]=cycles; 
			Victim.accessed[set_Victim][0]=0;
			Victim.tag[set_Victim][0]=0;
		}
      }
	  /* case of associative caches */
	  else if(Victim.getAssociativity()>=1) {
	  for (int i = 0; i < Victim.getAssociativity(); i++) {
              if (Victim.accessed[set_Victim][i] !=0 && Victim.tag[set_Victim][i]== tagbits_Victim) {
                 if(swapflag==1){
				 Victim.accessed[set_Victim][i] =cycles;
				 Victim.tag[set_Victim][i]== evicttag_L1_Victim;
				 L1.tag[set_L1][min]=evictag_Victim_L1;
				 L1.accessed[set_L1][min]=cycles;
				 }else if (swapflag==0){
				 L1.tag[set_L1][location]=evictag_Victim_L1;
				 L1.accessed[set_L1][location]=cycles; 
				 Victim.accessed[set_Victim][i]=0;
			     Victim.tag[set_Victim][i]=0;
				 }
              }
          }
	  }
	  
	  
	//=========SWAP ; update LRU  ---done
	
	return Victim.getHitLatency();

	}
	
	if(L2.isHit(ldst,  address, cycles)){
	totalMemRef++;
	l1Accesses++;
	l2Accesses++;
	victimAccesses++;
	l1Misses++;
	victimMisses++;
	
	int tag_L2_L1;
	tag_L2_L1= ((tagbits_L2 << s_L2)+set_L2) >>s_L1;
	int evicttag_L1;
	int evicttag_L1_Victim;
	
	if (L2.getAssociativity() == 1) {
        if (L2.accessed[set_L2][0]!=0 && L2.tag[set_L2][0] == tagbits_L2) {
				L2.accessed[set_L2][0]=cycles;
        }

      }
	  /* case of associative caches */
	  else if(L2.getAssociativity()>=1) {
	  for (int i = 0; i < L2.getAssociativity(); i++) {
              if (L2.accessed[set_L2][i] !=0 && L2.tag[set_L2][i]== tagbits_L2) {
                  L2.accessed[set_L2][i]=cycles;
              }
          }
	  }
	  // Store L1 , any evicted
		int haveeviction=0;
	 if (L1.getAssociativity() == 1) {
		if (L1.accessed[set_L1][0]!=0) {
			 L1.accessed[set_L1][0]=cycles;
			 evicttag_L1=L1.tag[set_L1][0];
			 haveeviction=1;
			 L1.tag[set_L1][0]=tag_L2_L1;
		}else if(L1.accessed[set_L1][0]==0 ){
			 L1.accessed[set_L1][0]=cycles;
			 L1.tag[set_L1][0]=tag_L2_L1;
		}

	}
	  /* case of associative caches */
	  else if(L1.getAssociativity() >=1) {
	      int flag = 1; /* to set when eviction needed */
          int min = 0;  /* used to find the LRU line */
	  
	  for (int i = 0; i < L1.getAssociativity(); i++) {
              if (L1.accessed[set_L1][i]==0) {
                  L1.accessed[set_L1][i]=cycles;
				  L1.tag[set_L1][i]=tag_L2_L1;
				  flag=0;
				  break;
              }
          }
		  
		  if(flag==1){
		  int i;
		  int getIndex=L1.accessed[set_L1][0];
		  for(i = 0; i < L1.getAssociativity(); i++) {
			  if (L1.accessed[set_L1][i] < getIndex) {
				   getIndex = L1.accessed[set_L1][i];
				   min = i;
			  }
		
		  }
		 evicttag_L1=L1.tag[set_L1][min];
			 haveeviction=1;
		 L1.tag[set_L1][min]=tag_L2_L1;
		 L1.accessed[set_L1][min]=cycles;
		}
	  }
	 if (haveeviction==1){
	 evicttag_L1_Victim= ((evicttag_L1 << s_L1)+set_L1) >>s_Victim;
	 
	 //victim踢掉或者插入一个
	 if (Victim.getAssociativity() == 1) {
		if (Victim.accessed[set_Victim][0]!=0) {
			 Victim.accessed[set_Victim][0]=cycles;
			 //evictag_L1=L1.tag[set_L1][0];
			 Victim.tag[set_Victim][0]=evicttag_L1_Victim;
		}else if(Victim.accessed[set_Victim][0]==0 ){
			 Victim.accessed[set_Victim][0]=cycles;
			 Victim.tag[set_Victim][0]=evicttag_L1_Victim;
		}

	}
	  /* case of associative caches */
	  else if(Victim.getAssociativity() >=1) {
	      int flag = 1; /* to set when eviction needed */
          int min = 0;  /* used to find the LRU line */
	  
	  for (int i = 0; i < Victim.getAssociativity(); i++) {
              if (Victim.accessed[set_Victim][i]==0) {
                  Victim.accessed[set_Victim][i]=cycles;
				  Victim.tag[set_Victim][i]=evicttag_L1_Victim;
				  flag=0;
				  break;
              }
          }
		  
		  if(flag==1){
		  int i;
		  int getIndex=Victim.accessed[set_Victim][0];
		  for(i = 0; i < Victim.getAssociativity(); i++) {
			  if (Victim.accessed[set_Victim][i] < getIndex) {
				   getIndex = Victim.accessed[set_Victim][i];
				   min = i;
			  }
		
		  }
		 //evicttag_L1=L1.tag[set_L1][i];
		 Victim.tag[set_Victim][min]=evicttag_L1_Victim;
		 Victim.accessed[set_Victim][min]=cycles;
		}
	  }
	}
	//==Store L1, update LRU for both, any evicted to Victim;  --done
	
	return L1L2Transfer+L2.getHitLatency();

	}
	
	
	
}

/* Implement the following functions. */
double Core::getL1MissRate() {
	return l1Misses/l1Accesses;
}
double Core::getVictimLocalMissRate() {
	return victimMisses/victimAccesses;
}
double Core::getL2LocalMissRate() {
	return l2Misses/l2Accesses;
}
double Core::getL2GlobalMissRate() {
	return l2Misses/totalMemRef;
}
