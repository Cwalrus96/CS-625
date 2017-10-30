//Caleb Compton, Nasser Alhamadah, and Hayden Svancara
//Optimization of Atomic Parameters Using Machine Learning Methods 
#include <stdlib.h> 
#include <stdio.h> 
#include <math.h>
#include "ParSet.h" 
https://github.com/Cwalrus96/CS-625
//Step 1 - Initial parameters pulled from the Cobalt-Cobalt bonds in Table 1 of the paper 
//"Machine Learnt Bond Order Potential to Model Metal-Organic (Co-C) Heterostructures"
//ParSet class holds individual sets of parameters. 
//Initializing a ParSet object creates a new set of parameters that is based on the optimized parameters in the papers
// but randomized +-25%. 

void initialParameters(*(ParSet)) //This will create the original 100 parameter sets 
{
  
}

void DFTData() //This function will read in the DFT data from a file and save it somewhere 
{
  
}

void getFitness(*ParSet, int setID) //In this function we will feed the necessary information into LAMMPS and get results back (Parallel)
{
  
}

void rankParSets(*(ParSet)) //This function will take the list of parameters and put them in order based on fitness 
{
  
}

void geneticOperations() //This will coordinate and call crossover and mutate 
{
   
}

void mutate() //This will use the mutation algorithm to modify the parameter set
{
  
}

void crossover() //This will use the crossover algorithm to combine two parameter sets 
{
  
}

void convergenceTest(*(ParSet)) //this function will cut population down from 200-100, compare previous fittest individual to current
{
   
}

void simplex()  //do final local minimization. maybe in LAMMPS? 
{
  
}




int main(int argc, char** argv) {
//Step 1. Get initial parameters 
  initialParameters(p); //we will make a ParSet array p, and populate it with initial parameters 
  DFTData(); 
//Step 2. Enter loop 
  while(currentBest != oldBest) 
  {
    getFitness(p,0); 
    rankParSets(); //might not need this 
    geneticOperations(); 
    getFitness(p, 1)
    rankParSets(); 
    convergenceTest(); 
  }
  simplex(); 
  return currentBest;
  
  
}
