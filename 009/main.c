#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

int main(){

	struct BODY* prism = makeRectangularPrism(0.0f,0.0f,0.0f,4.0f,5.0f,3.0f);
	triangulateBody(prism);
	char* file1="prism.stl";
	writeSTL_binary(prism,&file1);

	struct FACE* face=NULL;
	float normal[3]={0,0,1};
	float points[6][3]={{-1,-2,0},{-3,2,0},{2,3,0},{2,1,0},{5,1,0},{3,-2,0}};
	face=makePolygonFace(6,normal,points);
	struct BODY* hexagon = makeBodyFromFace(face);
	triangulateBody(hexagon);
	char* file2="hexagon.stl";
	writeSTL_ASCII(hexagon,&file2);

	return 0;
}
