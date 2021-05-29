#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Cache
{
	int valid;
	int tag;
} Cache;

int main(int argc, char *argv[])
{

	int cacheSize = atoi(argv[1]);
	int blockSize = atoi(argv[4]);
	int numSets;
	int assoc;	//num lines per set

	if (strcmp(argv[2], "direct") == 0)
	{
		assoc = 1;
		numSets = cacheSize / blockSize;
	}
	else if (strcmp(argv[2], "assoc") == 0)
	{
		assoc = cacheSize / blockSize;
		numSets = 1;
	}
	else if (strncmp(argv[2], "assoc:", 6) == 0)
	{
		if (strcmp(argv[2] + 6, "0") == 0 || strcmp(argv[2] + 6, "") == 0)
		{
			printf("error\n");
			return 0;
		}
		assoc = atoi(argv[2] + 6); 
		numSets = cacheSize / blockSize / assoc;
		if (floor(log2(assoc)) != ceil(log2(assoc)))
		{
			printf("error\n");
			return 0;
		}
	}
	else
	{
		printf("error\n");
		return 0;
	}

	int repPol; //0:fifo 1:lru
	if (strcmp(argv[3], "fifo") == 0)
		repPol = 0;
	else if (strcmp(argv[3], "lru") == 0)
		repPol = 1;
	else
	{
		printf("error\n");
		return 0;
	}

	int blockOff = log2(blockSize);
	int setBit = log2(numSets);

	if (floor(log2(cacheSize)) != ceil(log2(cacheSize)) ||
		floor(log2(blockSize)) != ceil(log2(blockSize)))
	{
		printf("error\n");
		return 0;
	}

	int memRead = 0;
	int memWrite = 0;
	int cacheHit = 0;
	int cacheMiss = 0;

	FILE *traceFile = fopen(argv[5], "r");

	Cache **cache = malloc(numSets * sizeof(Cache *));
	for (int i = 0; i < numSets; i++)
	{
		cache[i] = calloc(assoc, sizeof(Cache));
	}

	//counter for lru
	int **counterLine = malloc(numSets * sizeof(int *));
	for (int i = 0; i < numSets; i++)
	{
		counterLine[i] = calloc(assoc, sizeof(int));
	}
	//counter for fifo
	int *counterSet = calloc(numSets, sizeof(int));

	int lruCnt = 0;

	char op;
	unsigned long long address;

	while (fscanf(traceFile, "%*s %c 0x%llx ", &op, &address) != EOF)
	{
		int setInd = (address >> blockOff) & (unsigned long long)(pow(2, setBit) - 1);
		int tag = (address >> (blockOff + setBit));

		//write
		if (op == 'W')
		{
			int isHit = 0;
			for (int i = 0; i < assoc; i++)
			{
				if (!cache[setInd][i].valid)
					break;

				if (cache[setInd][i].tag == tag)
				{
					isHit = 1;
					if (repPol) //update lru counter
					{
						counterLine[setInd][i] = lruCnt;
						lruCnt++;
					}
					break;
				}
			}
			if (isHit)
			{
				cacheHit++;
				memWrite++;
			}
			else
			{ //miss
				int isFull = 1;
				cacheMiss++;
				memWrite++;
				memRead++;
				for (int j = 0; j < assoc; j++)
				{
					if (!cache[setInd][j].valid)
					{
						isFull = 0;
						cache[setInd][j].valid = 1;
						cache[setInd][j].tag = tag;
						//update lru counter
						if (repPol)
						{
							counterLine[setInd][j] = lruCnt;
							lruCnt++;
						}
						break;
					}
				}
				if (isFull)
				{
					if (repPol)
					{ //lru
						int ptr = counterLine[setInd][0];
						int cnt = 0;
						for (int i = 0; i < assoc; i++)
						{
							if (counterLine[setInd][i] < ptr)
							{
								ptr = counterLine[setInd][i];
								cnt = i;
							}
						}
						cache[setInd][cnt].tag = tag;
						counterLine[setInd][cnt] = lruCnt;
						lruCnt++;
					}
					else
					{ //fifo
						int cnt = counterSet[setInd] % assoc;
						cache[setInd][cnt].tag = tag;
						counterSet[setInd]++;
					}
				}
			}
		}

		else
		{ //op == 'R'
			int isHit = 0;
			for (int i = 0; i < assoc; i++)
			{
				if (!cache[setInd][i].valid)
					break;

				if (cache[setInd][i].tag == tag)
				{
					isHit = 1;
					//update lru counter
					if (repPol)
					{
						counterLine[setInd][i] = lruCnt;
						lruCnt++;
					}
					break;
				}
			}

			if (isHit)
			{
				cacheHit++;
			}

			else
			{ //miss
				int isFull = 1;
				cacheMiss++;
				memRead++;
				for (int j = 0; j < assoc; j++)
				{
					if (!cache[setInd][j].valid)
					{
						isFull = 0;
						cache[setInd][j].valid = 1;
						cache[setInd][j].tag = tag;
						//update lru counter
						if (repPol)
						{
							counterLine[setInd][j] = lruCnt;
							lruCnt++;
						}
						break;
					}
				}
				if (isFull)
				{
					if (repPol)
					{ //lru
						int ptr = counterLine[setInd][0];
						int cnt = 0;
						for (int i = 0; i < assoc; i++)
						{
							if (counterLine[setInd][i] < ptr)
							{
								ptr = counterLine[setInd][i];
								cnt = i;
							}
						}
						cache[setInd][cnt].tag = tag;
						counterLine[setInd][cnt] = lruCnt;
						lruCnt++;
					}
					else
					{ //fifo
						int cnt = counterSet[setInd] % assoc;
						cache[setInd][cnt].tag = tag;
						counterSet[setInd]++;
					}
				}
			}
		}
	}

	printf("Memory reads: %d\n", memRead);
	printf("Memory writes: %d\n", memWrite);
	printf("Cache hits: %d\n", cacheHit);
	printf("Cache misses: %d\n", cacheMiss);

	for (int i = 0; i < numSets; i++)
	{
		free(cache[i]);
		free(counterLine[i]);
	}
	free(cache);
	free(counterLine);

	free(counterSet);

	return 0;
}