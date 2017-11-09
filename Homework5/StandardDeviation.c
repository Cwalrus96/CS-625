//Caleb Compton, Nasser Alhamadah and Hayden Svancara 
//2017-08-11 
//Concurrent Systems Homework 5 Part 3 - Computing Standard Deviation 

#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include <sys/time.h>
#include <sys/resource.h>
#include <mpi.h> 

//Part 1 - Global Variables 
int N = 1000000 //This represents how many random numbers should be generated 
int i; 
Struct timeval t1, t2; 
Struct rusage memUsed; 
double elapsedTime; 
int numCores, start, end, range, rank; 

//Part 2 - Functions 
void generateSum();


//Part 3 - Main Function 
int main(int argc, char ** argv) 
{
    //Step 1 - Initialize MPI
    MPI_Initi(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &numCores); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    start = (N / (numCores - 1)) * (rank - 1); 
    end = start + (N / (numCores - 1)); 
    if(rank == (numCores - 1)) end = N; 
    range = end - start; 
    //Step 2 - All worker processes will randomly generate the assigned number of random numbers, and combine them together 
    if(rank != 0) 
    {
        generateSum();
        sendSum(); 
    }
    //Step 3 - Rank 0 recieves the sum of all the randomly generated numbers from the worker processes
    
    //Step 4 - After getting all the sums, Rank 0 finds the mean value and sends it to all the worker processes 
    //Step 5 - Each worker thread uses this mean value to find their individual variants, and these are squared and added together 
    //Step 6 - Worker threads send the sums of these individual deviations to the root node, which computes the overall variance 
    //Step 7 - Finally, Root node takes the square root of the variance to get the standard deviation. 

}
