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
		printf("run with input number 1-3 to select union, intersection, or difference\n");
		return 1;
	}
/*plot3([1 1 1 1],[1 0 0 1],[1 1 0 1],'-r');
plot3([1.75 .75 .75 1.75],[.75 .75 .75 .75],[1.75 .75 1.75 1.75],'-k');
plot3([1 1],[.75 .75],[1 .75],'-g');

% second 2 faces intersecting
plot3([1 1 1 1],[1 0 0 1],[1 1 0 1],'-r');
plot3([.75 1.75 1.75 .75],[.75 .75 .75 .75],[.75 1.75 .75 .75],'-k');
plot3([1 1],[.75 .75],[1 .75],'-g');
*/

/*	float triangle1_pts[3][3]={{1,1,1},{1,0,1},{1,0,0}};
	float triangle1_norm[3];
	getNormal(triangle1_pts[0],triangle1_pts[1],triangle1_pts[2],triangle1_norm);
	struct FACE* triangle1=makePolygonFace(3,triangle1_norm,triangle1_pts);
	
	float triangle2_pts[3][3]={{1.75,.75,1.75},{.75,.75,.75},{.75,.75,1.75}};	
	float triangle2_norm[3];
	getNormal(triangle2_pts[0],triangle2_pts[1],triangle2_pts[2],triangle2_norm);
	struct FACE* triangle2=makePolygonFace(3,triangle2_norm,triangle2_pts);
	
	int result;

	result=classifyTriangleIntersect(triangle1,triangle2);
	printf("Triangle1-Triangle2 Intersection Test Result: %d\n",result);
	if (result)
		triangleIntersectionPoints(triangle1,triangle2);
	
*/

	struct BODY* prism1 = makeRectangularPrism(0.0,0.0,0.0,1.0,1.0,1.0);
	struct BODY* prism2 = makeRectangularPrism(0.75,0.75,0.75,1.0,1.0,1.0);

	triangulateBody(prism1);
	triangulateBody(prism2);

	struct BODY* result = evaluateCSG(1, prism1, prism2);

	if (!strcmp(argv[1],"1")){

		printf("\n\n========== TEST CASE 1 - UNION ==========\n\n");

	}
	else if (!strcmp(argv[1],"2")){

		printf("\n\n========== TEST CASE 2 - INTERSECTION ==========\n\n");

	}
	else if (!strcmp(argv[1],"3")){
		printf("\n\n========== TEST CASE 3 - DIFFERENCE ==========\n\n");

	}
	
	return 0;
}
