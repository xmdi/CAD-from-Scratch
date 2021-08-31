#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "out.h"
#include "geom.h"

// arc in the x-y plane, returns next index in nodes
int nodeArc(float nodes[][3], int start, float x0, float y0, float z0, float theta_0, float theta_1, float r, int n){
	float d_theta=(theta_1-theta_0)/((float) n);
	for (int i=0; i<=n; i++){
		float angle=theta_0+i*d_theta;
		nodes[start][0]=x0+(r)*cos(angle);
		nodes[start][1]=y0+(r)*sin(angle);
		nodes[start++][2]=z0;
	}
	return (start);
}

// close and extrude x-y curve in the z-direction, returns numTriangles
int extrudeClosedCurve(float nodes[][3], int triangles[][3], int numNodesCurve, float depth){
	int ct=0;
	for (int i=0; i<numNodesCurve; i++){
		nodes[numNodesCurve+i][0]=nodes[i][0];
		nodes[numNodesCurve+i][1]=nodes[i][1];
		nodes[numNodesCurve+i][2]=nodes[i][2]+depth;
		triangles[ct][0]=i;
		triangles[ct][1]=i+1;
		triangles[ct++][2]=i+numNodesCurve;
		triangles[ct][0]=i+numNodesCurve;
		triangles[ct][1]=i+1;
		triangles[ct++][2]=i+numNodesCurve+1;
	}
	triangles[ct-2][1]=0;
	triangles[ct-1][1]=0;
	triangles[ct-1][2]=numNodesCurve;
	return ct;
}

// make custom clip geometry
void makeClip(float t, float w, float l, float r, float g, int n){
	int numNodes=2*(4*n+4)+4;
	int numTriangles=2*(4*n+4)+8*n+8;
	float nodes[numNodes][3];
	int triangles[numTriangles][3];
	int c=0;
	int ct=0;

	c=nodeArc(nodes,c,0.0f,0.0f,0.0f,3.14159f/2.0f,3.0f*3.14159f/2.0f,(g+4*r)/2.0f,n);
	c=nodeArc(nodes,c,nodes[c-1][0]+l+r,nodes[c-1][1]+r,0.0f,3.0f*3.14159f/2.0f,3.0f*3.14159f,r,n);
	c=nodeArc(nodes,c,nodes[c-1][0]-l,nodes[c-1][1]+r+((float) g)/2.0f,0.0f,3.0f*3.14159f/2.0f,3.14159f/2.0f,r+((float)g)/2.0f,n);
	c=nodeArc(nodes,c,nodes[c-1][0]+l+r,nodes[c-1][1],0.0f,3.14159f,5.0f*3.14159f/2.0f,r,n);

	ct=extrudeClosedCurve(nodes,triangles,c,t);
	
	c=2*c;

	nodes[c][0]=l+r;nodes[c][1]=r+((float)g)/2.0f;nodes[c++][2]=0;
	nodes[c][0]=l+r;nodes[c][1]=-r-((float)g)/2.0f;nodes[c++][2]=0;
	nodes[c][0]=l+r;nodes[c][1]=r+((float)g)/2.0f;nodes[c++][2]=t;
	nodes[c][0]=l+r;nodes[c][1]=-r-((float)g)/2.0f;nodes[c++][2]=t;

	int k=4*n+4; // nodes around curve

	for (int j=0;j<=1;j++){
		for (int i=0; i<n; i++){
			triangles[ct][0]=j*k+i;
			triangles[ct][1]=j*k+i+1;
			triangles[ct++][2]=j*k+3*n+2-i;
			triangles[ct][0]=j*k+3*n+1-i;
			triangles[ct][1]=j*k+3*n+2-i;
			triangles[ct++][2]=j*k+i+1;
		}
		for (int i=0; i<n; i++){
			triangles[ct][0]=j*k+n+1+i;
			triangles[ct][1]=j*k+n+2+i;
			triangles[ct++][2]=2*k+1+2*j;
		}
		for (int i=0; i<n; i++){
			triangles[ct][0]=j*k+3*n+3+i;
			triangles[ct][1]=j*k+3*n+4+i;
			triangles[ct++][2]=2*k+2*j;
		}
	}


	triangles[ct][0]=0;triangles[ct][1]=3*n+2;triangles[ct++][2]=2*k;
	triangles[ct][0]=2*k;triangles[ct][1]=k-1;triangles[ct++][2]=0;

	triangles[ct][0]=n;triangles[ct][1]=n+1;triangles[ct++][2]=2*k+1;
	triangles[ct][0]=2*k+1;triangles[ct][1]=2*n+2;triangles[ct++][2]=n;

	triangles[ct][0]=k;triangles[ct][1]=k+3*n+2;triangles[ct++][2]=2*k+2;
	triangles[ct][0]=2*k+2;triangles[ct][1]=k+4*n+3;triangles[ct++][2]=k;

	triangles[ct][0]=k+n;triangles[ct][1]=k+n+1;triangles[ct++][2]=2*k+3;
	triangles[ct][0]=2*k+3;triangles[ct][1]=k+2*n+2;triangles[ct++][2]=k+n;

	float lookAt[3]={l/2.0f,0,t/2.0f};
	float lookFrom[3]={-1,-.5,5};
	char* filename="clip.stl";
	writeSTL_binary(numNodes,numTriangles,nodes,triangles,&filename);
	draw(800,800,lookAt,lookFrom,numNodes,numTriangles,nodes,triangles);
	
	return;
}

void makeNode(struct NODE* node, float x, float y, float z, struct NODE* next){
	node -> x = x;
	node -> y = y;
	node -> z = z;
	node -> next = next;
	return;
}

void makeFace(struct FACE* face, int numNodes, struct NODE* node_array[], struct FACE* next){
	for (int i=0; i<numNodes; i++){
		face -> node_array[i] = node_array[i];
	}
	face -> numNodes = numNodes;
	face -> next = next;
	return;
}

struct BODY* makeRectangularPrism(float x0, float y0, float z0, float l, float w, float h){
	struct BODY* prism = (struct BODY*)malloc(sizeof(struct BODY));

	struct NODE* node1 = (struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* node2 = (struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* node3 = (struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* node4 = (struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* node5 = (struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* node6 = (struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* node7 = (struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* node8 = (struct NODE*)malloc(sizeof(struct NODE));

	makeNode(node1,x0,y0,z0,node2);
	makeNode(node2,x0+l,y0,z0,node3);
	makeNode(node3,x0+l,y0+w,z0,node4);
	makeNode(node4,x0,y0+w,z0,node5);
	makeNode(node5,x0,y0,z0+h,node6);
	makeNode(node6,x0+l,y0,z0+h,node7);
	makeNode(node7,x0+l,y0+w,z0+h,node8);
	makeNode(node8,x0,y0+w,z0+h,NULL);

	struct FACE* face1 = (struct FACE*)malloc(sizeof(struct FACE*) + sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face2 = (struct FACE*)malloc(sizeof(struct FACE*) + sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face3 = (struct FACE*)malloc(sizeof(struct FACE*) + sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face4 = (struct FACE*)malloc(sizeof(struct FACE*) + sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face5 = (struct FACE*)malloc(sizeof(struct FACE*) + sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face6 = (struct FACE*)malloc(sizeof(struct FACE*) + sizeof(int) + 4 * sizeof(struct NODE*));
	
	struct NODE* node_array[4];

	node_array[0]=node1;node_array[1]=node2;node_array[2]=node3;node_array[3]=node4;
	makeFace(face1,4,node_array,face2);

	node_array[0]=node1;node_array[1]=node5;node_array[2]=node6;node_array[3]=node2;
	makeFace(face2,4,node_array,face3);
	
	node_array[0]=node2;node_array[1]=node6;node_array[2]=node7;node_array[3]=node3;
	makeFace(face3,4,node_array,face4);

	node_array[0]=node3;node_array[1]=node7;node_array[2]=node8;node_array[3]=node4;
	makeFace(face4,4,node_array,face5);

	node_array[0]=node4;node_array[1]=node8;node_array[2]=node5;node_array[3]=node1;
	makeFace(face5,4,node_array,face6);

	node_array[0]=node5;node_array[1]=node8;node_array[2]=node7;node_array[3]=node6;
	makeFace(face6,4,node_array,NULL);

	prism -> node = node1;
	prism -> face = face1;

	return prism;
}

void printBodyElements(struct BODY* body){
	int i=0;
	struct FACE* iter = body -> face;
	while(iter!=NULL){
		printf("FACE #%d\n",i++);
		for (int j=0; j<(iter->numNodes); j++){
			printf("\tNODE #%d (%f,%f,%f)\n",j,iter->node_array[j]->x,iter->node_array[j]->y,iter->node_array[j]->z);
		}
		iter = iter -> next;
	}
	return;
}

struct NODE* getClosestNode(struct BODY* body, float x0, float y0, float z0){
	struct NODE* closest = body -> node;
	struct NODE* iter = body -> node -> next;
	float dmin = pow(pow(closest->x-x0,2)+pow(closest->y-y0,2)+pow(closest->z-z0,2),0.5);
	float d;
	while(iter!=NULL){
		d = pow(pow(iter->x-x0,2)+pow(iter->y-y0,2)+pow(iter->z-z0,2),0.5);
		if (d<dmin){
			dmin=d;
			closest=iter;
		}
		iter=iter->next;
	}
	return closest;
}
