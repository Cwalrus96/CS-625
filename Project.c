//Caleb Compton, Nasser Alhamadah, and Hayden Svancara
//Optimization of Atomic Parameters Using Machine Learning Methods 
#include <stdlib.h> 
#include <stdio.h> 
#include <math.h>
#include "ParSet.h" 

//Global Variables 
ParSet * pars; //This variable will hold the array of parameter sets
int i; 
ParSet currentBest; 
ParSet oldBest; 

https://github.com/Cwalrus96/CS-625
//Step 1 - Initial parameters pulled from the Cobalt-Cobalt bonds in Table 1 of the paper 
//"Machine Learnt Bond Order Potential to Model Metal-Organic (Co-C) Heterostructures"
//ParSet class holds individual sets of parameters. 
//Initializing a ParSet object creates a new set of parameters that is based on the optimized parameters in the papers
// but randomized +-25%. 

void initialParameters(*(ParSet)) //This will create the original 100 parameter sets 
{
    pars = (ParSet*) malloc(200 * sizeof(ParSet)); 
    for(i = 0; i < 100; i++) 
    {
        initializeParSet(&pars[i], i);    
    }
}

void DFTData() //This function will read in the DFT data from a file and save it somewhere 
{
  
}

void getFitnessAll(ParSet * p, int setID) //This function will get the fitness scores for the specified subset of individuals
{
    if(setID == 0) //0 means get fitness for first 100 ParSets
    {
        for(i = 0; i < 100; i++) 
        {
            getFitness(&pars[i]);    
        }
    }
    else if(setID == 1) 
    {
        for(i = 100; i < 200; i++) 
        {
            getFitness(&pars[i]);    
        }
    }
}

void getFitness(ParSet * p) //This function will get the fitness score for the specified individual using LAMMPS
{ 
    
}

int parSetComparator(const void * a, const void * b) //this function will be used to sort based on fitness
{
    float x = ((ParSet *) a)->error; 
    float y = ((ParSet *) b)->error; 
    if(x < y) return -1; 
    else if (x > y) return 1; 
    else return 0; 
}

void rankParSets(ParSet* p) //This function will take the list of parameters and put them in order based on fitness (quickSort) 
{
  qsort(p, 200, sizeof(ParSet), parSetComparator); 
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
    geneticOperations(); 
    getFitness(p, 1)
    rankParSets(); 
    convergenceTest(); 
  }
  simplex(); 
  return currentBest;
  
  
}
