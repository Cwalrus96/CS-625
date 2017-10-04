//Caleb Compton
//2017-09-22
//INCLUDES 

#include<stdlib.h>
#include<stdio.h>
#include<mpi.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<string.h>

//MAIN 

int main(int argc, char** argv) 
{
	//1. GLOBAL VARIABLES 
	char ** wiki; 
	char ** dict; 
	int ** globalIndex; 
	int ** localIndex; 
	int numLines, numWords; 
	int err; 
	int i; 
	int numWiki = 1000000; 
	int numDict = 50000; 
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
	//2. Initialize MPI
	MPI_Init(&argc, &argv); 
	MPI_Comm_size(MPI_COMM_WORLD, &numCores);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	//3. All Processes will initialize their arrays 
	wiki = (char **) malloc(numWiki * sizeof(char *));
	wiki[0] = (char *) malloc(numWiki * lineLength * sizeof(char)); 
	for(i = 1; i < numWiki; i++) 
	{
		wiki[i] = wiki[i - 1] + lineLength; 
	} 
	dict = (char **) malloc(numDict * sizeof(char *));
	dict[0] = (char *) malloc(numDict * wordLength * sizeof(char)); 
	for(i = 1; i < numDict; i ++) 
	{
		dict[i] = dict[i - 1] + wordLength; 
	}
	//4. Only server will populate it's arrays, then broadcast to all processes 
	if(rank == 0) 
	{
		f = fopen("/homes/cwalrus96/hw3/keywords.txt", "r"); 
		if(NULL == f) {
			perror("FAILED: "); 
			return -1; 
		} 
		numWords = 0; 
		while(err != EOF && numWords < numDict) 
		{
			err = fscanf(f, "%[^\n]\n", dict[numWords]); 
			numWords ++; 
		}
		fclose(f); 
		f = fopen("/homes/cwalrus96/hw3/wiki_dump.txt", "r"); 
		if(NULL == f) {
			perror("FAILED: "); 
			return -1; 
		}
		while(err != EOF && numLines < numWiki) 
		{
			err = fscanf(f, "%[^\n]\n", wiki[numLines]); 
			numLines ++; 
		}
		fclose(f); 
		gettimeofday(&t1, NULL); 

	}
	MPI_Bcast(wiki[0], lineLength * numWiki, MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Bcast(dict[0], wordLength * numDict, MPI_CHAR, 0, MPI_COMM_WORLD);
	//5. Each array will search a different portion of the dictionary, based on their rank. They will store the indexes in their localIndex array 
	start = (numDict / numCores) * rank; 
	end = start + (numDict / numCores); 
	if(rank == numCores - 1) end = numDict; 
	range = end - start;
	localIndex = (int **) malloc(range * sizeof(int *)); 
	for(i = 0; i < range; i++) {
		localIndex[i] = (int *) malloc(indexSize * sizeof(int)); 
		localIndex[i][0] = 2; 
		localIndex[i][1] = indexSize; 
	}
	for(numWords = start; numWords < end; numWords ++) 
	{ 
		for(numLines = 0; numLines < numWiki; numLines ++) 
		{
			if(strstr(wiki[numLines], dict[numWords]) != NULL) 
			{
				tempIndex = localIndex[numWords - start][0];
				tempSize = localIndex[numWords - start][1];
					if(tempIndex >= tempSize)
					{
						tempSize *= 2; 
						localIndex[numWords - start] = (int *) realloc(localIndex[numWords - start], tempSize * sizeof(int)); 
						localIndex[numWords - start][1] = tempSize; 
					}
				localIndex[numWords - start][tempIndex] = numLines; 
				localIndex[numWords - start][0] ++; 
			}
		}
	}
	//6. All processes will send their temporary arrays (1 line at a time) to the root.
	if(rank != 0) {
		for(i = start; i < end; i++) 
		{	
			MPI_Send(localIndex[i - start], localIndex[i - start][1], MPI_INT, 0, i, MPI_COMM_WORLD);
		}
	}
	//7. The root will collect these arrays into one large global index
	if(rank == 0) {
		globalIndex = (int **) malloc(sizeof(int *) * numDict);
		for(i = start; i < end; i++) {
			globalIndex[i] = localIndex[i]; 
		}
		for(i = end; i < numDict; i++) 
		{
			MPI_Probe(MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &stat); 
			MPI_Get_count(&stat, MPI_INT, &size); 
			globalIndex[i] = (int *) malloc(sizeof(int) * size);
			MPI_Recv(globalIndex[i], size, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &stat); 
		}
	}
	//8. Print out results (master thread only) 
	if(rank == 0) 
	{
		gettimeofday(&t2, NULL); 
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; 
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; 
		getrusage(RUSAGE_SELF, &memUsed); 
		numWords = 0; 
		while((numWords < numDict) && !(NULL == dict[numWords]))
		{
			if(globalIndex[numWords][0] > 2) {
				printf("%-10s: ", dict[numWords]); 
				i = 2; 
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
	//9. Free all arrays and finalize mpi processes 
	free(dict[0]); 
	free(dict); 
	for(numWords = 0; numWords < range; numWords ++) 
	{
		free(localIndex[numWords]); 
	}
	free(localIndex); 
	if(rank == 0) 
	{
		for(i = 0; i < numDict; i ++) 
		{
			free(globalIndex[i]); 
		} 
		free(globalIndex); 
	}
	free(wiki[0]); 
	free(wiki); 
	MPI_Finalize(); 
	return 0; 
}
