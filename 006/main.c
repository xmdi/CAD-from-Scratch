#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "draw.h"
#include "trig.h"
#include "geom.h"
#include "out.h"

int main(){

//	makeClip(0.5f,0.1f,1.0f,0.1f,0.025f,50);

	struct BODY* prism = makeRectangularPrism(0.0f,0.0f,0.0f,4.0f,5.0f,3.0f);

	printBodyElements(prism);

	struct NODE* FAVORITE_NODE=getClosestNode(prism,4.01f,0.01f,3.1f);

	printf("my favorite node is close to (4.01,0.01,3.1) and it's (%f,%f,%f)\n",FAVORITE_NODE->x,FAVORITE_NODE->y,FAVORITE_NODE->z);


	return 0;
}
