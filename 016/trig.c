#include <math.h>
#include <stdio.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"


void getNormal(float p1[3], float p2[3], float p3[3], float normal[3]){
	float v1[3]={p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]};
	float v2[3]={p3[0]-p1[0],p3[1]-p1[1],p3[2]-p1[2]};
	normal[0]=v1[1]*v2[2]-v1[2]*v2[1];
	normal[1]=v1[2]*v2[0]-v1[0]*v2[2];
	normal[2]=v1[0]*v2[1]-v1[1]*v2[0];
	return;
}

void normalizeVector(float vec[3]){
	float mag=pow(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2],0.5);
	vec[0]=vec[0]/mag;
	vec[1]=vec[1]/mag;
	vec[2]=vec[2]/mag;
	return;
}

float dot(float x[3], float y[3]){
	return x[0]*y[0]+x[1]*y[1]+x[2]*y[2];
}

void cross(float x[3], float y[3], float out[3]){
	out[0]=x[1]*y[2]-x[2]*y[1];
	out[1]=x[2]*y[0]-x[0]*y[2];
	out[2]=x[0]*y[1]-x[1]*y[0];
	return;
}

void vectorBetweenNodes(struct NODE* tail, struct NODE* head, float out[3]){
	out[0]=head->x-tail->x;
	out[1]=head->y-tail->y;
	out[2]=head->z-tail->z;
	return;
}

float norm(int type, float x[3]){
	if (type==2)
		return pow(dot(x,x),0.5);
	printf("OOPS! Bad type in norm(...).\n");
	return 0;
}

// angle between nodes in 3d space
float getNodeAngle(struct NODE* node1, struct NODE* node2, struct NODE* node3, float normal[3]){
	float v1[3];
	float v3[3];
	vectorBetweenNodes(node2,node1,v1);
	vectorBetweenNodes(node2,node3,v3);
	float cp[3];
	float tempNormal[3]={normal[0],normal[1],normal[2]};
	cross(v1,v3,cp);
	normalizeVector(tempNormal);
	float ang=atan2(dot(cp,tempNormal),dot(v1,v3));
	return ang;
}

// angle between points in 3d space
float getPointAngle(float a[3], float b[3], float c[3], float normal[3]){
	float v1[3]={b[0]-a[0],b[1]-a[1],b[2]-a[2]};
	float v3[3]={c[0]-a[0],c[1]-a[1],c[2]-a[2]};
	float cp[3];
	float tempNormal[3]={normal[0],normal[1],normal[2]};
	cross(v1,v3,cp);
	normalizeVector(tempNormal);
	float ang=atan2(dot(cp,tempNormal),dot(v1,v3));
	return ang;
}

// gives 3x3 determinant as follows for (A,B,C,D)
// |Dx-Ax Dy-Ay Dz-Az|
// |Dx-Bx Dy-By Dz-Bz|
// |Dx-Cx Dy-Cy Dz-Cz|
float magicDeterminant3(struct NODE* A, struct NODE* B, struct NODE* C, struct NODE* D){
	float out=0;

	out+=(D->x-A->x)*((D->y-B->y)*(D->z-C->z)-(D->z-B->z)*(D->y-C->y));
	out+=(D->y-A->y)*((D->z-B->z)*(D->x-C->x)-(D->x-B->x)*(D->z-C->z));
	out+=(D->z-A->z)*((D->x-B->x)*(D->y-C->y)-(D->y-B->y)*(D->x-C->x));
	
	return out;	
}
