#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

int main(){

	char* file1="Hot_Air_Balloon.stl";
	struct BODY* balloon=readSTL(&file1);

	float lookAt[3]={0,0,0};
	getBodyNodalCentroid(balloon,lookAt);
	printf("Nodal centroid: (%f,%f,%f)\n",lookAt[0],lookAt[1],lookAt[2]);
	float lookFrom[3]={500,lookAt[1],lookAt[2]};

	struct DRAWSTYLE* drawStyle=defaultDrawstyle();
	drawBody(1000,1000,lookAt,lookFrom,balloon,drawStyle);
	
	return 0;
}
