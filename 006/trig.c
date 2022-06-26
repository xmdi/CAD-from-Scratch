#include <math.h>

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

