#include <stdio.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

void writeSTL_binary(struct BODY* body, char* *filename){
	FILE *f=fopen(*filename,"w");
	char header[80]={0};
	char attribute_byte_count[2]={0};
	int numTriangles = countFaces(body);
	fwrite(header,1,80,f);
	fwrite(&numTriangles,4,1,f);
	struct FACE* iter=body->face;
	while (iter!=NULL){	
		fwrite(iter->normal,4,3,f);
		for (int j=0; j<3; j++){
			float node[3]={iter->node_array[j]->x,iter->node_array[j]->y,iter->node_array[j]->z};
			fwrite(node,4,3,f);
		}
		fwrite(attribute_byte_count,1,2,f);
		iter=iter->next;
	}
	fclose(f);
	return;
}

void writeSTL_ASCII(struct BODY* body, char* *filename){
	FILE *f=fopen(*filename,"w");
	struct FACE* iter=body->face;
	fprintf(f,"solid \n");
	while (iter!=NULL){	
		fprintf(f,"facet normal %f %f %f\n",iter->normal[0],iter->normal[1],iter->normal[2]);
		fprintf(f,"\touter loop\n");
		for (int j=0; j<3; j++){
			fprintf(f,"\t\tvertex %f %f %f\n",iter->node_array[j]->x,iter->node_array[j]->y,iter->node_array[j]->z);
		}
		fprintf(f,"\tendloop\n");
		iter=iter->next;
	}
	fprintf(f,"endsolid \n");
	fclose(f);
	return;
}


