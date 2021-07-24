#include <stdio.h>

void getNormal(double p1[3], double p2[3], double p3[3], double normal[3]){
	double v1[3]={p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]};
	double v2[3]={p3[0]-p1[0],p3[1]-p1[1],p3[2]-p1[2]};
	normal[0]=v1[1]*v2[2]-v1[2]*v2[1];
	normal[1]=v1[2]*v2[0]-v1[0]*v2[2];
	normal[2]=v1[0]*v2[1]-v1[1]*v2[0];
	return;
}

void writeSTL_ASCII(int numNodes, int numTriangles, double nodes[][3], int triangles[][3], char* *filename){
	FILE *f=fopen(*filename,"w");
	fprintf(f,"solid \n");
	for (int i=0; i<numTriangles; i++){
		double normal[3];
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

int main(){
	double nodes[9][3]={{0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,0,1},{1,0,1},{1,1,1},{0,1,1},{.5,.5,1.5}};
	int triangles[14][3]={{0,1,5},{0,5,4},{1,2,6},{1,6,5},{2,3,7},{2,7,6},{3,0,4},{3,4,7},{4,5,8},{5,6,8},{6,7,8},{7,4,8},{0,3,2},{0,2,1}};
	char* filename="house.stl";
	writeSTL_ASCII(9,14,nodes,triangles,&filename);
	return 0;
}
