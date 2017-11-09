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
float sum, mean; 
float * randArray;
float tv; 
float iv; 
float var; 
float dev; 
MPI_Status stat; 

//Part 2 - Functions 
void generateSum() //Worker processes call this function to generate their random number arrays and send the local sums to root
{
    sum = 0; 
    randArray = malloc(sizeof(float) * range); 
    for(i = 0; i < range; i++) 
    {
        randArray[i] = ((float) rand()); 
        sum += randArray[i]; 
    }
    MPI_Send(sum, 1, MPI_FLOAT, 0, rank, MPI_COMM_WORLD);
}

void recieveSums() //root process call this function to gather all local sums 
{
    sum = 0;
    float tempSum = 0; 
    for(i = 0; i < (numCores - 1); i++) 
    {
        MPI_Recv(&tempSum, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); 
        sum += tempSum
    }
}

void getIndividualVariances()
{
    tv = 0; //this variable will hold the total variance
    iv = 0; //this variable will hold the individual variance 
    for(i = 0; i < range; i++)
    {
        iv = pow(randArray[i] - mean, 2); 
        tv += iv; 
    }
    MPI_Send(tv, 1, MPI_FLOAT, 0, rank, MPI_COMM_WORLD); 
}

void getStandarDeviation()
{
    tv = 0; 
    for(i = 0; i < (numCores - 1); i++)
    {
        MPI_Recv(&iv, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); 
        tv += iv; 
    }
    var = tv / N; 
    dev = sqrt(var); 
}



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
    }
    //Step 3 - Rank 0 recieves the sum of all the randomly generated numbers from the worker processes, and calculates the mean
    else 
    {
        gettimeofday(&t1, NULL); 
        recieveSums();       
        mean = sum / N; 
    }
    //Step 4 - After getting all the sums, Rank 0 finds the mean value and sends it to all the worker processes 
    MPI_Bcast(&mean, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    //Step 5 - Each worker thread uses this mean value to find their individual variants, and these are squared and added together
    //These individual variances are then sent to the root process
    if(rank != 0) 
    {
        getIndividualVariances(); 
    }
    //Step 6 - Worker threads send the sums of these individual deviations to the root node, which computes the overall variance 
    //Step 7 - Finally, Root node takes the square root of the variance to get the standard deviation. 
    else
    {
        getStandardDeviation();  
        gettimeofday(&t2, NULL); 
        getrusage(RUSAGE_SELF, &memUsed);
        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000; 
        elapsedTime += (t2.tv_usec - t2.tv_usec) / 1000; 
        printf("Standard Deviation is %f \n", dev); 
        printf("DATA, CORES, %d, TIME, %f, MEMORY, %ld \n", numCores, elapsedTime, memUsed.ru_maxrss) //ADD TIMERS AND MEMORY MANAGEMENT DATA
    }


}
