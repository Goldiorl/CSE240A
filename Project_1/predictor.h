/***************************************************
		CSE 240 A Project:Branch Prediction
			   Type: 
			Author:Junchuan Wang
 ***************************************************/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>



#ifndef PREDICTOR_H
#define PREDICTOR_H



/*
  Define all your tables and their sizes here.
  All tables must be statically-sized.
  Please don't call malloc () or any of its relatives within your
  program.  The predictor will have a budget, namely (32K + 256) bits
  (not bytes).  That encompasses all storage (all tables, plus GHR, if
  necessary -- but not PC) used for the predictor.  That may not be
  the amount of storage your predictor uses, however -- for example,
  you may implement 2-bit predictors with a table of ints, in which
  case the simulator will use more memory -- that's okay, we're only
  concerned about the memory used by the simulated branch predictor.
*/
#define NUMBER_TABLES 5 
#define THRESHOLD 5
#define TABLE_SIZE 2048 //A Table has 1K entries , each entry has 4 bits. 
#define TABLE_LENGTH_POW 10// Total size= 8(tables)*1K(entries)*4(bits)=32K+64 (BHT)
#define ENTRY_SIZE 2
#define BHT_LENGTH 64
#define MAXIMUM_BOUNDARY 4

/*
  Initialize the predictor.
*/
void init_predictor ();

/*
  Make a prediction for conditional branch instruction at PC 'pc'.
  Returning true indicates a prediction of taken; returning false
  indicates a prediction of not taken.
*/
bool make_prediction (unsigned int pc);

/*
  Train the predictor the last executed branch at PC 'pc' and with
  outcome 'outcome' (true indicates that the branch was taken, false
  indicates that the branch was not taken).
*/
void train_predictor (unsigned int pc, bool outcome);

#endif
