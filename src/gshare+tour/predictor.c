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

uint32_t ghist;
uint32_t gmask;
uint32_t lmask;
uint32_t pcmask;

//gshare
uint32_t *gs_pht;

//tournament
uint32_t *local_bht;
uint32_t *local_pht;
uint32_t *global_pht;
uint32_t *choice_pht;

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
  int size;
  ghist = 0;
  gmask = make_mask(ghistoryBits);
  lmask = make_mask(lhistoryBits);
  pcmask = make_mask(pcIndexBits);

  switch(bpType) {
    case GSHARE:
      size = 1<<ghistoryBits;
      gs_pht = (uint32_t*) malloc(sizeof(uint32_t)*size);
      for(int i=0;i<size;i++)
      {
        gs_pht[i] = 1;
      }
    case TOURNAMENT:
      // Local BHT
      size = 1<<pcIndexBits;
      local_bht = (uint32_t*) malloc(sizeof(uint32_t)*size);
      for(int i=0;i<size;i++)
      {
        local_bht[i] = 0;
      }
      // Local PHT
      size = 1<<lhistoryBits;
      local_pht = (uint32_t*) malloc(sizeof(uint32_t)*size);
      for(int i=0;i<size;i++)
      {
        local_pht[i] = 1;
      }
      // Global PHT
      size = 1<<ghistoryBits;
      global_pht = (uint32_t*) malloc(sizeof(uint32_t)*size);
      for(int i=0;i<size;i++)
      {
        global_pht[i] = 1;
      }

      // Choice PHT
      size = 1<<ghistoryBits;
      choice_pht = (uint32_t*) malloc(sizeof(uint32_t)*size);
      for(int i=0;i<size;i++)
      {
        choice_pht[i] = 2;
      }
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

  uint32_t prediction;

  // gshare
  uint32_t pcbits;
  uint32_t histbits;
  uint32_t index;
  

  //tournament
  uint32_t choice;
  uint32_t pcidx;
  uint32_t lhist;
  uint32_t ghistbits;

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;

    case GSHARE:
      pcbits = pc & gmask;
      histbits = ghist & gmask;
      index = histbits ^ pcbits;
      prediction = gs_pht[index];
      if(prediction>1)
        return TAKEN;
      else
        return NOTTAKEN;

    case TOURNAMENT:
      ghistbits = gmask & ghist;
      choice = choice_pht[ghistbits];
      if(choice<2)
      {
        pcidx = pcmask & pc;
        lhist = lmask & local_bht[pcidx];
        prediction = local_pht[lhist];
      }
      else
      {
        prediction = global_pht[ghistbits];
      }
      if(prediction>1)
        return TAKEN;
      else
        return NOTTAKEN;

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

  // gshare
  uint32_t pcbits;
  uint32_t histbits;
  uint32_t index;
  uint8_t prediction = make_prediction(pc);

  // tournament
  uint32_t ghistbits;
  uint32_t choice;
  uint32_t pcidx;
  uint32_t lhist;
  uint32_t lpred;
  uint32_t gpred;
  
  switch(bpType) {
    
    case GSHARE:
      pcbits = pc & gmask;
      histbits = ghist & gmask;
      index = histbits ^ pcbits;
      if(outcome==TAKEN)
      {
        if(gs_pht[index]<3)
          gs_pht[index]++;
      }
      else
      {
        if(gs_pht[index]>0)
          gs_pht[index]--;
      }
      ghist = ghist<<1 | outcome;
      return;
    
    case TOURNAMENT:
      //printf("Global Mask : %u\nLocal Mask : %u\nPC Mask : %u\n",gmask,lmask,pcmask);
      ghistbits = gmask & ghist;
      choice = choice_pht[ghistbits];
      pcidx = pcmask & pc;
      lhist = lmask & local_bht[pcidx];
      lpred = local_pht[lhist];
      if(lpred>1)
        lpred = TAKEN;
      else
        lpred = NOTTAKEN;
      gpred = global_pht[ghistbits];
      if(gpred>1)
        gpred = TAKEN;
      else
        gpred = NOTTAKEN;

      if(gpred==outcome && lpred!=outcome && choice_pht[ghistbits]!=3)
        choice_pht[ghistbits]++;
      else if(gpred!=outcome && lpred==outcome && choice_pht[ghistbits]!=0)
        choice_pht[ghistbits]--;
      if(outcome==TAKEN)
      {
        if(global_pht[ghistbits]!=3)
          global_pht[ghistbits]++;
        if(local_pht[lhist]!=3)
          local_pht[lhist]++;
      }
      else
      {
        if(global_pht[ghistbits]!=0)
          global_pht[ghistbits]--;
        if(local_pht[lhist]!=0)
          local_pht[lhist]--;
      }
      local_bht[pcidx] = ((local_bht[pcidx]<<1) | outcome) & lmask;
      ghist = ((ghist<<1) | outcome) & gmask;
      return;
      default:
      break;
  }
}
