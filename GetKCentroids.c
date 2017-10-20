#include "mainHeader.h"

void GetKCentroids(struct kmeans * KM)
{
  int Bigk = 1,currentCluster = 0;
  double * SSEArray = allocateAndInitializeZeroDouble(KM->k);
  SSEArray[currentCluster] = INFINITY;
  (KM->cluster_size)[currentCluster] = KM->ndata;
  while (Bigk < KM->k)
  {
      Bigk = Bisect(KM,SSEArray,Bigk,currentCluster);
      currentCluster = LargestSSE(SSEArray,Bigk);
  }
  return;
}

int Bisect(struct kmeans * KM, double * SSEArray, int Bigk, int currentCluster)
{
  int A = currentCluster, B = Bigk;

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

  return Bigk + 1;
}

int LargestSSE(double * SSEArray, int Bigk)
{
  int i,currentCluster;
  double maxSSE = -INFINITY;
  for (i = 0; i < K; i++)
  {
    if (SSEArray[i] > maxSSE)
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
  double * sumsAB = allocateAndInitializeZeroDouble((sizeof(double) * KM->dim)*2);
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
    if ((KM->cluster_centroid)[B][j] != sumsAB[j])
    {
      changed = 1;
    }
    (KM->cluster_centroid)[B][j] = sumsAB[j];
  }
  return changed;
}

void CalculateSSE(struct kmeans * KM, double * SSEArray, int A, int B)
{
  //double distance;
  double * meanDistAB = (double *)malloc(sizeof(double) * 2);
  double * distancesA = (double *)malloc(sizeof(double) * (KM->cluster_size)[A]);
  double * distancesB = (double *)malloc(sizeof(double) * (KM->cluster_size)[B]);
  int i,first_index,stepA = -1,stepB = -1;
  for ( i = 0; i < KM->ndata; i++)
  {
    if ((KM->cluster_assign)[i] == A)
    {
      stepA++;
      first_index = i * KM->dim;
      distancesA[stepA] = GetDistance2PointsDC(KM,first_index,A);
      meanDistAB[0] += distancesA[stepA];
    }
    else if ((KM->cluster_assign)[i] == B)
    {
      stepB++;
      first_index = i * KM->dim;
      distancesB[stepB] = GetDistance2PointsDC(KM,first_index,B);
      meanDistAB[1] += distancesB[stepB];
    }
  }

  meanDistAB[0] /= (KM->cluster_size)[A];
  meanDistAB[1] /= (KM->cluster_size)[B];

  SSEArray[A] = Sigma(distancesA,(KM->cluster_size)[A],meanDistAB[0]);
  SSEArray[B] = Sigma(distancesB,(KM->cluster_size)[B],meanDistAB[1]);

  return;
}

double Sigma(double * distances, int size, double meanDist)
{
  double SSE;
  int i;
  for (i = 0; i < size; i++)
  {
    SSE += pow( fabs(distances[i] - meanDist), 2);
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
