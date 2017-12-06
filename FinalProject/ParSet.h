#ifndef ParSet_H
#define ParSet_H

#include <stdlib.h>


typedef Struct parSet { ///This structure will be used to hold individual parameter sets
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
  float mutate = 0.0;  //keeps track if this individual is selected for mutation
  float selected = 0.0; //keeps track if this individual is selected for tournament select 
}ParSet; 


initializeParSet(*ParSet p, int id); //This function will take a ParSet pointer and initialize that structure with randomized values

randomizeParameter(float p); //This function will return a parameter that has randomly been adjusted in the range of +/-25%

