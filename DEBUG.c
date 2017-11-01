#include "mainHeader.h"

void displayKM(struct kmeans * KM,const char * displayText)
{
	printf("%s: \n",displayText);
	printf("dim = %d, ndata = %d, k = %d \n", KM->dim, KM->ndata, KM->k);

	printf("Working data array: \n");
	printDataArray(KM->data, KM->dim, KM->ndata);

	printf("cluster_size: \n");
	printArrayInt(KM->cluster_size, KM->k, "size of cluster");

	printf("cluster_start: \n");
	printArrayInt(KM->cluster_start, KM->k, "start of cluster");

	printArrayDouble(KM->cluster_radius, KM->k, "radius of cluster", "cluster_radius:");

	printArraysDouble(KM->cluster_centroid, KM->k, KM->dim, "centroid of cluster", "cluster_centroid:");

	printf("cluster_assign: \n");
	printArrayInt(KM->cluster_assign, KM->ndata, "cluster of DP");

	return;
}

void printArrayInt(int * ArrayInt, int size, const char * text)
{
	int i;
	for (i = 0; i < size; i++)
	{
		printf("%s %d = %d, \n", text, i, ArrayInt[i]);
	}
	return;
}

void printArraysDouble(double ** ArrayDouble, int size, int dim, const char * text, const char * headingText)
{
	int i, j;
	printf("%s \n",headingText);
	for (i = 0; i < size; i++)
	{
		printf("%s %d = ( ", text, i);
		for (j = 0; j < dim; j++)
		{
			if (j != dim - 1)
			{
				printf("%lf,", ArrayDouble[i][j]);
			}
			else
			{
				printf("%lf", ArrayDouble[i][j]);
			}
		}
		if (i != size - 1)
		{
			printf("),\n");
		}
		else
		{
			printf(")\n");
		}
	}
	return;
}

void printArrayKMD(double * array, int size)
{
	int i;
	printf("[ ");
	for (i = 0; i < size; i++)
	{
		printf("%lf ", array[i]);
	}
	printf("]\n");
}

void printDataArray(double * dataArray, int dim, int ndata)
{
	int i, j;
	for (i = 0; i < ndata; i++)
	{
		int first_dim_of_data_point = dim * i;
		for (j = 0; j < dim; j++)
		{
			if (SHOW_DP_NUMBER) { printf("%d -> %lf, ", i,dataArray[first_dim_of_data_point + j]); }
			else { printf("%lf, ", dataArray[first_dim_of_data_point + j]); }

		}
		printf("\n");
	}
	return;
}

void writeResults(int dim, int ndata, double* data, int* cluster_assign)
{
	int i;
	FILE* file = fopen("data.txt", "w");

	fprintf(file, "%d\n", dim);
	fprintf(file, "%d\n", ndata);
	for (i = 0; i < dim * ndata; i++)
		fprintf(file, "%lf\n", data[i]);

	for (i = 0; i < ndata; i++)
		fprintf(file, "%d\n", cluster_assign[i]);

	fclose(file);
}

void write_results_parallel(int dim, int ndata, double *data, int *cluster_assign, int k, double **cluster_centroids, int world_rank)
{
    FILE *output_file;
    char file_name[100];
    sprintf(file_name, "data_%d.txt", world_rank);
    output_file = fopen(file_name, "w");
    // Write the number of data points
       fprintf(output_file, "%d\n", ndata);
    // Write out the data points and their cluster assignments
    int i, j;
    for (i = 0; i < ndata; i++)
    {
			for (j = 0; j < dim; j++)
     		fprintf(output_file, "%lf ", data[i * dim + j]);
	    fprintf(output_file, "%d \n", cluster_assign[i]);
    }
    fclose(output_file);
    if (world_rank == 0)
    {
        // Write centroid information
        output_file = fopen("centroids.txt", "w");
        fprintf(output_file, "%d\n", k);
        for (i = 0; i < k; i++)
				{
            for (j = 0; j < dim; j++)
                fprintf(output_file, "%lf ", cluster_centroids[i][j]);
            fprintf(output_file, "\n");
        }
        fclose(output_file);
    }
		return;
}

void printArray(int * nums, int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		printf("%d ", nums[i]);
	}
	printf("\n");
}

void printArrayDoubles(double * nums, int ndata, int dim)
{
	int i,j,first_index;
	for (i = 0; i < ndata; i++)
	{
		first_index = i * dim;
		for (j = 0; j < dim; j++)
		{
			printf("%lf ", nums[first_index + j]);
		}
		printf("\n");
	}
	printf("\n");
}

void printArrayDouble(double * ArrayDouble, int size, const char * text, const char * headingText)
{
	int i, j;
	printf("%s \n",headingText);
	for (i = 0; i < size; i++)
	{
		printf("%s %d = ( ", text, i);
		for (j = 0; j < 1; j++)
		{
			if (j != 0)
			{
				printf("%lf,", ArrayDouble[i]);
			}
			else
			{
				printf("%lf", ArrayDouble[i]);
			}
		}
		if (i != size - 1)
		{
			printf("),\n");
		}
		else
		{
			printf(")\n");
		}
	}
	return;
}

void printStack(struct stackBase *stack)
{
	struct stackNode * iterator = stack->firstNode;
	int i;
	printf("Stack: \n");
	printf("stackDepth = %d\n", stack->stackDepth);
	while (iterator != NULL)
	{
		for (i = 0; i < stack->arraySize; i++)
		{
			printf("dim: %d = %lf, \n", i, (iterator->pointArray)[i]);
		}
		printf("distance = %lf, cluster = %d ", iterator->distance, iterator->cluster);
		printf("\n\n");
		iterator = iterator->nextNode;
	}
}

void printRStack(struct stackRBase * stack)
{
	struct stackRNode * iterator = stack->firstNode;
	int i,j = 0;
	printf("Stack: \n");
	printf("stackDepth = %d\n", stack->stackDepth);
	while (iterator != NULL)
	{
		printf("Region %d: \n",j);j++;
		for (i = 0; i < iterator->dim; i++)
		{
			printf("dim: %d = %lf, width = %lf \n", i, (iterator->lowerCorner)[i], (iterator->widths)[i]);
		}
		printf("number of points = %d ", iterator->numPoints);
		printf("\n\n");
		iterator = iterator->nextNode;
	}
	printf("\n\n");
}

void displaySelectedFromKM(struct kmeans * KM, int singleValues, int dataArray, int cluster_size, int cluster_start, int cluster_radius, int cluster_centroid, int cluster_assign)
{
	printf("Kmeans: \n");

	if (singleValues)
	{
		printf("dim = %d, ndata = %d, k = %d \n", KM->dim, KM->ndata, KM->k);
	}

	if (dataArray)
	{
		printf("Working data array: \n");
		printDataArray(KM->data, KM->dim, KM->ndata);
	}

	if (cluster_size)
	{
		printf("cluster_size: \n");
		printArrayInt(KM->cluster_size, KM->k, "size of cluster");
	}

	if (cluster_start)
	{
		printf("cluster_start: \n");
		printArrayInt(KM->cluster_start, KM->k, "start of cluster");
	}

	if (cluster_radius)
	{
		printArrayDouble(KM->cluster_radius, KM->k, "radius of cluster", "cluster_radius:");
	}

	if (cluster_centroid)
	{
		printArraysDouble(KM->cluster_centroid, KM->k, KM->dim, "centroid of cluster", "cluster_centroid:");
	}

	if (cluster_assign)
	{
		printf("cluster_assign: \n");
		printArrayInt(KM->cluster_assign, KM->ndata, "cluster of DP");
	}

	return;
}
