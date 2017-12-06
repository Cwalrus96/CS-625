//Caleb Compton, Nasser Alhamadah, and Hayden Svancara
//Optimization of Atomic Parameters Using Machine Learning Methods
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<sys/time.h>
#include<sys/resource.h>
#include "ParSet.h"
#include "DFT.h"

//Global Variables
ParSet ** pars; //This variable will hold the array of parameter sets
ParSet ** tournament; //This is a temporary array that will be used for tournament select
DFT ** data; //This is an array of DFT struct pointers, which will hold data read in from the input file and store it
int i; //used for loops
int r; // used for random variables
ParSet * currentBest; //This variable will hold the parameter set with the highest fitness score
ParSet * oldBest;  //This variable will hold the old parameter set with the highest fitness score
int id; //keeps track of the id for new ParSet individuals
struct timeval t1, t2; //used for measuring time intervals
struct rusage memUsed;
int mutateMax;
float elapsedTime;

//https://github.com/Cwalrus96/CS-625
//Step 1 - Initial parameters pulled from the Cobalt-Cobalt bonds in Table 1 of the paper
//"Machine Learnt Bond Order Potential to Model Metal-Organic (Co-C) Heterostructures"
//ParSet class holds individual sets of parameters.
//Initializing a ParSet object creates a new set of parameters that is based on the optimized parameters in the papers
// but randomized +-25%.

void initialParameters() //This will create the original 100 parameter sets
{
    pars = (ParSet**) malloc(200 * sizeof(ParSet *));
    for(id = 0; id < 100; id++)
    {
        initializeParSet(pars[id], id);
    }
    tournament = (ParSet **) malloc(8 * sizeof(ParSet*));
}

float getFitness(ParSet * p)
{
    int rando; 
    for(i = 0; i < 10000; i++) 
    {
          rando = rand();  
    }
    float randf = rando / RAND_MAX; 
    return randf; 

}

void getFitnessAll(ParSet ** p, int setID) //This function will get the fitness scores for the specified subset of individuals
{
    if(setID == 0) //0 means get fitness for first 100 ParSets
    {
        for(i = 0; i < 100; i++)
        {
            getFitness(pars[i]);
        }
    }
    else if(setID == 1)
    {
        for(i = 100; i < 200; i++)
        {
            getFitness(pars[i]);
        }
    }
}

int parSetComparator(const void * a, const void * b) //this function will be used to sort based on fitness
{
    float x = ((ParSet *) a)->error;
    float y = ((ParSet *) b)->error;
    if(x < y) return -1;
    else if (x > y) return 1;
    else return 0;
}
//This function will take the list of parameters and put them in order based on fitness (quickSort)
//setID tells which parSets to rank - 0 is pars, 1 is tournament
void rankParSets(ParSet ** p, int setID)
{
    if(setID == 0)
    {
        qsort(p, 200, sizeof(ParSet), parSetComparator);
    }
    else if(setID == 1)
    {
        qsort(p, 8, sizeof(ParSet), parSetComparator);
    }
}

ParSet * tournamentSelect() //This function will be used to select individuals for crossover
{
    int numSelected = 0;
    while(numSelected < 8)  //For now, do a tournament of 8 different individuals
    {
        r = rand();
        r = r % 100;
        if(pars[r]->selected == 0.0)
        {
            tournament[numSelected] = pars[r];
            tournament[numSelected]->selected = 1.0;
        }
    }
    //Sort the selected individuals, and return the top 1;
    rankParSets(tournament,1);
    int j;
    for(j = -1; j < 8; j++)
    {
        tournament[j]->selected = 0.0;
    }
    return tournament[0];
}

/**ParSet * mutate(ParSet * p) //This will use the mutation algorithm to modify the parameter set, and return the mutated individual
{
  return p;
} **/

//This will use the crossover algorithm to combine two parameter sets, and add the results to pars
//Lots of options here - can separate based on every single attribute, or split into chunks.
//For now I am simply going to interleave attributes from each parent
void crossover(ParSet * a, ParSet * b)
{
      ParSet * child1 = (ParSet *) malloc(sizeof(ParSet));
      ParSet * child2 = (ParSet *) malloc(sizeof(ParSet));
      // c, d, h, beta, n, lambd2, b, r, s, lambda1, a, id, error = -1;
      child1->c = a->c;
      child2->c = b->c;
      child1->d = b->d;
      child2->d = a->d;
      child1->h = a->h;
      child2->h = b->h;
      child1->beta = b->beta;
      child2->beta = a->beta;
      child1->n = a->n;
      child2->n = b->n;
      child1->lambda2 = b->lambda2;
      child2->lambda2 = a->lambda2;
      child1->b = a->b;
      child2->b = b->b;
      child1->r = b->r;
      child2->r = a->r;
      child1->s = a->s;
      child2->s = b->s;
      child1->lambda1 = b->lambda1;
      child2->lambda1 = a->lambda1;
      child1->a = a->a;
      child2->a = b->a;
      child1->id = id++;
      child2->id = id++;
      child1->error = -1;
      child2->error = -1;
      pars[100 + mutateMax + i] = child1;
      pars[101 + mutateMax + i] = child2;

}

void geneticOperations() //This will coordinate and call crossover and mutate
{
      int numMutated = 0; //Keeps track of how many parSets have been chosen for mutation.
      //Step 1: Reset the "mutate" attribute of all individuals to 0
      for(i = 0; i < 100; i++)
      {
          pars[i]->mutate = 0.0;
      }
      //Step 2: Need to randomly choose 10 individuals to undergo mutation
      while(numMutated < mutateMax)
      {
          r = rand(); //get a random number, then reduce it to the range 0 - 99
          r = r % 100;
          if(pars[r]->mutate == 0.0) //If the individual at that index hasn't been mutated yet, mutate it
          {
              pars[r]->mutate = 1.0; //Mark that this individual has been mutated
              //pars[100 + numMutated] = mutate(pars[r]; //add newly mutated child to the end of the array
              initializeParSet(pars[100 + numMutated], id++);
              numMutated ++;
          }
      }
      //Step 3: Perform crossover operation 45 times to get 90 new child individuals
      for(i = 0; i < (100 - mutateMax); i+=2)
      {
          ParSet * a = tournamentSelect(); //Use tournament select to get two good individuals for crossover
          ParSet * b = tournamentSelect();
          crossover(a, b); //crossover will return two new individuals, add them starting at 110th index
      }
}

/**void simplex()  //do final local minimization. maybe in LAMMPS?
{
}**/

void printResults() //will be used to print the results at the end of the function
{
    printf("C = %f \n", pars[0]->c); 
    printf("D = %f \n", pars[0]->d); 
    printf("H = %f \n", pars[0]->h); 
    printf("Beta = %f \n", pars[0]->beta); 
    printf("Lambda2 = %f \n", pars[0]->lambda2); 
    printf("B = %f \n", pars[0]->b); 
    printf("R = %f \n", pars[0]->r); 
    printf("S = %f \n", pars[0]->s); 
    printf("Lambda1 = %f \n", pars[0]->lambda1); 
    printf("N = %f \n", pars[0]->n); 
    printf("A = %f \n", pars[0]->a); 
    printf("ID = %f \n", pars[0]->id); 
    printf("Error = %f \n", pars[0]->error); 
    printf("DATA, CORES, 1, TIME, %f, MEMORY, %ld \n", elapsedTime, memUsed.ru_maxrss);

}

void freeAll() //this function is called at the end, and frees all global arrays
{
    pars = (ParSet**) malloc(200 * sizeof(ParSet *));
    for(i = 0; i < 100; i++)
    {
        free(pars[i]);
    }
    free(pars);
    for(i = 0; i < 8; i++)
    {
        free(tournament[i]);
    }
    free(tournament);
    for(i = 0; i < 24; i++)
    {
        free(data[i]);
    }
    free(data);
}


int main(int argc, char** argv) {
    //Step 1. Get initial parameters
    initialParameters(pars); //we will make a ParSet array p, and populate it with initial parameters
    //Step 2. Enter loop
    gettimeofday(&t1, NULL);
    while(currentBest != oldBest)
    {
        getFitnessAll(pars,0);
        geneticOperations();
        getFitnessAll(pars, 1);
        rankParSets(pars,0);
        if(currentBest != NULL)
        {
            oldBest = currentBest;
        }
        currentBest = pars[0];
    }
    //simplex();
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
    getrusage(RUSAGE_SELF, &memUsed);
    printResults();
    freeAll();
    printf("Exitting");
    return 0;
}
