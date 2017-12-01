//Caleb Compton, Nasser Alhamadah, and Hayden Svancara
//Optimization of Atomic Parameters Using Machine Learning Methods
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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

void getDFTData() //This function will read in the DFT data from a file and save it somewhere
{
    data = (DFT **) malloc(24 * sizeof(DFT *));
    char ** dftLines = (char **)malloc(31 * sizeof(char *));  //create some temporary arrays to store input from the file 
    char ** tokens = (char **) malloc(9 * sizeof(char *));
    int t; 
    int d; 
    for(i = 0; i < 31; i++) 
    {
      dftLines[i] = (char *) malloc(100 * sizeof(char)); 
    }
    for(t = 0; t < 9; t++) 
    {
        tokens[t] = (char *) malloc(20 * sizeof(char));    
    }
    for(d = 0; d < 24; d++) 
    {
        data[d] = (DFT *) malloc(sizeof(DFT));    
    }
    FILE * file; 
    if ((file = fopen("trainset.in", "r")) != NULL) 
    {
        i = 0; 
        d = 0; 
        while((i < 31) && (fgets(dftLines[i], 100, file) != NULL)) // Read all lines from the file and store them in the dftLines array
        {
            fputs ( line, stdout ); /* write the line */
        }
        for(i = 0; i < 31; i++) 
        {
          if(dftLines[i][0] == ' ') //Starting with a space means it is a data line. tokenize the line and store the tokens in a DFT struct 
          {
              t = 0; 
              tokens[t] = strtok (dftLines[i]," /");
              while ((tokens[t] != NULL) && (t < 9)) //after this look all 8 tokens should be in the tokens array
              {
                t++; 
                tokens[t] = strtok (NULL, " /");
              } //now we will use the tokens to fill in the attributes of a DFT struct in the data array
              data[d]->weight = atof(tokens[0]); 
              data[d]->op1 = tokens[1][0]; 
              strcpy(data[d]->key1, tokens[2]); 
              data[d]->n1 = atoi(tokens[3]); 
              data[d]->op2 = tokens[4][0]; 
              strcpy(data[d]->key2, tokens[5]); 
              data[d]->n2 = atoi(tokens[6]); 
              data[d]->energy = atof(tokens[7]);
              d++; 
          }
        }
    }
    else 
    {
        printf("Error: file not found"); 
        exit(1);
    }
    char ** dftLines = (char **)malloc(31 * sizeof(char *));  //create some temporary arrays to store input from the file 
    char ** tokens = (char **) malloc(9 * sizeof(char *));
    for(i = 0; i < 31; i++) 
    {
      free(dftLines[i]); 
    }
    free(dftLines); 
    for(t = 0; t < 9; t++) 
    {
        free(tokens[t]);    
    }
    free(tokens); 
 
}

void getFitness(ParSet * p)
{

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
  pars[110 + i] = child1;
  pars[111 + i] = child2;

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
  while(numMutated < 10)
    {
      r = rand(); //get a random number, then reduce it to the range 0 - 99
      r = r % 100;
      if(pars[r]->mutate == 0.0) //If the individual at that index hasn't been mutated yet, mutate it
        {
          pars[r]->mutate = 1.0; //Mark that this individual has been mutated
          //pars[100 + numMutated] = mutate(pars[r]; //add newly mutated child to the end of the array
          initializeParSet(100 + numMutated, id++); 
          numMutated ++;
        }
    }
  //Step 3: Perform crossover operation 45 times to get 90 new child individuals
  for(i = 0; i < 90; i+=2)
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
  getDFTData();
  //Step 2. Enter loop
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
  printResults(); 
  freeAll(); 
  printf("Exitting");
  return 0;
}
