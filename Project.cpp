//Caleb Compton and Nasser Alhamadah 
//Optimization of Atomic Parameters Using Machine Learning Methods 
#include <cstdlib> 
#include <cstdio> 
#include <cmath>
#include "ParSet.h" 

using namespace std; 
//Need to figure out how to include or interface with LAMMPS

//Should we using single or double precision values? Single Precision is faster, double is more accurate. 
//Using single precision for now based on the level of precision provided in original paper 

//Step 1 - Initial parameters pulled from the Cobalt-Cobalt bonds in Table 1 of the paper 
//"Machine Learnt Bond Order Potential to Model Metal-Organic (Co-C) Heterostructures"
//ParSet class holds individual sets of parameters. 
//Initializing a ParSet object creates a new set of parameters that is based on the optimized parameters in the papers
// but randomized +-25%. 
//C++ uses vectors as dynamically resizable arrays. Yay! 

int numParams = 100; 
vector<ParSet> parameters(numParams); //create a list of 100 ParSets

int main(int argc, char** argv) {
  for(int i = 0; i < numParams; i++) //create 100 new randomized ParSets and add them to the list
  {
    ParSet p; 
    parameters[i] = p; 
  }
  
  
  
}
