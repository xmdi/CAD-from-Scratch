#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

int main(int argc, char *argv[]){

	if (argc!=2){
		printf("run with input number 1-8 to select a test case\n");
		return 1;
	}

	if (!strcmp(argv[1],"1")){

		printf("\n\n==========TEST CASE 1==========\n\n");

		int numPoints=9;
		float pointCloud[9][2]={{0,7},
								  {-5,5},
								  {5,5},
								  {-2,3},
								  {3,1},
								  {-4,-1},
								  {1,-2},
								  {-6,-4},
								  {5,-4}};
		
		float (*points)[2]=malloc(numPoints*2*sizeof(float));
		for (int i=0;i<numPoints;i++){
			points[i][0]=pointCloud[i][0];
			points[i][1]=pointCloud[i][1];
		}

		struct ARRAYLIST* constraintEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		constraintEdge->next=NULL;
		constraintEdge->numVals=2;
		constraintEdge->array[0]=4;
		constraintEdge->array[1]=6;

		delaunayTriangulate(points,constraintEdge,numPoints);
	}
	else if (!strcmp(argv[1],"2")){

		printf("\n\n==========TEST CASE 2==========\n\n");

		int numPoints=9;
		float pointCloud[9][2]={{0,7},
								  {-5,5},
								  {5,5},
								  {-2,3},
								  {3,1},
								  {-4,-1},
								  {1,-2},
								  {-6,-4},
								  {5,-4}};
		
		float (*points)[2]=malloc(numPoints*2*sizeof(float));
		for (int i=0;i<numPoints;i++){
			points[i][0]=pointCloud[i][0];
			points[i][1]=pointCloud[i][1];
		}

		struct ARRAYLIST* constraintEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		constraintEdge->next=NULL;
		constraintEdge->numVals=2;
		constraintEdge->array[0]=4;
		constraintEdge->array[1]=5;

		delaunayTriangulate(points,constraintEdge,numPoints);
	}
	else if (!strcmp(argv[1],"3")){
		printf("\n\n==========TEST CASE 3==========\n\n");

		int numPoints=9;
		float pointCloud[9][2]={{0,7},
								  {-5,5},
								  {5,5},
								  {-2,3},
								  {3,1},
								  {-4,-1},
								  {1,-2},
								  {-6,-4},
								  {5,-4}};
		
		float (*points)[2]=malloc(numPoints*2*sizeof(float));
		for (int i=0;i<numPoints;i++){
			points[i][0]=pointCloud[i][0];
			points[i][1]=pointCloud[i][1];
		}

		struct ARRAYLIST* constraintEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		constraintEdge->next=NULL;
		constraintEdge->numVals=2;
		constraintEdge->array[0]=6;
		constraintEdge->array[1]=0;

		delaunayTriangulate(points,constraintEdge,numPoints);
	}
	else if (!strcmp(argv[1],"4")){
		printf("\n\n==========TEST CASE 4==========\n\n");

		int numPoints=9;
		float pointCloud[9][2]={{0,7},
								  {-5,5},
								  {5,5},
								  {-2,3},
								  {3,1},
								  {-4,-1},
								  {1,-2},
								  {-6,-4},
								  {5,-4}};
		
		float (*points)[2]=malloc(numPoints*2*sizeof(float));
		for (int i=0;i<numPoints;i++){
			points[i][0]=pointCloud[i][0];
			points[i][1]=pointCloud[i][1];
		}

		struct ARRAYLIST* constraintEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		constraintEdge->next=NULL;
		constraintEdge->numVals=2;
		constraintEdge->array[0]=2;
		constraintEdge->array[1]=7;

		delaunayTriangulate(points,constraintEdge,numPoints);
	}
	else if (!strcmp(argv[1],"5")){
	
		printf("\n\n==========TEST CASE 5==========\n\n");

		int numPoints=9;
		float pointCloud[9][2]={{0,7},
								  {-5,5},
								  {5,5},
								  {-2,3},
								  {3,1},
								  {-4,-1},
								  {1,-2},
								  {-6,-4},
								  {5,-4}};
		
		float (*points)[2]=malloc(numPoints*2*sizeof(float));
		for (int i=0;i<numPoints;i++){
			points[i][0]=pointCloud[i][0];
			points[i][1]=pointCloud[i][1];
		}

		struct ARRAYLIST* constraintEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		constraintEdge->next=NULL;
		constraintEdge->numVals=2;
		constraintEdge->array[0]=1;
		constraintEdge->array[1]=8;

		delaunayTriangulate(points,constraintEdge,numPoints);
	}
	else if (!strcmp(argv[1],"6")){
	
		printf("\n\n==========TEST CASE 6==========\n\n");

		int numPoints=9;
		float pointCloud[9][2]={{0,7},
								  {-5,5},
								  {5,5},
								  {-2,3},
								  {3,1},
								  {-4,-1},
								  {1,-2},
								  {-6,-4},
								  {5,-4}};
		
		float (*points)[2]=malloc(numPoints*2*sizeof(float));
		for (int i=0;i<numPoints;i++){
			points[i][0]=pointCloud[i][0];
			points[i][1]=pointCloud[i][1];
		}

		struct ARRAYLIST* constraintEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		struct ARRAYLIST* constraintEdge2=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		constraintEdge->next=constraintEdge2;
		constraintEdge2->next=NULL;
		constraintEdge->numVals=2;
		constraintEdge2->numVals=2;
		constraintEdge->array[0]=4;
		constraintEdge->array[1]=5;
		constraintEdge2->array[0]=2;
		constraintEdge2->array[1]=3;

		delaunayTriangulate(points,constraintEdge,numPoints);
	}
	else if (!strcmp(argv[1],"7")){
	
		printf("\n\n==========TEST CASE 7==========\n\n");

		int numPoints=9;
		float pointCloud[9][2]={{0,7},
								  {-5,5},
								  {5,5},
								  {-2,3},
								  {3,1},
								  {-4,-1},
								  {1,-2},
								  {-6,-4},
								  {5,-4}};
		
		float (*points)[2]=malloc(numPoints*2*sizeof(float));
		for (int i=0;i<numPoints;i++){
			points[i][0]=pointCloud[i][0];
			points[i][1]=pointCloud[i][1];
		}

		struct ARRAYLIST* constraintEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		struct ARRAYLIST* constraintEdge2=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		constraintEdge->next=constraintEdge2;
		constraintEdge2->next=NULL;
		constraintEdge->numVals=2;
		constraintEdge2->numVals=2;
		constraintEdge->array[0]=1;
		constraintEdge->array[1]=2;
		constraintEdge2->array[0]=2;
		constraintEdge2->array[1]=5;

		delaunayTriangulate(points,constraintEdge,numPoints);
	}
	else if (!strcmp(argv[1],"8")){
	
		printf("\n\n==========TEST CASE 8==========\n\n");

		int numPoints=9;
		float pointCloud[9][2]={{0,7},
								  {-5,5},
								  {5,5},
								  {-2,3},
								  {3,1},
								  {-4,-1},
								  {1,-2},
								  {-6,-4},
								  {5,-4}};
		
		float (*points)[2]=malloc(numPoints*2*sizeof(float));
		for (int i=0;i<numPoints;i++){
			points[i][0]=pointCloud[i][0];
			points[i][1]=pointCloud[i][1];
		}

		struct ARRAYLIST* constraintEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		struct ARRAYLIST* constraintEdge2=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
		constraintEdge->next=constraintEdge2;
		constraintEdge2->next=NULL;
		constraintEdge->numVals=2;
		constraintEdge2->numVals=2;
		constraintEdge->array[0]=7;
		constraintEdge->array[1]=4;
		constraintEdge2->array[0]=4;
		constraintEdge2->array[1]=8;

		delaunayTriangulate(points,constraintEdge,numPoints);
	}
	return 0;
}
