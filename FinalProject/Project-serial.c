//Caleb Compton, Nasser Alhamadah, and Hayden Svancara
//Optimization of Atomic Parameters Using Machine Learning Methods
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<math.h>
#include<string.h>
#include "ParSet.h"
#include "DFT.h"
#include "unistd.h"
#include "library.h" //LAMMPS library

//Global Variables
ParSet ** pars; //This variable will hold the array of parameter sets
ParSet ** tournament; //This is a temporary array that will be used for tournament select
int i,j; //used for loops
int r; // used for random variables
ParSet * currentBest; //This variable will hold the parameter set with the highest fitness score
ParSet * oldBest;  //This variable will hold the old parameter set with the highest fitness score
int id; //keeps track of the id for new ParSet individuals
struct timeval t1, t2; //used for measuring time intervals
struct rusage memUsed;
int mutateMax = 10;
float elapsedTime;
void * lmps;

const float PTBCC = -275.046;
//const float PTBCC97 = -270.6076653;
//const float PTBCC95 = -259.7649905;
const float PTBCC93 = -241.4031493;
//const float PTBCC103 = -272.77316546;
//const float PTBCC105 = -266.95674333;
//const float PTBCC107 = -259.20086043;

const float PTFCC = -556.68203601;
//const float PTFCC97 = -545.98671808;
//const float PTFCC95 = -524.24895188;
const float PTFCC93 = -487.03936076;
//const float PTFCC103 = -548.97963052;
//const float PTFCC105 = -536.71478904;
//const float PTFCC107 = -520.59643877;

const float PTSC = -111.50174437;
//const float PTSC97 = -109.32768872;
//const float PTSC95 = -105.26301453;
const float PTSC93 = -98.305635193;
//const float PTSC103 = -109.8785013;
//const float PTSC105 = -107.60075391;
//const float PTSC107 = -104.6146305;

const float PTHCP = -274.15742061;
//const float PTHCP103 = -270.14737685;
//const float PTHCP105 = -264.08533688;
//const float PTHCP107 = -256.1035068;
const float PTHCP93 = -239.79702863;
//const float PTHCP97 = -259.56185016;
//const float PTHCP95 = -258.09995016;

const float weight = 5.0;
FILE * file;
int first;

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
    lammps_open_no_mpi(0, NULL, &lmps);  //Initialize LAMMPS pointer
    //for(i = 0; i < 100; i++)
    //{
//      printf("%d \n", (int) pars[i]->id);
  //  }
    first = 0;
}

//This file will be called to create a .tersoff file containing the parameters of an individual ParSet
char * writeTersoffFile(ParSet * p)
{
    //First, build file name string
    char* tersoffFile = (char *) malloc(21 * sizeof(char));  //Building a file name to hold the parameter set
    char* idString = (char *) malloc(7 * sizeof(char)); //idString holds a string representation of the ParSet id
    //printf("ID~ %d \n",(int) p->id);
    sprintf(idString, "%d",(int) p->id);
    memcpy(tersoffFile, "Pt\0", 3 * sizeof(char));
    strcat(tersoffFile, idString);
    strcat(tersoffFile, ".tersoff\0"); //tersoffFile now holds the name of a file for this individual's parameters

    //printf("TERSOFF~ %s \n", tersoffFile);

    //c, d, costheta0 (h), n, beta, lambda2, B, R, D (s), lambda1, A
    //Second, build atomic parameters string
    char * paramString = (char *) malloc(200 * sizeof(char)); //paramString will hold the atomic parameters
    memcpy(paramString, "Pt Pt Pt 3.0 1.0 0.0 \0", 22 * sizeof(char));
    //add c to paramString
    char * param = (char *) malloc(11 * sizeof(char));
    sprintf(param, "%10f", p->c);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add d
    sprintf(param, "%10f", p->d);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add  h
    sprintf(param, "%10f", p->h);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add n
    sprintf(param, "%10f", p->n);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add beta
    sprintf(param, "%10f", p->beta);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add lambda2
    sprintf(param, "%10f", p->lambda2);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add b
    sprintf(param, "%10f", p->b);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add r
    sprintf(param, "%10f", p->r);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add s
    sprintf(param, "%10f", p->s);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //add lambda1
    sprintf(param, "%10f", p->lambda1);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, " ");
    //finally, add a
    sprintf(param, "%10f", p->a);
    //printf("%s \n", param);
    strcat(paramString, param);
    strcat(paramString, "\n \0");

    //printf("%s \n", paramString);

    //Write the parameter string to the file specified in the tersoff file string, and close the file
    file = fopen(tersoffFile, "w");
    fputs(paramString, file);
    fclose(file);
    free(paramString);
    free(idString);
    free(param);
    return tersoffFile;

}

//Call this function to calculate the tersoff potential for an individual.
//geo is a string that contains the name of a file. This file specifies the atomic geometry
//paramFile is a .tersoff file that contains the parameters of a particular individual.
float getPotential(ParSet * p, char * geo, char * paramFile)
{
    //printf("%s \n", geo);
    //printf("%s \n", paramFile);
    //printf("Initializing string buffer \n");
    //char* buf = (char *) malloc(sizeof(char) * 2848);

    //printf("Initializing pipes to redirect output \n");
    //Use pipes to redirect stdout
    //int npipe[2];
    //pipe(npipe);
    //int saved_stdout = dup(STDOUT_FILENO);
    //dup2(npipe[1], STDOUT_FILENO);
    //close(npipe[1]);

    //printf("Building command string \n");
    //Build a command string that sets up the geometry and parameters of the tersoff function for LAMMPS
    char * commandString = (char *) malloc(sizeof(char) * 350);
    //if(first == 0)
    //{
        char * logFile = (char *) malloc(20 * sizeof(char));
        memcpy(logFile, "log\0", 4 * sizeof(char));
        char * idString = (char *) malloc(7 * sizeof(char));
        sprintf(idString, "%d", (int) p->id);
        strcat(logFile, idString);
        strcat(logFile, ".lammps\0");

        memcpy(commandString, "#clearing garbage \n \0",21 * sizeof(char) );
        strcat(commandString, "log\t");
        strcat(commandString, logFile);
        strcat(commandString, "\necho\tscreen\n");
//      first = 1;

        strcat(commandString, "boundary\tp p p\n");
        strcat(commandString, "units\treal\n");
        strcat(commandString, "atom_style\tcharge\n");
        strcat(commandString, "read_data\t");
        strcat(commandString, geo);
        strcat(commandString, "\n");
        strcat(commandString, "pair_style\ttersoff\n");
        strcat(commandString, "pair_coeff\t* * ");
        strcat(commandString, paramFile);
        strcat(commandString, " Pt\n");
        strcat(commandString, "neighbor\t2 bin\n");
        strcat(commandString, "neigh_modify\tevery 10 delay 0 check no\n");
        strcat(commandString, "minimize\t1.0e-4 1.0e-6 100 1000\n");
        strcat(commandString, "timestep\t0.25\n \0");
        //first = 1;
    //}
    /**else
    {
        memcpy(commandString, "#trying to remove garbage \n \0", 27 * sizeof(char));
        strcat(commandString, "read_data\t");
        strcat(commandString, geo);
        strcat(commandString, "\npair_coeff\t** ");
        strcat(commandString, paramFile);
        strcat(commandString, " Pt\n");
    }**/
    //printf("%s \n", commandString);

    //void * lmps;
    //lammps_open_no_mpi(0, NULL, &lmps);  //Initialize LAMMPS pointer
    //Submit command to LAMMPS
    lammps_commands_string(lmps, commandString);

    //Read standardOutput into buffer
    //read(npipe[0], buf, sizeof(buf));

    //Run LAMMPS simulation 1 time
    lammps_command(lmps, "run\t1\n");

    //read(npipe[0], buf, sizeof(buf));

    //dup2(saved_stdout, STDOUT_FILENO);

    //printf("%s \n", buf);

    lammps_command(lmps, "clear\n");

    /**std::stringstream ss;
    ss.str(buf);
    std::string line;
    double etotal;
    for ( int i = 0; i < 8; i++ )
    {
        std::getline( ss, line );
    }
    for ( int i = 0; i < 5; i++ )
    {
         ss >> etotal;
    }
**/
    free(commandString);
    //free(idString);
    //lammps_free(lmps);
    //lammps_close(lmps);
    char * line = (char *) malloc(sizeof(char) * 1000);
    file = fopen(logFile, "r");
    double energy = 0.0;
    while(fscanf(file, " %s", line) == 1)
    {
        if(strcmp("Energy", line) == 0)
        {
            for(j = 0; j < 4; j++)
            {
                fscanf(file, " %s", line);
            }
            fscanf(file, " %lf", &energy); //first energy
            fscanf(file, " %lf", &energy); //second energy
            fscanf(file, " %lf", &energy); //third energy

            break;
        }
    }
    free(line);
    free(idString);
    free(logFile);
    fclose(file);
    //printf("\n\n\n\n\n Energy: %f \n\n\n\n\n", energy);
    return energy;
}

//This will be used to calculate a single error value. All error values will then be summed together
float calcError(const float g1,const float g2, float p1, float p2) 
{
    float diff1 = p1 - p2;
    float diff2 = g1 - g2;
    float diff3 = diff2 - diff1;
    float errorVal = pow(diff3, 2);
    return errorVal / weight;   
}

float  getFitness(ParSet * p)
{
    //start by creating and populating a .tersoff file containing all the parameters in p
    char * paramFile = writeTersoffFile(p);
    float p1 = getPotential(p, "geo/gPtbcc.in", paramFile); //Get potential with two different geometries and compare them. (Can expand later
    float p2 = getPotential(p, "geo/g0.93xPtbcc.in", paramFile);
    //float p3 = getPotential(p, "geo/g0.95xPtbcc.in", paramFile);
    //float p4 = getPotential(p, "geo/g0.97xPtbcc.in", paramFile);
    //float p5 = getPotential(p, "geo/g1.03xPtbcc.in", paramFile);
    //float p6 = getPotential(p, "geo/g1.05xPtbcc.in", paramFile);
    //float p7 = getPotential(p, "geo/g1.07xPtbcc.in", paramFile);

    float p8 = getPotential(p, "geo/gPtfcc.in", paramFile); 
    float p9 = getPotential(p, "geo/g0.93xPtfcc.in", paramFile);
    //float p10 = getPotential(p, "geo/g0.95xPtfcc.in", paramFile);
    //float p11 = getPotential(p, "geo/g0.97xPtfcc.in", paramFile);
    //float p12 = getPotential(p, "geo/g1.03xPtfcc.in", paramFile);
    //float p13 = getPotential(p, "geo/g1.05xPtfcc.in", paramFile);
    //float p14 = getPotential(p, "geo/g1.07xPtfcc.in", paramFile);
    
    float p15 = getPotential(p, "geo/gPtSC.in", paramFile); 
    float p16 = getPotential(p, "geo/g0.93xPtsc.in", paramFile);
    //float p17 = getPotential(p, "geo/g0.95xPtsc.in", paramFile);
    //float p18 = getPotential(p, "geo/g0.97xPtsc.in", paramFile);
    //float p19 = getPotential(p, "geo/g1.03xPtsc.in", paramFile);
    //float p20 = getPotential(p, "geo/g1.05xPtsc.in", paramFile);
    //float p21 = getPotential(p, "geo/g1.07xPtsc.in", paramFile);

    float p22 = getPotential(p, "geo/gPthcp.in", paramFile); 
    float p23 = getPotential(p, "geo/g0.93Pthcp.in", paramFile);
    //float p24 = getPotential(p, "geo/g0.95Pthcp.in", paramFile);
    //float p25 = getPotential(p, "geo/g0.97Pthcp.in", paramFile);
    //float p26 = getPotential(p, "geo/g1.03xPthcp.in", paramFile);
    //float p27 = getPotential(p, "geo/g1.05xPthcp.in", paramFile);
    //float p28 = getPotential(p, "geo/g1.07xPthcp.in", paramFile);
    
    //float p2 = getPotential("DataPtbcc.in", paramFile);
    float totalError = 0.0; 
    
    totalError += calcError(PTBCC, PTBCC93, p1, p2); 
    //totalError += calcError(PTBCC, PTBCC95, p1, p3); 
    //totalError += calcError(PTBCC, PTBCC97, p1, p4);
    //totalError += calcError(PTBCC, PTBCC103, p1, p5);
    //totalError += calcError(PTBCC, PTBCC105, p1, p6);
    //totalError += calcError(PTBCC, PTBCC107, p1, p7);
    
    totalError += calcError(PTFCC, PTFCC93, p8, p9);
    //totalError += calcError(PTFCC, PTFCC95, p8, p10);
    //totalError += calcError(PTFCC, PTFCC97, p8, p11);
    //totalError += calcError(PTFCC, PTFCC103, p8, p12);
    //totalError += calcError(PTFCC, PTFCC105, p8, p13);
    //totalError += calcError(PTFCC, PTFCC107 , p8, p14);
    
    totalError += calcError(PTSC, PTSC93, p15, p16);
    //totalError += calcError(PTSC, PTSC95, p15, p17);
    //totalError += calcError(PTSC, PTSC97, p15, p18);
    //totalError += calcError(PTSC, PTSC103, p15, p19);
    //totalError += calcError(PTSC, PTSC105, p15, p20);
    //totalError += calcError(PTSC, PTSC107, p15, p21);
    
    totalError += calcError(PTHCP, PTHCP93, p22, p23);
    //totalError += calcError(PTHCP, PTHCP93, p22, p24);
    //totalError += calcError(PTHCP, PTHCP93, p22, p25);
    //totalError += calcError(PTHCP, PTHCP93, p22, p26);
    //totalError += calcError(PTHCP, PTHCP93, p22, p27);
    //totalError += calcError(PTHCP, PTHCP93, p22, p28);

    
    char * logFile = (char *)malloc(20 * sizeof(char));
    memcpy(logFile, "log\0", 4 * sizeof(char));
    char * idString = (char *) malloc(7 * sizeof(char));
    sprintf(idString, "%d", (int) p->id);
    strcat(logFile, idString);
    strcat(logFile, ".lammps\0");
    remove(logFile);
    free(logFile);
    free(idString);
    remove(paramFile);
    free(paramFile);
    
    return totalError; 
}

void getFitnessAll(ParSet ** p, int setID) //This function will get the fitness scores for the specified subset of individuals
{
    if(setID == 0) //0 means get fitness for first 100 ParSets
    {
        //printf("Finding fitness of first 100 individuals \n");
        for(i = 0; i < 100; i++)
        {
            pars[i]->error = getFitness(pars[i]);
        }
    }
    else if(setID == 1)
    {
        //printf("Finding fitness of second 100 individuals \n");
        for(i = 100; i < 200; i++)
        {
            pars[i]->error = getFitness(pars[i]);
        }
    }
    else {
        //printf("Something is wrong! We shouldn't have entered this branch!");
    }
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
      //printf("Starting mutation. Number to undergo mutation is %d \n", mutateMax);
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
    //Step 1. Get initial parameters
    //printf("About to initialize parameters \n");
    initialParameters(pars); //we will make a ParSet array p, and populate it with initial parameters
    //printf("Initial parameters initialized \n");
    //Step 2. Enter loop
    gettimeofday(&t1, NULL);
    //printf("Entering while loop with 1st generation \n");
    currentBest = pars[1];
    oldBest = pars[0];
    //int numGens = 0;
    //printf("About to get fitness for first 100 individuals \n");
    getFitnessAll(pars,0);
    while((currentBest != oldBest) )//&& (numGens < 100))
    {
        //printf("About to perform genetic operations \n");
        geneticOperations();
        //printf("About to calculate fitness for newly generated individuals \n");
        getFitnessAll(pars, 1);
        //printf("About to rank the parSets \n");
        rankParSets(pars,0);
        //printf("Checking for convergence \n");
        if(currentBest != NULL)
        {
            oldBest = currentBest;
        }
        currentBest = pars[0];
        //printf("OldBest = %f, %f, newBest = %f, %f \n", oldBest->error, oldBest->id, currentBest->error, currentBest->id);

//      numGens ++;
    }
    //printf("Exitting while loop \n");
    //simplex();
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
    getrusage(RUSAGE_SELF, &memUsed);
    //printf("Printing results \n");
    printResults();
    //printf("Freeing allocated memory \n");
    freeAll();
    lammps_close(lmps);
    //printf("Exitting");
    return 0;
}
