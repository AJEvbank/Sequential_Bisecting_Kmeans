#include "mainHeader.h"

int search(struct kmeans * KM, double * query, struct stackBase * result)
{
  int loop_control = 1,pointCount = 0;
  /* Calculate distances to clusters. */
  double * ClusterDistances = allocateAndInitializeZeroDouble(KM->k);
  int nearestCluster = GetClusterDistances(KM,query,ClusterDistances),candidateCluster;

  /* Iterate: do-while */
  /* Find nearest point in nearest cluster. Distance = straight line - radius */
  do {
      pointCount += GetNearestPoint(KM,result,query,nearestCluster);
      ClusterDistances[nearestCluster] = INFINITY;
      // printArrayDouble(ClusterDistances,KM->k,"distance =>","ClusterDistances");
      candidateCluster = GetNearestCluster(ClusterDistances,KM->k,(result->firstNode)->distance,nearestCluster);
      /* If there is a cluster K nearer than the current nearest point,
         then reiterate searching K to update the nearest point. */
      if (candidateCluster != nearestCluster)
      {
        nearestCluster = candidateCluster;
      }
      else
      {
        loop_control = 0;
      }
  }while(loop_control);
  return pointCount;
}

int GetClusterDistances(struct kmeans * KM, double * query, double * ClusterDistances)
{
  int i,nearestCluster;
  double distance, minDist = INFINITY;
  // Iterate over the clusters.
  for (i = 0; i < KM->k; i++)
  {
    // Calculate the distance to the cluster.
    distance = GetDistance2PointsQC(KM,query,i);
    distance = distance - (KM->cluster_radius)[i];
    // If the query point is inside the cluster, the distance is 0.
    if (distance < 0)
    {
      distance = 0;
    }
    ClusterDistances[i] = distance;
    // Get the minimum distance to a cluster.
    if (distance < minDist)
    {
      minDist = distance;
      nearestCluster = i;
    }
  }
  return nearestCluster;
}

double GetDistance2PointsQC(struct kmeans * KM, double * query, int cluster)
// Returns the distance from a query point to a centroid.
{
  int i;
  double distCalc = 0;
  for (i = 0; i < KM->dim; i++)
  {
    distCalc += pow( fabs(query[i] - (KM->cluster_centroid)[cluster][i]), 2);
  }
  distCalc = sqrt(distCalc);
  return distCalc;
}

int GetNearestPoint(struct kmeans * KM, struct stackBase * result, double * query, int nearestCluster)
{
  double * thisPoint = (double *)malloc(sizeof(double)*KM->dim);
	int i,j,k, dataPoint,start = (KM->cluster_start)[nearestCluster],size = (KM->cluster_size)[nearestCluster];
	double distanceCalculating;
  // Iterate over all the data points in nearestCluster.
	for (i = start; i < start + size; i++)
	{
		dataPoint = i * KM->dim;
		distanceCalculating = 0;
    // Calculate the distance to each data point.
		for (j = 0; j < KM->dim; j++)
		{
			distanceCalculating += pow( fabs((query[j] - (KM->data)[dataPoint+j])), 2);
		}
		distanceCalculating = sqrt(distanceCalculating);
    // If there are no points saved on the stack, save the current point and distance.
		if (result->stackDepth == 0) {
			for (k = 0 ; k < KM->dim; k++) { thisPoint[k] = (KM->data)[dataPoint + k]; }
			pushNode(thisPoint, distanceCalculating, nearestCluster, result);
		}
		else
		{
      // If the current data point is closer than the saved data point(s), clear the stack
      // and save the current data point.
			if (distanceCalculating < (result->firstNode)->distance)
			{
				for (k = 0; k < KM->dim; k++) { thisPoint[k] = (KM->data)[dataPoint + k]; }
				clearStack(result);
				pushNode(thisPoint, distanceCalculating, nearestCluster, result);
			}
      // If the current data point is equidistant with the saved data point(s), push the
      // current data point onto the stack.
			else if (distanceCalculating == (result->firstNode)->distance)
			{
				for (k = 0; k < KM->dim; k++) { thisPoint[k] = (KM->data)[dataPoint + k]; }
				pushNode(thisPoint, distanceCalculating, nearestCluster, result);
			}
		}
	}
	free(thisPoint);
	return (KM->cluster_size)[nearestCluster];
}

int GetNearestCluster(double * ClusterDistances, int size, double minDist, int nearestCluster)
// Returns the number of the nearest cluster.
{
  int i;
  for (i = 0; i < size; i++)
  {
    if (ClusterDistances[i] <= minDist)
    {
      return i;
    }
  }
  return nearestCluster;
}
