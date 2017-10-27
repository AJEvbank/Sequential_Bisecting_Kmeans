#include "mainHeader.h"

int main(int argc, char** argv) {

	int dim, ndata, k;
	double max_double;
	unsigned int seedArray[] = { 2, 4, 8, 16 };
	unsigned int querySeed;
	int seedMult,numSeeds = 4;


	getCmdArgs(argc, argv, &dim, &ndata, &k,&max_double,&seedMult);

	printf("Values: dim = %d, ndata = %d, k = %d, max_double = %lf, ndata = %d, numSeeds = %d, seedMult = %d  \n\n", dim, ndata, k, max_double, ndata, numSeeds,seedMult);

	double * dataArray = (double *)malloc(sizeof(double)*ndata*dim);
	double * query = (double *)malloc(sizeof(double)*dim);
	generateRandomArray(dataArray, ndata * dim, max_double, numSeeds, &seedArray[0],seedMult);
	querySeed = seedMult;
	generateRandomArray(query, dim, max_double,1, &querySeed,querySeed);
	printf("Query Point: \n");
	printArrayDoubles(query, 1, dim);


//Now begin building the kmeans structure.
/******************************************************************************************************************/

struct kmeans * KM = NULL;

bkm(&KM,dim,ndata,dataArray,k);

if (KM_DISPLAY) { displaySelectedFromKM(KM,1,1,1,0,1,1,1); }

//The search can now be run.
/*******************************************************************************************************************/

struct stackBase * result = initStack(dim);
int pointsSearched = search(KM,query,result);
printf("->>>%d points searched.\n",pointsSearched);


/**********************************************************************************************************************************/




	//Use brute force search to find the nearest point.

	int isOneResult,i;
	double * LocalBresult = (double *)malloc(sizeof(double)*(dim+1));

	LocalBresult[0] = bruteForceSearch(dataArray, query, dim, ndata, LocalBresult);

	struct stackNode * iterator = result->firstNode;
	printf("\n");
	printf("Result stack data: \n");
	printf("stackDepth = %d, distance to query point = %lf \n",result->stackDepth,result->firstNode->distance);
	printf("\n");
	printf("BRUTE FORCE RESULT: \n");
	printf("minimum distance = %lf \n",LocalBresult[0]);
	while (iterator != NULL)
	{
		isOneResult = checkResult(iterator->pointArray,&LocalBresult[1],dim);
		if (isOneResult)
		{
			printf("\n");
				printf("Search Result: \t\t Brute Force Result: \n");
				for (i = 0; i < dim; i++)
				{
					printf("%lf \t\t %lf \n",(iterator->pointArray)[i],LocalBresult[i+1]);
				}
				printf("In cluster %d \n",iterator->cluster);
				printf("\n");
			break;
		}
		else
		{
			iterator = iterator->nextNode;
		}
	}
	printf("\n");
	if (isOneResult)
	{
		printf("THE RESULT IS CORRECT. \n");
	}
	else
	{
		printf("THE RESULT IS NOT CORRECT. \n");
	}
	printf("\n");

if (dim == 2)
{
	write_results_parallel((KM)->dim,(KM)->ndata,(KM)->data,(KM)->cluster_assign,(KM)->k,(KM)->cluster_centroid,0);
}


	clearStack(result);
	free(result);
	free(LocalBresult);
	free(dataArray);
	free(query);
	//MPI_Finalize();
}
