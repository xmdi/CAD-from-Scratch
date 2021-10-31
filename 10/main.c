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

	char* file2="Hot_Air_Balloon2.stl";
	writeSTL_ASCII(balloon,&file2);

	struct BODY* balloon2=readSTL(&file2);
	
	float lookAt[3]={0,0,0};
	getBodyNodalCentroid(balloon2,lookAt);
	printf("Nodal centroid: (%f,%f,%f)\n",lookAt[0],lookAt[1],lookAt[2]);
	float lookFrom[3]={500,lookAt[1],lookAt[2]};
	drawBody(1000,1000,lookAt,lookFrom,balloon2);
	
	return 0;
}
