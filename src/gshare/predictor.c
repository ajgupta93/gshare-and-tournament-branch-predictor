//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Ajitesh Gupta";
const char *studentID   = "A53220177";
const char *email       = "ajgupta@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint32_t *gs_pht;
uint32_t ghist;
uint32_t lhist;
uint32_t mask;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint32_t
make_mask(uint32_t size)
{
  uint32_t mmask = 0;
  for(int i=0;i<size;i++)
  {
    mmask = mmask | 1<<i;
  }
  return mmask;
}
// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  int tablesize = 1<<ghistoryBits;
  ghist = 0;
  lhist = 0;
  mask = make_mask(ghistoryBits);
  //printf("Mask: %u\n", mask);

  switch(bpType) {
    case GSHARE:
      gs_pht = (uint32_t*) malloc(sizeof(uint32_t)*tablesize);
      for(int i=0;i<tablesize;i++)
      {
        gs_pht[i] = 1;
      }
      break;
  }      
  
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //
  uint32_t pcbits;
  uint32_t histbits;
  uint32_t index;
  uint32_t prediction;

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      //printf("Making prediction\n");
      pcbits = pc & mask;
      histbits = ghist & mask;
      index = histbits ^ pcbits;
      //printf("Index: %u\n", index);
      prediction = gs_pht[index];
      if(prediction>1)
        return TAKEN;
      else
        return NOTTAKEN;
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  uint32_t pcbits;
  uint32_t histbits;
  uint32_t index;
  uint8_t prediction = make_prediction(pc);
  
  switch(bpType) {
    case GSHARE:
      //printf("Training predictor\n");
      pcbits = pc & mask;
      histbits = ghist & mask;
      index = histbits ^ pcbits;
      //printf("XOR: %u\n", index);
      //printf("mask: %u\n", mask);
      if(outcome==TAKEN)
      {
        ghist = ghist<<1 | 1;
        ghist = ghist & mask;
        if(gs_pht[index]<3)
          gs_pht[index]++;
      }
      else
      {
        ghist = ghist<<1;
        ghist = ghist & mask;
        if(gs_pht[index]>0)
          gs_pht[index]--;
      }
      //printf("Global history: %u\n",ghist);
  }
}
