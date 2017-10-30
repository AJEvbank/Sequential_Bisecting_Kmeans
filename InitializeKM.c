#include "mainHeader.h"


void initializeKM(struct kmeans ** KM, int dim, int ndata, double * dataArray, int k)
{
  *KM = (struct kmeans *)malloc(sizeof(struct kmeans));
  (*KM)->dim = dim;
  (*KM)->ndata = ndata;
  (*KM)->data = dataArray;
  (*KM)->k = k;
  (*KM)->cluster_size = allocateAndInitializeZeroInt(k);
  (*KM)->cluster_start = allocateAndInitializeZeroInt(k);
  (*KM)->cluster_radius = allocateAndInitializeZeroDouble(k);
  (*KM)->cluster_assign = allocateAndInitializeZeroInt(ndata);
  (*KM)->cluster_centroid = allocateAndInitializeZeroDoubleMulti(k,dim);
  return;
}

int * allocateAndInitializeZeroInt(int size_of_target)
{
	int * target = (int *)malloc(size_of_target * sizeof(int));
	int i = 0;
	for (i = 0; i < size_of_target; i++) {
		target[i] = 0;
	}
	return target;
}

double ** allocateAndInitializeZeroDoubleMulti(int k, int dimension)
{
	double ** target = (double **)malloc(sizeof(double *)* k);
	int i,j;
	for (i = 0; i < k; i++)
	{
		target[i] = (double *)malloc(sizeof(double) * dimension);
		for (j = 0; j < dimension; j++)
		{
			target[i][j] = 0;
		}
	}
	return target;
}

double * allocateAndInitializeZeroDouble(int size_of_target)
{
	double * target = (double *)malloc(size_of_target * sizeof(double));
	int i = 0;
	for (i = 0; i < size_of_target; i++) {
		target[i] = 0;
	}
	return target;
}

void bkm(struct kmeans ** KM, int dim, int ndata, double * dataArray, int k)
{
  // Initialize the kmeans structure.
  initializeKM(KM,dim,ndata,dataArray,k);
  // Get initial k centroids.
  GetKCentroids(*KM);
  // Generate clusters in the kmeans structure.
  ClusterizeKM(*KM);
  return;
}

void destroyKM(struct kmeans * KM)
{
  int i;
  for (i = 0; i < KM->k; i++)
  {
    free((KM->cluster_centroid)[i]);
  }
  free(KM->cluster_centroid);
  free(KM->cluster_assign);
  free(KM->cluster_radius);
  free(KM->cluster_start);
  free(KM->cluster_size);
  free(KM);
  return;
}
