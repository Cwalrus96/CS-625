//Caleb Compton, Nasser Alhamadah, and Hayden Svancara
//Optimization of Atomic Parameters Using Machine Learning Methods
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<sys/time.h>
#include<sys/resource.h>
#include "ParSet.h"
#include "DFT.h"
#include<mpi.h>

//Global Variables
ParSet ** pars; //This variable will hold the array of parameter sets
ParSet ** tournament; //This is a temporary array that will be used for tournament select
DFT ** data; //This is an array of DFT struct pointers, which will hold data read in from the input file and store it
int i,j; //used for loops
int r; // used for random variables
ParSet * currentBest; //This variable will hold the parameter set with the highest fitness score
ParSet * oldBest;  //This variable will hold the old parameter set with the highest fitness score
int id; //keeps track of the id for new ParSet individuals
struct timeval t1, t2; //used for measuring time intervals
struct rusage memUsed; //Keeps track of memory usage
int mutateMax = 10; //Can be used to adjust the rate of mutation
int numCores, rank;
int exitLoop = 0; //Used to control while loops
int source, tag, jobNumber;
ParSet * job;
MPI_Status stat;
float elapsedTime;

//https://github.com/Cwalrus96/CS-625
//Step 1 - Initial parameters pulled from the Cobalt-Cobalt bonds in Table 1 of the paper
//"Machine Learnt Bond Order Potential to Model Metal-Organic (Co-C) Heterostructures"
//ParSet class holds individual sets of parameters.
//Initializing a ParSet object creates a new set of parameters that is based on the optimized parameters in the papers
// but randomized +-25%.

void initialParameters() //This will create the original 100 parameter sets
{
    //printf("About to allocate memory for 200 ParSet Pointers \n");
    pars = (ParSet**) malloc(200 * sizeof(ParSet *));
    //printf("About to initialize each individual ParSet pointer \n");
    for(i = 0; i < 200; i++)
    {
        pars[i] = (ParSet *) malloc(sizeof(ParSet));
    }
    //printf("About to populate first 100 parSets \n");
    for(id = 0; id < 100; id++)
    {
        initializeParSet(pars[id], id);
    }
    //printf("About to initialize the tournament array \n");
    tournament = (ParSet **) malloc(8 * sizeof(ParSet*));
    //printf("About to initialize each individual in the tournament array \n");
    for(i = 0; i < 8; i++)
    {
        tournament[i] = (ParSet *) malloc(sizeof(ParSet));
    }
}


float  getFitness(ParSet * p)
{
    float randf;
    //printf("Wasting time... \n");
    for(j = 0; j < 1000000; j++)
    {
    //  for(j = 0; j < 100000; j++)
    //  {
            randf = rand();
   ///  }
    }
    //printf("Done wasting time \n");
    randf = randf / RAND_MAX;
    //printf("Randf = %f \n", randf);
    return randf;
}

int parSetComparator(const void * a, const void * b) //this function will be used to sort based on fitness
{
    ParSet ** par1 = (ParSet **) a;
    ParSet ** par2 = (ParSet **) b;
    ParSet * par3 = *par1;
    ParSet * par4 = *par2;
    float x = par3->error;
    //printf("X: %f \n", x);
    float y = par4->error;
    //printf("Y: %f \n", y);
    if(x < y) return -1;
    else if (x > y) return 1;
    else return 0;
}

//This function will take the list of parameters and put them in order based on fitness (quickSort)
//setID tells which parSets to rank - 0 is pars, 1 is tournament
void rankParSets(ParSet ** p, int setID)
{
    //printf("About to rank the ParSets \n");
    if(setID == 0)
    {
        qsort(p, 200 , sizeof(ParSet *), parSetComparator);
	for(j = 0; j < 200; j++) 
	{
	//	printf("Rank: %f, Error:%f \n", pars[j]->id, pars[j]->error);
	}
    }
    else if(setID == 1)
    {
        //printf("SetID of 1 means we will be ranking the Tournament array \n");
        qsort(p, 8, sizeof(ParSet *), parSetComparator);
        //printf("Done sorting array \n");
    }
}


ParSet * tournamentSelect() //This function will be used to select individuals for crossover
{
    int numSelected = 0;
    //printf("Selecting a tournament of 8 individuals \n");
    while(numSelected < 8)  //For now, do a tournament of 8 different individuals
    {
        r = rand();
        r = r % 100;
        //printf("choosing a random individual who hasn't yet been selected: Number %d \n", r);
        if(pars[r]->selected == 0.0)
        {
            tournament[numSelected] = pars[r];
            tournament[numSelected]->selected = 1.0;
            numSelected++;
        }
        //printf("numSelected: %d \n", numSelected);
    }
    //printf("Exitting while loop \n");
    //Sort the selected individuals, and return the top 1;
    //printf("Now about to rank the individuals \n");
    rankParSets(tournament,1);
    int j;
    for(j = 0; j < 8; j++)
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
      //for(i = 0; i < 100; i++)
      //{
      //    pars[i]->mutate = 0.0;
      //}
      //Step 2: Need to randomly choose 10 individuals to undergo mutation
     // printf("Starting mutation. Number to undergo mutation is %d \n", mutateMax);
      while(numMutated < mutateMax)
      {
        //  r = rand(); //get a random number, then reduce it to the range 0 - 99
        //  r = r % 100;
        //  if(pars[r]->mutate == 0.0) //If the individual at that index hasn't been mutated yet, mutate it
        //  {
        //      pars[r]->mutate = 1.0; //Mark that this individual has been mutated
              //pars[100 + numMutated] = mutate(pars[r]; //add newly mutated child to the end of the array
              initializeParSet(pars[100 + numMutated], id++);
              numMutated ++;
        //  }
      }
      //printf("Now moving on to crossover operation \n");
      //Step 3: Perform crossover operation 45 times to get 90 new child individuals
      for(i = 0; i < (100 - mutateMax); i+=2)
      {
          //printf("Selecting the first individual for crossover \n");
          ParSet * a = tournamentSelect(); //Use tournament select to get two good individuals for crossover
          //printf("Selecting the second individual for crossover \n");
          ParSet * b = tournamentSelect();
          //printf("Performing the crossover operation \n");
          crossover(a, b); //crossover will return two new individuals, add them starting at 110th index
      }
}

/**void simplex()  //do final local minimization. maybe in LAMMPS?
{
}**/

void distributeJobs() //This function will be called once per generation by Rank 0, distributing jobs to all other processes
{
    int jobRequest;
    jobNumber = 0;
    int numRecieved = 0;
    //printf("Going to begin sending first 100 jobs \n"); 
    while(((pars[jobNumber]->error > 0) && (jobNumber < 100)) && (numRecieved < 100)) //Dont send out individuals if fitness has already been calculated
    {
        jobNumber++;
	numRecieved++; 
    }
    while((jobNumber < 100) || (numRecieved < 100)) //Send out jobs until end is reached
    {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
        source = stat.MPI_SOURCE;
        tag = stat.MPI_TAG;
        if(tag == 200) //tag of 200 is a job request - send out next job
        {
            MPI_Recv(&jobRequest, 1, MPI_INT, source, 200, MPI_COMM_WORLD, &stat); //Recieve a job request
            //ParSet struct is a contiguous array of floats. Use address of first element as pointer
            MPI_Send(&(pars[jobNumber]->error), 15, MPI_FLOAT, source, jobNumber, MPI_COMM_WORLD);
            jobNumber ++;
            while((pars[jobNumber]->error > 0) && (jobNumber < 100) && (numRecieved < 100) ) //Dont send out individuals if fitness has already been calculated
            {
                jobNumber++;
		numRecieved++; 
            }
        }
        else //Other tags mean they are sending completed job
        {
            //update ParSet array with newly completed job
            MPI_Recv(&(pars[tag]->error), 15, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &stat);
	    numRecieved++; 
        }
    }
    //printf("About to perform genetic operations \n"); 
    geneticOperations();
    //printf("About to send out the next 100 jobs \n"); 
    while((jobNumber < 200) || (numRecieved < 200)) //Send out jobs until end is reached
    {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
        source = stat.MPI_SOURCE;
        tag = stat.MPI_TAG;
        if(tag == 200) //tag of 200 is a job request - send out next job
        {
	    if(jobNumber >= 200) 
	    {
		MPI_Recv(&jobRequest, 1, MPI_INT, source, 200, MPI_COMM_WORLD, &stat); 
		MPI_Send(&(pars[0]->error), 15, MPI_FLOAT, source, 201, MPI_COMM_WORLD); 
		//Tag of 201 means "do nothing"
	    }
	    else 
	    {
            	MPI_Recv(&jobRequest, 1, MPI_INT, source, 200, MPI_COMM_WORLD, &stat); //Recieve a job request
            	//ParSet struct is a contiguous array of floats. Use address of first element as pointer
            	MPI_Send(&(pars[jobNumber]->error), 15, MPI_FLOAT, source, jobNumber, MPI_COMM_WORLD);
            	jobNumber ++;
	    	//printf("Just sent out job number %d to rank %d\n", jobNumber -1, source);
	    }
        }
        else //Other tags mean they are sending completed job
        {
            //update ParSet array with newly completed job
            MPI_Recv(&(pars[tag]->error), 15, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &stat);
	    numRecieved++; 
	    source = stat.MPI_SOURCE;
	    //printf("Just recieved job number %d from rank %d \n", numRecieved - 1, source); 
        }
    }
    //printf("Ranking the parSets \n"); 
    rankParSets(pars,0);
    //printf("Checking for convergence \n"); 
    //if(currentBest != NULL) //Update oldBest and currentBest
    //{
        oldBest = currentBest;
    //}
    currentBest = pars[0];
    printf("OldBest = %f, %f, newBest = %f, %f \n", oldBest->error, oldBest->id, currentBest->error, currentBest->id); 
    if(oldBest == currentBest) //If they still match, this is the exit condition. Program is done
    {
         exitLoop = 1;
	 printf("Convergence has occurred - notify worker processes \n"); 
         for(i = 1; i < numCores; i++)
         {
       	    MPI_Recv(&jobRequest, 1, MPI_INT, MPI_ANY_SOURCE,200, MPI_COMM_WORLD, &stat); 
	    source = stat.MPI_SOURCE; 
	    MPI_Send(&(pars[0]->error), 15, MPI_FLOAT, source, 200, MPI_COMM_WORLD);
        }
    }
}

void requestJobs() //This function will be called by all worker processes until they recieve a signal to move on

{
    //Send MPI message asking for a job
    MPI_Send(&rank, 1, MPI_INT, 0, 200, MPI_COMM_WORLD); //Send message to root asking for job (tag 200 is job request);
    MPI_Recv(&(job->error), 15, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); //Recieve job into the Job ParSet;
    jobNumber = stat.MPI_TAG;
    if(jobNumber < 200)
    {
        job->error = getFitness(job);
        MPI_Send(&(job->error), 15, MPI_FLOAT, 0, jobNumber, MPI_COMM_WORLD); //Send completed job back to root;
    }
    else if(jobNumber == 200) 
    {
        exitLoop = 1;
    }
    else 
    {
	//Do nothing 
    }
}

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
    printf("DATA, CORES, %d , TIME, %f, MEMORY, %ld \n",numCores, elapsedTime, memUsed.ru_maxrss);

}

void freeAll() //this function is called at the end, and frees all global arrays
{
    for(i = 0; i < 200; i++)
    {
        free(pars[i]);
    }
    free(pars);
    for(i = 0; i < 8; i++)
    {
        free(tournament[i]);
    }
    free(tournament);
}



int main(int argc, char** argv) {
    //Step 1. Initialize MPI
    printf("Initialize MPI \n"); 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numCores);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //Step 2. Get initial parameters
    if(rank == 0)
    {
    	//printf("Getting initial parameters \n"); 
        initialParameters(pars); //we will make a ParSet array p, and populate it with initial parameters
        gettimeofday(&t1, NULL);
    }
    //Step 3 - while loop
        //a. Rank 0 will distribute fitness jobs to the other processes until exit condition is met
    if(rank == 0)
    {
        currentBest = pars[1]; 
	oldBest = pars[0]; 
        while(exitLoop != 1)
        { 
	    //printf("Beginning to distribute jobs \n"); 
            distributeJobs();
        }
	printf("Done distributing jobs \n"); 
        //simplex();
        gettimeofday(&t2, NULL);
        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
        getrusage(RUSAGE_SELF, &memUsed);
	//printf("Printing results \n"); 
        printResults();
	printf("Freeing arrays \n"); 
        freeAll();
	printf("MPI_Finalize() \n"); 
        //printf("Exitting");
    }
    else
    {
        job = (ParSet *) malloc(sizeof(ParSet));
	//printf("Rank %d beginning to request jobs \n",rank); 
        while(exitLoop != 1)
        { 
            requestJobs();
        }
        free(job);
	printf("MPI_Finalize - rank %d \n", rank); 
    }
    MPI_Finalize(); 
    return 0;
}
