#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

int main(){

	struct BODY* cube=makeRectangularPrism(-.5f,-.5f,-.5f,1.0f,1.0f,1.0f);
	triangulateBody(cube);

	float lookAt[3]={0,0,0};
	float lookFrom[3]={1,1,1};

	struct DRAWSTYLE* drawStyle=defaultDrawstyle();
	drawBody(1000,1020,lookAt,lookFrom,cube,drawStyle);
	
	return 0;
}
