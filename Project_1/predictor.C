#include "predictor.h"

using namespace std;

int8_t *tables[NUMBER_TABLES]; 
uint64_t  brh;
int length[8]={0,4,8,32,64,34,55,89};


void init_predictor ()
{

  for (int i = 0; i < NUMBER_TABLES; i ++) {
        tables[i] = new int8_t[TABLE_SIZE];
	for (int j=0;j<TABLE_SIZE;j++) {
	  tables[i][j] = 0;
	}
  }
	brh=0;
}

uint64_t IndexBHT(uint64_t from, int nBits) {
  int n_ones = 3;
  for (int i=2; i<nBits;i++) 
    n_ones += (1 << i);
  
  return (from & n_ones);
  
}




bool make_prediction (unsigned int pc)
{
	int32_t pred = tables[0][pc & (TABLE_SIZE-1)];
	  for (int i=1;i<NUMBER_TABLES;i++)
	    pred += tables[i][(IndexBHT(brh,length[i])^pc) & (TABLE_SIZE-1)];
		  
	pred += NUMBER_TABLES/2;
	if (pred>0) return true;
	if (pred<=0) return false;

}

void train_predictor (unsigned int pc, bool outcome)
{

	 //update predictor
	 
	 
	 int32_t pred = tables[0][pc & (TABLE_SIZE-1)];
	  
	  for (int i=1;i<NUMBER_TABLES;i++)
	    pred += tables[i][(IndexBHT(brh,length[i])^pc) & (TABLE_SIZE-1)];
	    
	  pred += NUMBER_TABLES/2;
	  int32_t pred_abs = (pred >= 0 ? pred : ((pred ^ -1) + 1));
	  
	if ( ((pred > 0) != outcome) ) { //|| (pred_abs <= THRESHOLD)
	    if (outcome== true) {
			if((tables[0][pc & (TABLE_SIZE-1)])<(MAXIMUM_BOUNDARY-1))
	      tables[0][pc & (TABLE_SIZE-1)]++;
		}
	    if(outcome==false){
			if((tables[0][pc & (TABLE_SIZE-1)])> (-MAXIMUM_BOUNDARY))
	      tables[0][pc & (TABLE_SIZE-1)]--;
		}

	    for (int i=1;i<NUMBER_TABLES;i++){
		if (outcome== true) {
			if(tables[i][(IndexBHT(brh,length[i])^pc) & (TABLE_SIZE-1)]<(MAXIMUM_BOUNDARY-1))
			  tables[i][(IndexBHT(brh,length[i])^pc) & (TABLE_SIZE-1)]++;
			}
	     if(outcome==false){
			if(tables[i][(IndexBHT(brh,length[i])^pc) & (TABLE_SIZE-1)]>(-MAXIMUM_BOUNDARY))
               tables[i][(IndexBHT(brh,length[i])^pc) & (TABLE_SIZE-1)]--;
	          }
	    }	    
	  }

	//update brh
	 brh = (brh << 1) | (outcome ? 1 : 0);
	  
}
