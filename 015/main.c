#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

int main(){

	float triangle1_pts[3][3]={{0,0,0},{3,0,0},{3,3,0}};
	float triangle1_norm[3];
	getNormal(triangle1_pts[0],triangle1_pts[1],triangle1_pts[2],triangle1_norm);
	struct FACE* triangle1=makePolygonFace(3,triangle1_norm,triangle1_pts);
	
	float triangle2_pts[3][3]={{0,2,-1},{0,1,1},{8,1,0.5f}};	
	float triangle2_norm[3];
	getNormal(triangle2_pts[0],triangle2_pts[1],triangle2_pts[2],triangle2_norm);
	struct FACE* triangle2=makePolygonFace(3,triangle2_norm,triangle2_pts);

	float triangle3_pts[3][3]={{2,1,0},{1,-2,0},{2,-2,0}};
	float triangle3_norm[3];
	getNormal(triangle3_pts[0],triangle3_pts[1],triangle3_pts[2],triangle3_norm);
	struct FACE* triangle3=makePolygonFace(3,triangle3_norm,triangle3_pts);
	
	float triangle4_pts[3][3]={{1,2,0},{4,2,0},{1,-1,0}};
	float triangle4_norm[3];
	getNormal(triangle4_pts[0],triangle4_pts[1],triangle4_pts[2],triangle4_norm);
	struct FACE* triangle4=makePolygonFace(3,triangle4_norm,triangle4_pts);
	
	int result;

	result=classifyTriangleIntersect(triangle1,triangle2);
	printf("Triangle1-Triangle2 Intersection Test Result: %d\n",result);
	if (result)
		triangleIntersectionPoints(triangle1,triangle2);
	
	result=classifyTriangleIntersect(triangle2,triangle3);
	printf("Triangle2-Triangle3 Intersection Test Result: %d\n",result);
	if (result)
		triangleIntersectionPoints(triangle2,triangle3);

	result=classifyTriangleIntersect(triangle1,triangle3);
	printf("Triangle1-Triangle3 Intersection Test Result: %d\n",result);
	if (result)
		triangleIntersectionPoints(triangle1,triangle3);
	
	result=classifyTriangleIntersect(triangle1,triangle4);
	printf("Triangle1-Triangle4 Intersection Test Result: %d\n",result);
	if (result)
		triangleIntersectionPoints(triangle1,triangle4);

	return 0;
}
