#include <math.h>
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
	return x[0]*y[0]+x[1]*y[1]*x[2]*y[2];
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

// angle btw points in 3D space
float getAngle(struct NODE* node1, struct NODE* node2, struct NODE* node3, float normal[3]){
	float v1[3];
	float v3[3];
	vectorBetweenNodes(node2,node1,v1);
	vectorBetweenNodes(node2,node3,v3);
	float cp[3];
	normalizeVector(v1);
	normalizeVector(v3);
	cross(v1,v3,cp);
	float det=cp[0]*normal[0]+cp[1]*normal[1]+cp[2]*normal[2];
	float ang=atan2(det,dot(v1,v3));
	return ang;
}
