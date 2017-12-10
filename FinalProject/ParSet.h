#ifndef ParSet_H
#define ParSet_H

#include <stdlib.h>


typedef struct parSet { ///This structure will be used to hold individual parameter sets
  float error;
  float id;
  float c;
  float d;
  float h;
  float beta;
  float n;
  float lambda2;
  float b;
  float r;
  float s;
  float lambda1;
  float a;
  float mutate;  //keeps track if this individual is selected for mutation
  float selected; //keeps track if this individual is selected for tournament select
}ParSet;


void initializeParSet(ParSet *  p, int id); //This function will take a ParSet pointer and initialize that structure with randomized values

float randomizeParameter(float p); //This function will return a parameter that has randomly been adjusted in the range of +/-25%
#endif
