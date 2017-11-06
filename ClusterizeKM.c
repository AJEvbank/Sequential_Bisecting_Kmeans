#include "mainHeader.h"


void ClusterizeKM(struct kmeans * KM)
{
  int i,changed;
  for (i = 0,changed = 1; changed != 0; i++)
  {

  /* Assign each data point to the cluster with the nearest centroid */

  AssignDPs(KM);

  /* For each cluster, recalculate the centroid based on the data oints newly assigned. */

  changed = RecalculateCentroids(KM);

  /* Repeat the iteration until cluster assignments do not change. */

  }

  SaveClusters(KM);

  return;
}

void AssignDPs(struct kmeans * KM)
{
  int i,j,first_index,assignment;
  double distance = 0, minDist;
  // Iterate the data set.
  for (i = 0; i < (KM->ndata); i++)
  {
    // Calculate the distance to each centroid.
    first_index = i * (KM->dim);
    minDist = INFINITY;
    for (j = 0; j < (KM->k); j++)
    {
      distance = GetDistance2PointsDC(KM, first_index, j);
      // If the distance is less than the minimum distance to a centroid,
      // make that cluster the current assignment.
      if (distance < minDist)
      {
        minDist = distance;
        assignment = j;
      }
    }
    // Assign the data point to the selected cluster.
    (KM->cluster_assign)[i] = assignment;
  }

  return;
}

int RecalculateCentroids(struct kmeans * KM)
{
  int * newClusterSizes = allocateAndInitializeZeroInt(KM->k);
  double * centroids = allocateAndInitializeZeroDouble(KM->k * KM->dim);
  int i,j,group,group_coordinate,first_index,changed = 0;

  /* Get the sums of each dimension in each cluster */
  for ( i = 0; i < KM->ndata; i++)
  {
    group = (KM->cluster_assign)[i];
    newClusterSizes[group] += 1;
    group_coordinate = group * KM->dim;
    first_index = i * KM->dim;
    for (j = 0; j < KM->dim; j++)
    {
      centroids[group_coordinate + j] += (KM->data)[first_index + j];
    }
  }

  /* Divide the sum of each dimension in each cluster by the size of the cluster. */
  for ( i = 0; i < KM->k; i++)
  {
    first_index = i * KM->dim;
    for ( j = 0; j < KM->dim; j++)
    {
      centroids[first_index + j] /= (double)newClusterSizes[i];
    }
  }

  /* Save the newly calculated centroids in the kmeans structure. */
  for ( i = 0; i < KM->k; i++)
  {
    first_index = i * KM->dim;
    for ( j = 0; j < KM->dim; j++)
    {
      if ((KM->cluster_centroid)[i][j] != centroids[first_index + j])
      {
        (KM->cluster_centroid)[i][j] = centroids[first_index + j];
        changed = 1;
      }
    }
  }

  /* Save the newly calculated cluster sizes in the kmeans structure. */
  for ( i = 0; i < KM->k; i++)
  {
    if ((KM->cluster_size)[i] != newClusterSizes[i])
    {
      (KM->cluster_size)[i] = newClusterSizes[i];
      changed = 1;
    }
  }

  free(newClusterSizes);
  free(centroids);
  return changed;
}

void SaveClusters(struct kmeans * KM)
{
  /* Sort the data array (locally) by cluster assignment */

  SortDataArray(KM);

  /* Set the cluster start values. */

  int i,count = 0;
  for ( i = 0; i < (KM->k); i++)
  {
    (KM->cluster_start)[i] = count;
    count += (KM->cluster_size)[i];
  }

  /* Calculate the radius of each cluster. */

  CalculateRadii(KM);

  /* Deal with empty clusters. */

  EmptyClusters(KM);

  return;
}

void SortDataArray(struct kmeans * KM)
{
  quickSort(KM, 0, (KM->ndata)-1);
  return;
}

void CalculateRadii(struct kmeans * KM)
{
  double * MaxRadii = allocateAndInitializeZeroDouble(KM->k);
  int i,j,start;
  double distance = 0;
  // Iterate over each cluster.
  for (i = 0; i < (KM->k); i++)
  {
    // Iterate over the data points in the cluster.
    for (j = 0; j < (KM->cluster_size)[i]; j++)
    {
      // Calculate the distance for each data point.
      start = (KM->cluster_start)[i];
      distance = GetDistance2PointsDC(KM,(start + j)*KM->dim,i);
      // If the distance is greater than the current max, set the current distance
      // as the max.
      if (distance > MaxRadii[i])
      {
        MaxRadii[i] = distance;
      }
    }
  }
  // Set the cluster radii with the calculated maximum radii.
  for (i = 0; i < (KM->k); i++)
  {
    (KM->cluster_radius)[i] = MaxRadii[i];
  }
  free(MaxRadii);
  return;
}

void EmptyClusters(struct kmeans * KM)
{
  int i,limit = KM->k;

  // Iterate over the cluster size array in reverse order.
  for (i = limit-1; i > -1; i--)
  {
    // If a cluster is empty, delete it.
    if((KM->cluster_size)[i] == 0)
    {
      DeleteEmptyCluster(KM,i);
    }
  }
  return;
}

void DeleteEmptyCluster(struct kmeans * KM, int cluster)
{
  int i,limit = KM->k;

  /* Free the centroid. */
  free((KM->cluster_centroid)[cluster]);
  (KM->cluster_centroid)[cluster] = NULL;

  /* Adjust the cluster assignment indices. */
  for (i = (KM->cluster_start)[cluster]; i < (KM->ndata); i++)
  {
    if ((KM->cluster_assign)[i] >= cluster)
    {
      (KM->cluster_assign)[i] = (KM->cluster_assign)[i] + 1;
    }
  }

  /* Shift back size, radius, start, and centroid. */
  for (i = cluster; i < limit-1; i++)
  {
    (KM->cluster_size)[i] = (KM->cluster_size)[i+1];
    (KM->cluster_radius)[i] = (KM->cluster_radius)[i+1];
    (KM->cluster_start)[i] = (KM->cluster_start)[i+1];
    (KM->cluster_centroid)[i] = (KM->cluster_centroid)[i+1];
  }

  /* Decrement k. */
  KM->k = KM->k - 1;

  return;
}
