//Caleb Compton
//2017-09-22
//INCLUDES

#include<stdlib.h>
#include<stdio.h>
#include<mpi.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<string.h>

//1. GLOBAL VARIABLES
char ** wiki;
char *** jobs;
char ** job; 
int ** globalIndex;
int * localIndex;
char * word;
int numLines, numWords;
int err;
int i,j,k;
int numWiki = 1000;
int numDict = 1000;
int lineLength = 2001;
int wordLength = 10;
int indexSize = 10;
struct timeval t1, t2;
struct rusage memUsed;
FILE *f;
int numCores, start, end, range, size, tempIndex, tempSize;
int rank;
double elapsedTime;
MPI_Status stat;
int jobSize = 100; 
int jobNumber; 

void initGlobalArrays()
{
  printf("initializing global arrays \n");
  wiki = (char **) malloc(numWiki * sizeof(char *)); //initialize global wiki
  wiki[0] = (char *) malloc(numWiki * lineLength * sizeof(char)); //make it one large chunk of memory
  for(i = 1; i < numWiki; i++) //every index is immediately following the previous in memory
    {
      wiki[i] = wiki[i - 1] + lineLength;
    }
  //jobs is an array of jobs. Each job is a two dimensional char array made up of 100 words
  int numJobs = numDict / jobSize; 
  jobs = (char ***) malloc(numJobs * sizeof(char **));
  for(i = 0; i < numJobs; i ++) //Initialize each job
    {
      jobs[i] = (char **) malloc(jobSize * sizeof(char *)); //each job is a contiguous piece of memory
      jobs[i][0] = (char *) malloc( jobSize * wordLength * sizeof(char));
      for(j = 1; j < jobSize; j++) //initialize each word
      {
        jobs[i][j] = jobs[i][j - 1] +  wordLength; 
      } 
    }
  globalIndex = (int **) malloc(sizeof(int *) * numDict); //initialize globalIndex to store results
  printf("globalIndex will have %d rows \n", numDict);
  globalIndex[0] = (int *) malloc(numDict * (jobSize + 1) * sizeof(int));
  for(i = 1; i < numDict; i++) {
    //printf("initializing globalIndex[%d] \n", i);
    globalIndex[i] = globalIndex[i - 1] + (jobSize + 1);
    //printf("%d \n",globalIndex[i][0]);
  }
}

void initLocalArrays()
{
  wiki = (char **) malloc(numWiki * sizeof(char *)); //initialize global wiki
  wiki[0] = (char *) malloc(numWiki * lineLength * sizeof(char)); //make it one large chunk of memory
  for(i = 1; i < numWiki; i++) //every index is immediately following the previous in memory
    {
      wiki[i] = wiki[i - 1] + lineLength;
    }
  job = (char **) malloc(jobSize * sizeof(char *)); 
  job[0] = (char *) malloc(jobSize * wordLength * sizeof(char)); //job is one chunk 
  for(i = 1; i < jobSize; i++) {
    job[i] = job[i - 1] + wordLength;    
  }
  localIndex = (int **) malloc(jobSize * sizeof(int *)) //localIndex will hold the results - 100 lines, each 101 integers long.
  localIndex[0] = (int *) malloc((jobSize + 1) * jobSize * sizeof(int)); //One chunk, for better communication
  for(i = 1; i < jobSize; i++) 
  {
    localIndex[i] = localIndex[i - 1] + (jobSize + 1); 
    localIndex[i][0] = 1; //first entry in each row is the index of the next open spot 
  }
  word = (char *) malloc(12 * sizeof(char));
}

void initArrays()
{
        if(rank == 0)
        {
                initGlobalArrays();
        }
        else
        {
                initLocalArrays();
        }
}

void popArrays()
{
  printf("populating arrays \n");
  f = fopen("/homes/cwalrus96/hw3/keywords.txt", "r"); //reads the keywords file (address must be modified to be correct location)
  if(NULL == f) {
    perror("FAILED: ");
    exit(-1);
  }
  for(i = 0; i < numJobs; i++) 
    {
      for(j = 0; j < jobSize; j++) {
        err = fscanf(f, "%[^\n]\n", jobs[i][j]);
      }
    }
  fclose(f);
  f = fopen("/homes/cwalrus96/hw3/wiki_dump.txt", "r"); //modify address with correct file location
  if(NULL == f) {
    perror("FAILED: ");
    exit(-1);
  }
  while(err != EOF && numLines < numWiki) //populates wiki array
    {
      err = fscanf(f, "%[^\n]\n", wiki[numLines]);
      numLines ++;
    }
  fclose(f);
}


void searchArrays()
{
  printf("rank %d searching arrays \n", rank);
  int count;
  jobNumber = -2; 
  while(jobNumber != -1) //keep requesting jobs until none are left (tag = -1)
    {
      MPI_Send(&jobNumber, sizeof(int), MPI_INT, 0, -1, MPI_COMM_WORLD); //Send a message to root asking for next job (tag -1)
      MPI_Recv(job[0], jobSize * wordLength, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); //recieve next job
      jobNumber = stat.MPI_TAG; 
      printf("rank %d just recieved job number %s \n", rank, jobNumber);
      if(jobNumber != -1) {
          for(i = 0; i < jobSize; i ++) //Goes through each word in the dictionary
          {
              for(j = 0; j < numWiki; j++) //goes through every line of the Wiki
              {
                if(strstr(wiki[j], job[i]) != NULL && localIndex[i][0] <= 100)
                {
                  localIndex[i][localIndex[i][0]] = j; 
                  localIndex[i][0] ++; 
                }
          }
          printf("rank %d sending results for word %d to rank 0 size = %d \n", rank, numWords, count + 1);
          MPI_Send(localIndex,(jobSize) * (jobSize + 1), MPI_INT, 0, jobNumber, MPI_COMM_WORLD); //send index to root (tag jobNumber)
      }
    }
}

void distributeJobs()
{
  printf("distributing jobs \n");
  int source;
  int tag;
  jobNumber = 0; 
  for(i = 0; i < ((numJobs * 2) + (numCores - 1)); i++) //Go through the loop twice for every job, plus once for every core 
                                                        //Each job needs to be sent out and results recieved. When all jobs are done, 
                                                        //every process needs to recieve a -1
    {
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
      source = stat.MPI_SOURCE;
      tag = stat.MPI_TAG;
      MPI_Get_count(&stat, MPI_INT, &size);
      if(tag == -1) { //tag -1 is asking for the next word
        if(jobNumber <= numJobs) { //still jobs to give out - give next job!
            printf("About to recieve a request from rank %d \n", source);
            MPI_Recv(&numWords, sizeof(int), MPI_INT, source, -1, MPI_COMM_WORLD, &stat);
            printf("recieved a request from rank %d for job %d \n", source, jobNumber);
            MPI_Send(jobs[jobNumber][0], jobSize * wordLength, MPI_CHAR, source, jobNumber, MPI_COMM_WORLD);
            jobNumber ++; 
        }
        else { //no more jobs - send a tag of -1
            MPI_Recv(&numWords, 1, MPI_INT, source, -1, MPI_COMM_WORLD, &stat);
            MPI_Send(jobs[0][0], jobSize * wordLength, MPI_CHAR, source, -1, MPI_COMM_WORLD);
            jobNumber ++; 
        }
          
      }
      else //other tags are returning job results 
      {
          printf("About to recieve results for a word \n");
          MPI_Recv(globalIndex[100 * tag], size, MPI_INT, source, tag, MPI_COMM_WORLD, &stat); //add every int recieved to the global index
          printf("results recieved for job %d \n", tag);
      }
    }      
}

void printResults(int argc, char ** argv)
{
  printf("printing results \n");
  elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
  elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
  getrusage(RUSAGE_SELF, &memUsed);
  numWords = 0;
  while((numWords < numDict) && !(NULL == dict[numWords]))
    {
      if(globalIndex[numWords][0] > 1) {
        printf("%-10s: ", dict[numWords]);
        i = 1;
        while(!(NULL == globalIndex[numWords]) && (i < globalIndex[numWords][0]))
          {
            printf("%d, ", globalIndex[numWords][i]);
            i++;
          }
        printf(" \n");
      }
      numWords ++;
    }
  int groupSize;
  if(argc == 1) {
    groupSize = numCores;
  }
  else groupSize = atoi(argv[1]);
  double secs = (elapsedTime/ 1000.0);
  int hours = secs / 3600;
  secs = secs - (3600 * hours);
  int minutes = secs/60;
  secs = secs - (60 * minutes);
  printf("DATA, %d, %d, %d:%d:%f, %ld, \n", numCores,groupSize, hours, minutes, secs, memUsed.ru_maxrss);


}


void freeGlobal()
{
  printf("freeing global arrays \n");
  for(i = 0; i < numJobs; i++) 
  {
    free(jobs[i][0]); 
    free(jobs[i]); 
  }
  free(jobs);
  if(rank == 0)
    {
      free(globalIndex[0]);
      free(globalIndex);
    }
  free(wiki[0]);
  free(wiki);
  MPI_Finalize();
}

void freeLocal()
{
  printf("rank %d freeing local arrays \n", rank);
  free(wiki[0]); 
  free(wiki); 
  free(jobs[0]); 
  free(jobs); 
  free(localIndex);
  free(word);
}

void freeAll() {
        if(rank == 0)
        {
                freeGlobal();
        }
        else freeLocal();
}

//MAIN
int main(int argc, char** argv)
{

  //2. Initialize MPI (done)
  printf("initializing MPI \n");
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numCores);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  start = (numWiki / (numCores - 1)) * (rank - 1);
  end = start + (numWiki / (numCores - 1));
  printf("rank = %d, start = %d, end = %d \n", rank, start, end);
  if(rank == numCores - 1) end = numDict;
  range = end - start;
  //3. All Processes will initialize their arrays
  //Server will initialize global arrays, clients will initialize local arrays
  initArrays();
  //4. Only server will populate it's arrays
  if(rank == 0)
    {
      popArrays();
    }
  //5. Server will distribute the entire wiki to all threads 
  printf("broadcasting wiki \n");
  MPI_Bcast(wiki[0], lineLength * numWiki, MPI_CHAR, 0, MPI_COMM_WORLD);
  //6. Each array (besides the server) will request "jobs" from the server (sets of 100 keywords)
  if(rank != 0) {
    searchArrays();
  }
  //7. The server will listen to the other threads and hand out jobs 1 at a time 
  if(rank == 0)
    {
      gettimeofday(&t1, NULL);
      distributeJobs();
    }
  //8. Print out results (master thread only)
  if(rank == 0)
    {
      gettimeofday(&t2, NULL);
      printResults(argc, argv);
    }
  //9. Free all arrays and finalize mpi processes
  freeAll();
  return 0;
}
