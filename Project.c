//Caleb Compton and Nasser Alhamadah 
//Optimization of Atomic Parameters Using Machine Learning Methods 
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
//Need to figure out how to include or interface with LAMMPS

//Should we using single or double precision values? Single Precision is faster, double is more accurate. 
//Using single precision for now based on the level of precision provided in original paper 

//Step 1 - Initial parameters pulled from the Cobalt-Cobalt bonds in Table 1 of the paper 
//"Machine Learnt Bond Order Potential to Model Metal-Organic (Co-C) Heterostructures"
//Do we want to create a "parameters" data structure to hold all of these parameters? 

float c = 30611; 
float d = 113.064 
