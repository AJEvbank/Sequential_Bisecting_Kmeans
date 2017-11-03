#ifndef stackR

#define stackR

#include "mainHeader.h"

struct stackRBase * initRStack()
{
	struct stackRBase * rtrn = (struct stackRBase *)malloc(sizeof(struct stackRBase));
	rtrn->firstNode = NULL;
	rtrn->stackDepth = 0;
	return rtrn;
}

void pushRNode(double * cornerPoint, double * widthsOfRegion, int dim, int numPoints, struct stackRBase * thisSB)
{
	int i;
	struct stackRNode * temp = (struct stackRNode *)malloc(sizeof(struct stackRNode));
	double * lowerCorner = (double *)malloc(sizeof(double) * dim);
  double * widths = (double *)malloc(sizeof(double) * dim);
	for (i = 0; i < dim; i++) { lowerCorner[i] = cornerPoint[i];
                              widths[i] = widthsOfRegion[i]; }
	temp->nextNode = thisSB->firstNode;
	temp->lowerCorner = lowerCorner;
	temp->widths = widths;
  temp->numPoints = numPoints;
  temp->dim = dim;
	thisSB->firstNode = temp;
	thisSB->stackDepth = thisSB->stackDepth + 1;
	return;
}

void Rpop(struct stackRBase * thisSB)
{
	struct stackRNode * temp = thisSB->firstNode;
	if (temp == NULL)
	{
		return;
	}
	else
	{
		thisSB->firstNode = temp->nextNode;
		thisSB->stackDepth = thisSB->stackDepth - 1;
		free(temp->lowerCorner);
    free(temp->widths);
		free(temp);
		return;
	}
}

int peekRDepth(struct stackRBase * thisSB)
{
	return (thisSB->stackDepth);
}

void clearRStack(struct stackRBase * thisSB)
{
	while (thisSB->firstNode != NULL)
	{
		Rpop(thisSB);
	}
	return;
}




#endif
