#include "mainHeader.h"

void GetKCentroids(struct kmeans * KM)
{
  int numClusters = 1,currentCluster = 0;
  double * SSEArray = allocateAndInitializeZeroDouble(KM->k);
  (KM->cluster_size)[currentCluster] = KM->ndata;
  while (numClusters < KM->k)
  {
      if (WP3) { printf("\n"); printf("currentCluster = %d, size = %d \n",currentCluster,(KM->cluster_size)[currentCluster]); displayKM(KM,"Before Bisect: "); }
      numClusters = Bisect(KM,SSEArray,numClusters,currentCluster);
      if (WP1) { displayKM(KM,"After Bisect: "); }
      currentCluster = LargestSSE(KM,SSEArray,numClusters);
      if (WP2) { printf("currentCluster = %d \n",currentCluster); printArrayDouble(SSEArray,(KM->k),"SSE -> ","SSEArray: "); printf("\n"); }
  }
  return;
}

int Bisect(struct kmeans * KM, double * SSEArray, int numClusters, int currentCluster)
{

  int A = currentCluster, B = numClusters;

  /* 1. Find first point. */

  int firstPoint = GetRandomInCluster(KM,currentCluster);

  /* 2. Find second point. */

  int secondPoint = GetFurthestPointInCluster(KM,firstPoint,currentCluster);

  setCentroid(KM,A,firstPoint);

  setCentroid(KM,B,secondPoint);

  /* 3. Iterate while no changes: */

  int changed = 1;
  while (changed == 1)
  {

        /* A. Assign points */

        AssignDPsAB(KM,A,B);

        /* B. Recalculate Centroids */

        changed = RecalculateCentroidsAB(KM,A,B);

  }

  /* 4. Calculate SSEs for both clusters. */

  CalculateSSE(KM,SSEArray,A,B);

  return numClusters + 1;
}

//void displaySelectedFromKM(struct kmeans * KM, int singleValues, int dataArray, int cluster_size, int cluster_start, int cluster_radius, int cluster_centroid, int cluster_assign);

int LargestSSE(struct kmeans * KM, double * SSEArray, int numClusters)
{
  int i,currentCluster;
  double maxSSE = -INFINITY;
  for (i = 0; i < numClusters; i++)
  {
    if (SSEArray[i] > maxSSE && (KM->cluster_size)[i] > 1)
    {
      maxSSE = SSEArray[i];
      currentCluster = i;
    }
  }
  return currentCluster;
}

int GetRandomInCluster(struct kmeans * KM, int currentCluster)
{
  int randomPoint = rand() % (KM->cluster_size)[currentCluster];
  int i,step = -1,firstPoint = 0;
  for (i = 0; i < KM->ndata; i++)
  {
    if ((KM->cluster_assign)[i] == currentCluster)
    {
      step++;
      if (step == randomPoint)
      {
        firstPoint = i;
        return i;
      }
    }
  }
  return firstPoint;
}

int GetFurthestPointInCluster(struct kmeans * KM, int firstPoint, int currentCluster)
{
  int i,furthestPoint,first_index_Point,first_index_firstPoint = firstPoint * KM->dim;
  double maxDist = -INFINITY, distance;
  for (i = 0; i < KM->ndata; i++)
  {
    if ((KM->cluster_assign)[i] == currentCluster)
    {
      first_index_Point = i * KM->dim;
      distance = GetDistance2Points(KM,first_index_Point,first_index_firstPoint);
      if (distance > maxDist)
      {
        maxDist = distance;
        furthestPoint = i;
      }
    }
  }
  return furthestPoint;
}

double GetDistance2Points(struct kmeans * KM, int first_index_Point, int first_index_firstPoint)
{
  int i;
  double distance = 0;
  for (i = 0; i < KM->dim; i++)
  {
    distance += pow( fabs((KM->data)[first_index_Point + i] - (KM->data)[first_index_firstPoint + i]), 2);
  }
  return sqrt(distance);
}

void setCentroid(struct kmeans * KM, int A, int firstPoint)
{
  int i,first_index = firstPoint * KM->dim;
  for (i = 0; i < KM->dim; i++)
  {
    (KM->cluster_centroid)[A][i] = (KM->data)[first_index + i];
  }
  return;
}

void AssignDPsAB(struct kmeans * KM, int A, int B)
{
  int i,first_index;
  double distanceA,distanceB;
  (KM->cluster_size)[A] = 0;
  (KM->cluster_size)[B] = 0;
  for (i = 0; i < KM->ndata; i++)
  {
    if ((KM->cluster_assign)[i] == A || (KM->cluster_assign)[i] == B)
    {
      first_index = i * KM->dim;
      distanceA = GetDistance2PointsDC(KM,first_index,A);
      distanceB = GetDistance2PointsDC(KM,first_index,B);
      if (distanceA < distanceB)
      {
        (KM->cluster_assign)[i] = A;
        (KM->cluster_size)[A] = (KM->cluster_size)[A] + 1;
      }
      else
      {
        (KM->cluster_assign)[i] = B;
        (KM->cluster_size)[B] = (KM->cluster_size)[B] + 1;
      }
    }
  }
  return;
}

int RecalculateCentroidsAB(struct kmeans * KM, int A, int B)
{
  int changed = 0,i,j,first_index;
  double * sumsAB = allocateAndInitializeZeroDouble((KM->dim)*2);
  for (i = 0; i < KM->ndata; i++)
  {
    if ((KM->cluster_assign)[i] == A)
    {
      first_index = i * KM->dim;
      for (j = 0; j < KM->dim; j++)
      {
        sumsAB[j] += (KM->data)[first_index + j];
      }
    }
    else if ((KM->cluster_assign)[i] == B)
    {
      first_index = i * KM->dim;
      for (j = 0; j < KM->dim; j++)
      {
        sumsAB[KM->dim + j] += (KM->data)[first_index + j];
      }
    }
  }
  for(i = 0,j = KM->dim; i < KM->dim; i++,j++)
  {
    sumsAB[i] /= (KM->cluster_size)[A];
    sumsAB[j] /= (KM->cluster_size)[B];
  }
  for (i = 0,j = KM->dim; i < KM->dim; i++,j++)
  {
    if ((KM->cluster_centroid)[A][i] != sumsAB[i])
    {
      changed = 1;
    }
    (KM->cluster_centroid)[A][i] = sumsAB[i];
    if ((KM->cluster_centroid)[B][i] != sumsAB[j])
    {
      changed = 1;
    }
    (KM->cluster_centroid)[B][i] = sumsAB[j];
  }
  free(sumsAB);
  return changed;
}

void CalculateSSE(struct kmeans * KM, double * SSEArray, int A, int B)
{
  double * distancesA = allocateAndInitializeZeroDouble((KM->cluster_size)[A]);
  double * distancesB = allocateAndInitializeZeroDouble((KM->cluster_size)[B]);
  int i,first_index,stepA = -1,stepB = -1;
  for ( i = 0; i < KM->ndata; i++)
  {
    if ((KM->cluster_assign)[i] == A)
    {
      stepA++;
      first_index = i * KM->dim;
      distancesA[stepA] = GetDistance2PointsDC(KM,first_index,A);
    }
    else if ((KM->cluster_assign)[i] == B)
    {
      stepB++;
      first_index = i * KM->dim;
      distancesB[stepB] = GetDistance2PointsDC(KM,first_index,B);
    }
  }

  if (WP4) { printf("\n");
             displayKM(KM,"Before CalculateSSE: ");
             printArrayDouble(distancesA,(KM->cluster_size)[A],"distancesA -> ","distancesA: ");
             printArrayDouble(distancesB,(KM->cluster_size)[B],"distancesB -> ","distancesB: "); }

  SSEArray[A] = Sigma(distancesA,(KM->cluster_size)[A]);
  SSEArray[B] = Sigma(distancesB,(KM->cluster_size)[B]);

  if (WP4) { printf("\n");
             printArrayDouble(SSEArray,(KM->k),"SSE -> ","SSEArray: "); }

  free(distancesA);
  free(distancesB);
  return;
}

double Sigma(double * distances, int size)
{
  double SSE = 0;
  int i;
  for (i = 0; i < size; i++)
  {
    SSE += pow( fabs(distances[i]), 2);
  }
  return SSE;
}










int GetNextCluster(struct kmeans * KM, int numCentroids)
{
  int i,j,nextCentroid,first_index;
  double * minDistArray = allocateAndInitializeZeroDouble((KM->ndata));
  double minDist = INFINITY,maxminDist = -INFINITY, distance;

  for (i = 0; i < KM->ndata; i++)
  {
    first_index = (KM->dim) * i;
    minDist = INFINITY;
    for (j = 0; j < numCentroids; j++)
    {
      distance = GetDistance2PointsDC(KM,first_index,j);
      if (distance < minDist)
      {
        minDist = distance;
        minDistArray[i] = distance;
      }
    }
  }
  for (i = 0; i < KM->ndata; i++)
  {
    if (minDistArray[i] > maxminDist)
    {
      maxminDist = minDistArray[i];
      nextCentroid = i;
    }
  }
  for (i = 0; i < KM->dim; i++)
  {
    (KM->cluster_centroid)[numCentroids][i] = (KM->data)[(nextCentroid * KM->dim) + i];
  }
  free(minDistArray);
  return numCentroids + 1;
}

double GetDistance2PointsDC(struct kmeans *KM, int first_index, int centroid)
{
  int i;
  double distCalc = 0;
  for (i = 0; i < KM->dim; i++)
  {
    distCalc += pow( fabs((KM->data)[first_index + i] - (KM->cluster_centroid)[centroid][i]), 2);
  }
  distCalc = sqrt(distCalc);
  return distCalc;
}
