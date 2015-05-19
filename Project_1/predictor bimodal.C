// A very stupid predictor.  It will always predict not taken.
#include "predictor.h"
#include <stdio.h>
#include <cassert>
#include <string.h>
#include <inttypes.h>
#include <math.h>
uint8_t table[TABLE_LENGTH];

void init_predictor ()
{
	for(i=0;i<TABLE_LENGTH;i++){
	uint8_t table[i]=0;
	}
}

bool make_prediction (unsigned int pc)
{
  return (table[pc & (TABLE_LENGTH-1)]>>1)==1? true:false;
}

void train_predictor (unsigned int pc, bool outcome)
{
	if((table[pc & (TABLE_LENGTH-1)]>>1)!=outcome){
		if(outcome=true && table[pc & (TABLE_LENGTH-1)]<3)
		table[pc & (TABLE_LENGTH-1)]++;
		if(outcome=false && table[pc & (TABLE_LENGTH-1)]>0)
		table[pc & (TABLE_LENGTH-1)]--;
	}
	
}
