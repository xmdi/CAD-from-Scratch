#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

int main(){

	printf("\n\n==========TEST CASE 1==========\n\n");

	int numPointsA=9;
	float pointCloud2DA[9][2]={{0,7},
							  {-5,5},
							  {5,5},
							  {-2,3},
							  {3,1},
							  {-4,-1},
							  {1,-2},
							  {-6,-4},
							  {5,-4}};
	
	float (*pointsA)[2]=malloc(numPointsA*2*sizeof(float));
	for (int i=0;i<numPointsA;i++){
		pointsA[i][0]=pointCloud2DA[i][0];
		pointsA[i][1]=pointCloud2DA[i][1];
	}

	delaunayTriangulate(pointsA,numPointsA);
	
	printf("\n\n==========TEST CASE 2==========\n\n");

	int numPointsB=7;
	float pointCloud2DB[7][2]={{1,1},
							  {3,4},
							  {-2,3},
							  {-2,2},
							  {-1,-1},
							  {-2,-3},
							  {4,-2}};
	float (*pointsB)[2]=malloc(numPointsB*2*sizeof(float));
	for (int i=0;i<numPointsB;i++){
		pointsB[i][0]=pointCloud2DB[i][0];
		pointsB[i][1]=pointCloud2DB[i][1];
	}

	delaunayTriangulate(pointsB,numPointsB);

	return 0;
}
