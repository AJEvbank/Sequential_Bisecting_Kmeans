#include "mainHeader.h"

void GetKCentroids(struct kmeans * KM)
{
  // We start with one cluster which is the entire data set.
  int numClusters = 1,currentCluster = 0;
  double * SSEArray = allocateAndInitializeZeroDouble(KM->k);
  (KM->cluster_size)[currentCluster] = KM->ndata;

  // Now we generate clusters using the bisection algorithm.
  while (numClusters < KM->k)
  {

      // Bisect the currently selected cluster. This will be cluster 0 initially.

      numClusters = Bisect(KM,SSEArray,numClusters,currentCluster);

      // Select the cluster with the highest SSE to be the next currently selected cluster.

      currentCluster = LargestSSE(KM,SSEArray,numClusters);

  }
  return;
}

int Bisect(struct kmeans * KM, double * SSEArray, int numClusters, int currentCluster)
{

  int A = currentCluster, // A is the cluster we are bisecting.
      B = numClusters;    // B is the new cluster we are creating.

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

  return numClusters + 1; // Return the new number of clusters.
}

int LargestSSE(struct kmeans * KM, double * SSEArray, int numClusters)
{
  int i,currentCluster;
  double maxSSE = -INFINITY;
  for (i = 0; i < numClusters; i++)
  {
    // Note: Will not attempt to subdivide a cluster of size 1.
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
  // Randomly select the Rth point in the cluster.
  int randomPoint = rand() % (KM->cluster_size)[currentCluster];
  int i,step = -1,firstPoint = 0;

  // Iterate over the data array.
  for (i = 0; i < KM->ndata; i++)
  {
    // If a point is in the target cluster, increment the count.
    if ((KM->cluster_assign)[i] == currentCluster)
    {
      step++;
      // If the current point is the Rth point in the cluster, return its index.
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

  // Iterate over the data set.
  for (i = 0; i < KM->ndata; i++)
  {
    // If a point is in the current cluster...
    if ((KM->cluster_assign)[i] == currentCluster)
    {
      // ...get the distance between the current point and the first point.
      first_index_Point = i * KM->dim;
      distance = GetDistance2Points(KM,first_index_Point,first_index_firstPoint);
      // If the distance is greater than the max distance from the point, set the
      // new furthest point to the current point.
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
  // Iterate the data set.
  for (i = 0; i < KM->ndata; i++)
  {
    // If a point is in cluster A or B...
    if ((KM->cluster_assign)[i] == A || (KM->cluster_assign)[i] == B)
    {
      // ...get the distance to each centroid...
      first_index = i * KM->dim;
      distanceA = GetDistance2PointsDC(KM,first_index,A);
      distanceB = GetDistance2PointsDC(KM,first_index,B);
      // ...and assign the point to the cluster with the nearest centroid.
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
  // Prepare an array to hold the new centroids for A and B.
  double * sumsAB = allocateAndInitializeZeroDouble((KM->dim)*2);
  // Iterate the data set.
  for (i = 0; i < KM->ndata; i++)
  {
    // If a point is in cluster A...
    if ((KM->cluster_assign)[i] == A)
    {
      // ...add the point's values to the sum array for A.
      first_index = i * KM->dim;
      for (j = 0; j < KM->dim; j++)
      {
        sumsAB[j] += (KM->data)[first_index + j];
      }
    }
    // If a point is in cluster B...
    else if ((KM->cluster_assign)[i] == B)
    {
      // ...add the point's values to the sum array for B.
      first_index = i * KM->dim;
      for (j = 0; j < KM->dim; j++)
      {
        sumsAB[KM->dim + j] += (KM->data)[first_index + j];
      }
    }
  }
  // Iterate over the sum array for A and B.
  for(i = 0,j = KM->dim; i < KM->dim; i++,j++)
  {
    // Divide each value by the size of each cluster.
    sumsAB[i] /= (KM->cluster_size)[A];
    sumsAB[j] /= (KM->cluster_size)[B];
  }
  // The sum array now contains the new centroids for A and B.
  // Iterate over the sum array for A and B.
  for (i = 0,j = KM->dim; i < KM->dim; i++,j++)
  {
    // If the centroid has changed, set changed.
    if ((KM->cluster_centroid)[A][i] != sumsAB[i])
    {
      changed = 1;
    }
    // Set the centroid of cluster A to the newly calculated centroid.
    (KM->cluster_centroid)[A][i] = sumsAB[i];
    // If the centroid has changed, set changed.
    if ((KM->cluster_centroid)[B][i] != sumsAB[j])
    {
      changed = 1;
    }
    // Set the centroid of cluster B to the newly calculated centroid.
    (KM->cluster_centroid)[B][i] = sumsAB[j];
  }
  free(sumsAB);
  return changed; // Return true if either centroid has changed, false otherwise.
}

void CalculateSSE(struct kmeans * KM, double * SSEArray, int A, int B)
{
  // Prepare arrays to hold the distances between each point and the centroid in
  // each of clusters A and B.
  double * distancesA = allocateAndInitializeZeroDouble((KM->cluster_size)[A]);
  double * distancesB = allocateAndInitializeZeroDouble((KM->cluster_size)[B]);
  int i,first_index,stepA = -1,stepB = -1;
  // Iterate over the data set.
  for ( i = 0; i < KM->ndata; i++)
  {
    // If a point is in cluster A...
    if ((KM->cluster_assign)[i] == A)
    {
      // ...calculate its distance to the centroid and save it in the distance array for A.
      stepA++;
      first_index = i * KM->dim;
      distancesA[stepA] = GetDistance2PointsDC(KM,first_index,A);
    }
      // If a point is in cluster B...
    else if ((KM->cluster_assign)[i] == B)
    {
      // ...calculate its distance to the centroid and save it in the distance array for B.
      stepB++;
      first_index = i * KM->dim;
      distancesB[stepB] = GetDistance2PointsDC(KM,first_index,B);
    }
  }

  // Save the sum of squares for A and B in the SSE array.
  SSEArray[A] = Sigma(distancesA,(KM->cluster_size)[A]);
  SSEArray[B] = Sigma(distancesB,(KM->cluster_size)[B]);

  free(distancesA);
  free(distancesB);
  return;
}

double Sigma(double * distances, int size)
// Return the sum of square errors in the distances array.
{
  double SSE = 0;
  int i;
  for (i = 0; i < size; i++)
  {
    SSE += pow( fabs(distances[i]), 2);
  }
  return SSE;
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
