#ifndef COMMANDLINEARGS

#define COMMANDLINEARGS


#include "mainHeader.h"

void getCmdArgsWithOptions(int argc, char ** argv, int * dim, int * ndata, int * k, double * max_double,int * seedMult)
{
	*dim = DIM; *ndata = NDATA; *k = K; *max_double = MAX_DOUBLE; *seedMult = SEEDMULT;
	static char * SHORT = "";
	static struct option OPTS[] =
																{
																	{"dim",required_argument,0,'d'},
																	{"ndata",required_argument,0,'n'},
																	{"k",required_argument,0,'k'},
																	{"max",required_argument,0,'m'},
																	{"seed",required_argument,0,'s'}
																};
	int ch;
	int opt_index = 0;
	while(1)
	{
		ch = getopt_long_only(argc,argv,SHORT,OPTS,&opt_index);
		if (ch == -1)
		{
			break;
		}

		switch(ch)
		{
			case 'd':
							if (isNumber(optarg))
							{
								*dim = (int)atof(optarg);
							}
							if (*dim <= 0)
							{
								*dim = DIM;
							}
							break;
			case 'n':
							if (isNumber(optarg))
							{
								*ndata = (int)atof(optarg);
							}
							if (*ndata <= 0)
							{
								*ndata = NDATA;
							}
							break;
			case 'k':
							if (isNumber(optarg))
							{
								*k = (int)atof(optarg);
							}
							if (*k <= 0)
							{
								*k = K;
							}
							break;
			case 'm':
							if (isNumber(optarg))
							{
								*max_double = atof(optarg);
							}
							if (*max_double <= 0)
							{
								*max_double = MAX_DOUBLE;
							}
							break;
			case 's':
							if (isNumber(optarg))
							{
								*seedMult = (int)atof(optarg);
							}
							if (*seedMult <= 0)
							{
								*seedMult = SEEDMULT;
							}
							break;
			default:
							break;
		}
	}
	return;
}

int isNumber(const char * str)
{
	int i;
	int len = strlen(str);
	char ch;
	enum isNumStates state = INITIAL;
	if (len > 0)
	{
		for (i = 0; i < len; i++)
		{
			ch = str[i];
			switch (state)
			{
			case INITIAL:
			{
				if (ch == '-' || ch == '+')
				{
					state = PLUSORMINUS;
				}
				else if (ch == '0')
				{
					state = ZERO;
				}
				else if (isdigit(ch))
				{
					state = NUMBER;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case PLUSORMINUS:
			{
				if (ch == '0')
				{
					state = ZERO;
				}
				else if (isdigit(ch))
				{
					state = NUMBER;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case ZERO:
			{
				if (ch == '.')
				{
					state = DECIMAL;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case NUMBER:
			{
				if (isdigit(ch))
				{
					state = NUMBER;
				}
				else if (ch == '.')
				{
					state = DECIMAL;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case DECIMAL:
			{
				if (isdigit(ch))
				{
					state = DECIMAL;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case ERROR:
			{
				return 0;
				break;
			}

			default:
			{
				printf("default in isNumber: %d \n", (int)state);
				return 0;
				break;
			}

			}
		}
		if (state == ERROR)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

void generateRandomArrayRegions(double max_double, unsigned int seedMult, int dim, struct stackRBase * regions, int numRegions, double UpperBound, double LowerBound, int maxSize, int minSize)
{
	int i,j,size;
	double randomScale;
	double * widths = allocateAndInitializeZeroDouble(dim);
	double * lowerCorner = allocateAndInitializeZeroDouble(dim);
	srand(seedMult);
	for (i = 0; i < numRegions; i++)
	{
		for (j = 0; j < dim; j++)
		{
			randomScale = LowerBound + (((double)rand() / (double)RAND_MAX) * (UpperBound - LowerBound));
			widths[j] = ((double)rand() / (double)RAND_MAX) * (max_double * randomScale);
			lowerCorner[j] = ((double)rand() / (double)RAND_MAX) * (max_double - widths[j]);
		}
		size = (int)(minSize + (((double)rand() / (double)RAND_MAX) * (double)(maxSize - minSize)));
		pushRNode(lowerCorner,widths,dim,size,regions);
	}
	return;
}

void generateRandomArrayWithRegions(double * dataArray, int ndata, double max_double, int seeds, unsigned int * seedArray, unsigned int seedMult, struct stackRBase * regions, int dim)
{
	int i,j,first_index,seedPeriod = ndata/seeds,nextSeed = 0, regionCount;
	struct stackRNode * iterator = regions->firstNode;
	if (iterator != NULL) { regionCount = iterator->numPoints; }
	else { regionCount = 0; }
	for (i = 0; i < ndata; i++)
	{
		if (i % seedPeriod == 0)
		{
			srand(seedArray[nextSeed] * seedMult);
			nextSeed++;
		}
		first_index = i * dim;
		if (iterator != NULL && regionCount > 0)
		{
			for (j = 0; j < dim; j++)
			{
				dataArray[first_index + j] = (iterator->lowerCorner)[j] + (((double)rand() / (double)RAND_MAX) * (iterator->widths)[j]);
			}
			regionCount--;
		}
		else if (iterator != NULL && regionCount == 0)
		{
			iterator = iterator->nextNode;
			if (iterator != NULL)
			{
				regionCount = iterator->numPoints;
				for (j = 0; j < dim; j++)
				{
					dataArray[first_index + j] = (iterator->lowerCorner)[j] + (((double)rand() / (double)RAND_MAX) * (iterator->widths)[j]);
				}
				regionCount--;
			}
			else
			{
				regionCount = -1;
				for (j = 0; j < dim; j++)
				{
					dataArray[first_index + j] = ((double)rand() / (double)RAND_MAX) * max_double;
				}
			}
		}
		else
		{
			for (j = 0; j < dim; j++)
			{
				dataArray[first_index + j] = ((double)rand() / (double)RAND_MAX) * max_double;
			}
		}
	}
	return;
}

void generateRandomArray(double * dataArray, int domain, double max_double, int seeds, unsigned int * seedArray)
{
	int i,j,first_index;
	for (i = 0; i < seeds; i++)
	{
		srand(seedArray[i]);
		first_index = i * domain/seeds;
		for (j = 0; j < domain / seeds; j++)
		{
			dataArray[first_index + j] = ((double)rand() / (double)RAND_MAX) * max_double;
		}
	}
	return;
}

double bruteForceSearch(double * dataArray, double * query, int dim, int ndata, double * bruteForceResult)
{
	int first_index, i, j, nearestPoint;
	double minDist = INFINITY, calcDist = 0;


	// For each data point, calculate the distance to the query point and compare it to the current
	// minimum distance and see if it is the new current nearest point.
	for (i = 0; i < ndata; i++)
	{
		calcDist = 0;
		first_index = i * dim;
		for (j = 0; j < dim; j++)
		{
			calcDist += pow(fabs(query[j] - dataArray[first_index + j]), 2);
		}
		calcDist = sqrt(calcDist);
		if (calcDist < minDist)
		{
			minDist = calcDist;
			nearestPoint = i;
		}
	}

	// Set the brute force result array with the values of the nearest point.
	first_index = nearestPoint * dim;
	for (i = 0; i < dim; i++)
	{
		bruteForceResult[i+1] = dataArray[(first_index) + i];
	}
	return minDist; //Return the distance to the nearest point.
}

int checkResult(double * pointA, double * pointB, int dim)
// Test whether two points are the same.
{
	int i;
	for (i = 0; i < dim; i++)
	{
		if(pointA[i] != pointB[i])
		{
			return 0;
		}
	}
	return 1;
}


#endif
