#include <stdio.h>
#include "trig.h"

void writeSTL_binary(int numNodes, int numTriangles, float nodes[][3], int triangles[][3], char* *filename){
	FILE *f=fopen(*filename,"w");
	char header[80]={0};
	char attribute_byte_count[2]={0};
	fwrite(header,1,80,f);
	fwrite(&numTriangles,4,1,f);
	for (int i=0; i<numTriangles; i++){
		float normal[3];
		getNormal(nodes[triangles[i][0]],nodes[triangles[i][1]],nodes[triangles[i][2]],normal);
		fwrite(normal,4,3,f);
		for (int j=0; j<3; j++){
			fwrite(nodes[triangles[i][j]],4,3,f);
		}
		fwrite(attribute_byte_count,1,2,f);
	}
	return;
}

void writeSTL_ASCII(int numNodes, int numTriangles, float nodes[][3], int triangles[][3], char* *filename){
	FILE *f=fopen(*filename,"w");
	fprintf(f,"solid \n");
	for (int i=0; i<numTriangles; i++){
		float normal[3];
		getNormal(nodes[triangles[i][0]],nodes[triangles[i][1]],nodes[triangles[i][2]],normal);
		fprintf(f,"facet normal %f %f %f\n",normal[0],normal[1],normal[2]);
		fprintf(f,"\touter loop\n");
		for (int j=0; j<3; j++){
			fprintf(f,"\t\tvertex %f %f %f\n",nodes[triangles[i][j]][0],nodes[triangles[i][j]][1],nodes[triangles[i][j]][2]);
		}
		fprintf(f,"\tendloop\n");
	}
	fprintf(f,"endsolid \n");
	return;
}


