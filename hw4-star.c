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
char ** globalWiki;
char ** localWiki;
char ** dict;
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

void initGlobalArrays()
{
  printf("initializing global arrays \n");
  globalWiki = (char **) malloc(numWiki * sizeof(char *)); //initialize global wiki
  globalWiki[0] = (char *) malloc(numWiki * lineLength * sizeof(char)); //make it one large chunk of memory
  for(i = 1; i < numWiki; i++) //ever index is immediately following the previous in memory
    {
      globalWiki[i] = globalWiki[i - 1] + lineLength;
    }
  dict = (char **) malloc(numDict * sizeof(char *)); //initialize global dictionary as one large memory chunk
  dict[0] = (char *) malloc(numDict * wordLength * sizeof(char));
  for(i = 1; i < numDict; i ++)
    {
      dict[i] = dict[i - 1] + wordLength;
    }
  localIndex = (int *) malloc(101 * sizeof(int));
  globalIndex = (int **) malloc(sizeof(int *) * numDict); //initialize globalIndex to store results
  printf("globalIndex will have %d rows \n", numDict);
  int rowLength = ((100 * (numCores - 1)) + 1);
  printf("each row will have length of %d \n", rowLength);
  globalIndex[0] = (int *) malloc(numDict * rowLength * sizeof(int));
  globalIndex[0][0] = 1;
  for(i = 1; i < numDict; i++) {
    //printf("initializing globalIndex[%d] \n", i);
    globalIndex[i] = globalIndex[i - 1] + (rowLength);
    globalIndex[i][0] = 1;
    //printf("%d \n",globalIndex[i][0]);
  }
}

void initLocalArrays()
{
  printf("rank %d initializing local arrays \n", rank);
  localWiki = (char **) malloc(range * sizeof(char *));
  localWiki[0] = (char *) malloc(range * lineLength * sizeof(char));
  for(i = 1; i < range; i++)
    {
      localWiki[i] = localWiki[i - 1] + lineLength;

    }
  localIndex = (int *) malloc(101 * sizeof(int));
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
  numWords = 0;
  while(err != EOF && numWords < numDict) //populates dictionary array with keywords
    {
      err = fscanf(f, "%[^\n]\n", dict[numWords]);
      numWords ++;
    }
  fclose(f);
  f = fopen("/homes/cwalrus96/hw3/wiki_dump.txt", "r"); //modify address with correct file location
  if(NULL == f) {
    perror("FAILED: ");
    exit(-1);
  }
  while(err != EOF && numLines < numWiki) //populates wiki array
    {
      err = fscanf(f, "%[^\n]\n", globalWiki[numLines]);
      numLines ++;
    }
  fclose(f);
}

void distributeArrays()
{
  printf("distributing arrays \n");
  for(i = 1; i < numCores; i++) //sends to every core from 1...n
    {
      start = (numWiki / (numCores - 1)) * (i - 1);  //get start and end ranges to break up the array
      end = start + (numWiki / (numCores - 1));
      if(i == numCores - 1) end = numDict;
      range = end - start;
      MPI_Send(globalWiki[start], range * sizeof(char) * lineLength, MPI_CHAR, i, 0, MPI_COMM_WORLD); //sends every core their part
    }

}

void searchArrays()
{
  printf("rank %d searching arrays \n", rank);
  int count;
  for(numWords = 0; numWords < numDict; numWords ++) //For every word in the dictionary
    {
      printf("rank %d gonna search for word number %d \n", rank, numWords);
      count = 0;
      MPI_Send(&numWords, sizeof(int), MPI_INT, 0, 2, MPI_COMM_WORLD); //Send a message to root asking for next word (tag 2)
      printf("rank %d send message asking for word %d \n", rank, numWords);
      MPI_Recv(word, sizeof(char) * 12, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); //recieve next word
      printf("rank %d just recieved the word %s \n", rank, word);
      localIndex[0] = numWords; //first entry in index is the ID of the word
      for(numLines = 0; numLines < range; numLines ++) //searches for word and adds indexes to array
        {
          //if((numLines + start) % 100 == 0) printf("rank %d searching wiki line  %d \n", rank, numLines + start);
          if(strstr(localWiki[numLines], word) != NULL && count < 100)
            {
              //printf("rank %d found a match for the word %s on line %d count = %d new count = %d\n", rank, word, numLines + start, count, count + 1);
              count ++;
              localIndex[count] = numLines;
            }
        }
      printf("rank %d sending results for word %d to rank 0 size = %d \n", rank, numWords, count + 1);
      MPI_Send(localIndex, sizeof(int) * (count + 1), MPI_INT, 0, 3, MPI_COMM_WORLD); //send index to root (tag 3)
    }
}

void distributeKeys()
{
  printf("distributing keys \n");
  int source;
  int tag;
  for(i = 1; i < numCores; i++) //every core must get every dictionary word
    {
      for(j = 0; j < numDict * 2; j++) // *2 because each core will be sending two messages when they go through the loop
        {
          MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
          source = stat.MPI_SOURCE;
          tag = stat.MPI_TAG;
          MPI_Get_count(&stat, MPI_INT, &size);
          if(tag == 2) { //tag 2 is asking for the next word
            printf("About to recieve a request from rank %d \n", source);
            MPI_Recv(&numWords, sizeof(int), MPI_INT, source, 2, MPI_COMM_WORLD, &stat);
            printf("recieved a request from rank %d for word %d \n", source, numWords);
            MPI_Send(dict[numWords], sizeof(char) * 12, MPI_CHAR, source, 0, MPI_COMM_WORLD);
          }
          else if(tag == 3) //tag 3 is sending the results
            {
              printf("About to recieve results for a word \n");
              MPI_Recv(localIndex, size, MPI_INT, source, 3, MPI_COMM_WORLD, &stat); //add every int recieved to the global indei
              printf("results recieved \n");
              numLines = size / sizeof(int);
              int tempRow = localIndex[0];
              printf("recieved results from rank %d for word %d size = %d\n", source, tempRow, numLines);
              if(NULL != globalIndex[tempRow]) {
                for(k = 1; k < numLines; k++)
                        {
                          printf("About to get tempIndex \n");
                          int tempIndex = globalIndex[tempRow][0];
                          printf("tempIndex = %d", tempIndex); fflush(stdout);
                          printf("localIndex[k] = %d \n", localIndex[k]);
                          globalIndex[tempRow][tempIndex] = localIndex[k];
                          globalIndex[tempRow][0] ++;
                        }
                }else printf("Why the fuck is %d NULL? \n", tempRow);
            }
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
  free(dict[0]);
  free(dict);
  free(localIndex);
  if(rank == 0)
    {
      free(globalIndex[0]);
      free(globalIndex);
    }
  free(globalWiki[0]);
  free(globalWiki);
  MPI_Finalize();
}

void freeLocal()
{
  printf("rank %d freeing local arrays \n", rank);
  free(localWiki[0]);
  free(localWiki);
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
      gettimeofday(&t1, NULL);
    }
  //5. Server will distribute parts of the wiki to all the other threads, based on their rank. (EDIT)
  if(rank == 0) {
    distributeArrays();
  }
  else
    {
      MPI_Recv(localWiki[0], range * sizeof(char) * lineLength, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
    }
  //6. Each array (besides the server) will recieve keywords from the server (1 at a time)
  //and search their portion of the wiki for that word (up to 100 instances).
  if(rank != 0) {
    searchArrays();
  }
  //7. The server will listen to the other threads and hand out keywords one at a time.
  if(rank == 0)
    {
      distributeKeys();
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

