#include <math.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

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
	//writeSTL_binary(numNodes,numTriangles,nodes,triangles,&filename);
	//draw(800,800,lookAt,lookFrom,numNodes,numTriangles,nodes,triangles);
	
	return;
}

void makeNode(struct NODE* node, float x, float y, float z, struct NODE* next){
	node -> x = x;
	node -> y = y;
	node -> z = z;
	node -> next = next;
	return;
}

// episode 18
void addNode(struct BODY* body, struct NODE* node){ // might be useful to keep track of lastNode for every body
	struct NODE* iter=body->node;
	while (iter->next!=NULL){
		iter=iter->next;
	}
	iter->next=node;
	return;
}
// episode 18

void makeFace(struct FACE* face, int numNodes, float normal[3], struct NODE* node_array[], struct FACE* next){

	float xmin=node_array[0]->x;
	float xmax=node_array[0]->x;
	float ymin=node_array[0]->y;
	float ymax=node_array[0]->y;
	float zmin=node_array[0]->z;
	float zmax=node_array[0]->z;

	for (int i=0; i<numNodes; i++){ // can start this at 1 btw
		face -> node_array[i] = node_array[i];
		if (node_array[i]->x<xmin)
			xmin=node_array[i]->x;
		else if (node_array[i]->x>xmax)
			xmax=node_array[i]->x;
		if (node_array[i]->y<ymin)
			ymin=node_array[i]->y;
		else if (node_array[i]->y>ymax)
			ymax=node_array[i]->y;
		if (node_array[i]->z<zmin)
			zmin=node_array[i]->z;
		else if (node_array[i]->z>zmax)
			zmax=node_array[i]->z;
	}
	
	for (int i=0; i<3; i++){
		face -> normal[i] = normal[i];
	}
	
	face -> numNodes = numNodes;
	face -> next = next;
	face -> constraint_edges = NULL;

	face -> xmin = xmin;
	face -> xmax = xmax;
	face -> ymin = ymin;
	face -> ymax = ymax;
	face -> zmin = zmin;
	face -> zmax = zmax;

	return;
}

// episode 18
void refreshFaceExtremes(struct FACE* face){

	float xmin=face->node_array[0]->x;
	float xmax=face->node_array[0]->x;
	float ymin=face->node_array[0]->y;
	float ymax=face->node_array[0]->y;
	float zmin=face->node_array[0]->z;
	float zmax=face->node_array[0]->z;

	for (int i=0; i<face->numNodes; i++){ // can start this at 1 btw
		if (face->node_array[i]->x<xmin)
			xmin=face->node_array[i]->x;
		else if (face->node_array[i]->x>xmax)
			xmax=face->node_array[i]->x;
		if (face->node_array[i]->y<ymin)
			ymin=face->node_array[i]->y;
		else if (face->node_array[i]->y>ymax)
			ymax=face->node_array[i]->y;
		if (face->node_array[i]->z<zmin)
			zmin=face->node_array[i]->z;
		else if (face->node_array[i]->z>zmax)
			zmax=face->node_array[i]->z;
	}
	
	face->xmax=xmax;
	face->xmin=xmin;
	face->ymax=ymax;
	face->ymin=ymin;
	face->zmax=zmax;
	face->zmin=zmin;

	return;

}
// episode 18

// episode 18
void addFace(struct BODY* body, struct FACE* face){ 
	struct FACE* iter=body->face;
	while (iter->next!=NULL){
		iter=iter->next;
	}
	iter->next=face;
	return;
}
// episode 18

// episode 18
struct FACE* deepCopyFace(struct FACE* faceA){
	struct FACE* faceB=(struct FACE*)malloc(sizeof(struct FACE*)+sizeof(float)*9+sizeof(int)+sizeof(struct FLOATARRAYLIST*)+faceA->numNodes*sizeof(struct NODE*));
	makeFace(faceB,faceA->numNodes,faceA->normal,faceA->node_array,faceA->next);
	faceB->constraint_edges=faceA->constraint_edges;
	return faceB;
}
// episode 18

// episode 18
void ensureProperTriangleNodeOrder(struct FACE* face){
	float v1[3], v2[3], v3[3];
	vectorBetweenNodes(face->node_array[0],face->node_array[1],v1);
	vectorBetweenNodes(face->node_array[0],face->node_array[2],v2);
	cross(v1,v2,v3);
	if (dot(v3,face->normal)>=0){
		return;
	}
	else {
		struct NODE* tempNode=face->node_array[2];
		face->node_array[2]=face->node_array[1];
		face->node_array[1]=tempNode;
		return;
	}
}
// episode 18

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

	struct FACE* face1 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + sizeof(struct FLOATARRAYLIST*) + 4 * sizeof(struct NODE*));
	struct FACE* face2 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + sizeof(struct FLOATARRAYLIST*) + 4 * sizeof(struct NODE*));
	struct FACE* face3 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + sizeof(struct FLOATARRAYLIST*) + 4 * sizeof(struct NODE*));
	struct FACE* face4 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + sizeof(struct FLOATARRAYLIST*) + 4 * sizeof(struct NODE*));
	struct FACE* face5 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + sizeof(struct FLOATARRAYLIST*) + 4 * sizeof(struct NODE*));
	struct FACE* face6 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + sizeof(struct FLOATARRAYLIST*) + 4 * sizeof(struct NODE*));
	
	struct NODE* node_array[4];
	float normal[3];

	node_array[0]=node1;node_array[1]=node2;node_array[2]=node3;node_array[3]=node4;
	normal[0]=0;normal[1]=0;normal[2]=-1;
	makeFace(face1,4,normal,node_array,face2);

	node_array[0]=node1;node_array[1]=node5;node_array[2]=node6;node_array[3]=node2;
	normal[0]=0;normal[1]=-1;normal[2]=0;
	makeFace(face2,4,normal,node_array,face3);
	
	node_array[0]=node2;node_array[1]=node6;node_array[2]=node7;node_array[3]=node3;
	normal[0]=1;normal[1]=0;normal[2]=0;
	makeFace(face3,4,normal,node_array,face4);

	node_array[0]=node3;node_array[1]=node7;node_array[2]=node8;node_array[3]=node4;
	normal[0]=0;normal[1]=1;normal[2]=0;
	makeFace(face4,4,normal,node_array,face5);

	node_array[0]=node4;node_array[1]=node8;node_array[2]=node5;node_array[3]=node1;
	normal[0]=-1;normal[1]=0;normal[2]=0;
	makeFace(face5,4,normal,node_array,face6);

	node_array[0]=node5;node_array[1]=node8;node_array[2]=node7;node_array[3]=node6;
	normal[0]=0;normal[1]=0;normal[2]=1;
	makeFace(face6,4,normal,node_array,NULL);

	prism -> node = node1;
	prism -> face = face1;

	return prism;
}

void printBodyElements(struct BODY* body){
	int i=0;
	printf("\n============\n");
	struct FACE* iter = body -> face;
	while(iter!=NULL){
		printf("FACE #%d\n",i++);
		printf("NORMAL: (%f,%f,%f)\n",iter->normal[0],iter->normal[1],iter->normal[2]);
		for (int j=0; j<(iter->numNodes); j++){
			printf("\tNODE #%d (%f,%f,%f)\n",j,iter->node_array[j]->x,iter->node_array[j]->y,iter->node_array[j]->z);
		}
		iter = iter -> next;
	}
	printf("\n============\n");
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

void removeNodeFromFace(struct FACE* face, struct NODE* node){
	for (int i=0;i<(face->numNodes);i++){
		if (face->node_array[i]==node){
			for (int j=i;j<(face->numNodes-1);j++){
				face->node_array[j]=face->node_array[j+1];
			}
			face->node_array[--(face->numNodes)]=NULL;
			return;
		}
	}
	return;
}

void replaceFaceFromBody(struct BODY* body, struct FACE* face, struct FACE* new_faces){
	struct FACE* last=new_faces;
	while(last->next!=NULL){
		last=last->next;
	}
	struct FACE* iter=body->face;
	struct FACE* prev=iter;
	if (iter==face){
		body->face=new_faces;
		last->next=face->next;
		free(face);
		return;
	}
	while(iter!=NULL){ 
		if(iter==face){
			prev->next=new_faces;
			last->next=face->next;
			free(face);		
			return;
		}
		prev=iter;
		iter=iter->next;
	}
	return;
}

void triangulateFace(struct BODY* body, struct FACE* face){
	struct FACE* target_face=(struct FACE*)malloc(sizeof(struct FACE*)+sizeof(int)+9*sizeof(float)+sizeof(struct FLOATARRAYLIST*)+3*sizeof(struct NODE*));
	int i=0;
	while (face->numNodes>3){
		struct NODE* A = face->node_array[i%face->numNodes];
		struct NODE* B = face->node_array[(i+1)%face->numNodes];
		struct NODE* C = face->node_array[(i+2)%face->numNodes];
		float angle = getNodeAngle(A,B,C,face->normal);
		if (angle>0){
			int numNodesOutside=0;
			for (int j=0;j<(face->numNodes);j++){
				if (j<i || j>(i+2)){
					struct NODE* P = face->node_array[j];
					
					float AB[3], BC[3], CA[3];
					float AP[3], BP[3], CP[3];
					float N1[3], N2[3], N3[3];
					float S1,S2,S3,S4,S5,S6;

					vectorBetweenNodes(A,B,AB);
					vectorBetweenNodes(B,C,BC);
					vectorBetweenNodes(C,A,CA);
					vectorBetweenNodes(A,P,AP);
					vectorBetweenNodes(B,P,BP);
					vectorBetweenNodes(C,P,CP);
					
					cross(AB,face->normal,N1);
					cross(BC,face->normal,N2);
					cross(CA,face->normal,N3);

					S1=dot(AP,N1);
					S2=dot(BP,N1);
					S3=dot(BP,N2);
					S4=dot(CP,N2);
					S5=dot(CP,N3);
					S6=dot(AP,N3);

					if ((S1>0 && S2>0 && S3>0 && S4>0 && S5>0 && S6>0) || (S1<0 && S2<0 && S3<0 && S4<0 && S5<0 && S6<0)){
						//we are inside triangle
						i++;
						i=i%(face->numNodes);
						break;
					}
					else{ //we are outside of triangle
						numNodesOutside++;
						if (numNodesOutside==(face->numNodes-3)){ //all points outside of triangle
							removeNodeFromFace(face,B);
							struct FACE* iter=target_face;
							while (iter->numNodes==3){
								iter=iter->next;
							}
							struct FACE* next_face=(struct FACE*)malloc(sizeof(struct FACE*)+sizeof(int)+9*sizeof(float)+sizeof(struct FLOATARRAYLIST*)+3*sizeof(struct NODE*));
							struct NODE* node_array[3];
							node_array[0]=A;node_array[1]=B;node_array[2]=C;
							makeFace(iter,3,face->normal,node_array,next_face);
							i++;
							i=i%(face->numNodes);
						}
					}
				}
			}
		}
		i++;
		i=i%(face->numNodes);
	}
	// 3 nodes left
	struct NODE* A = face->node_array[0];
	struct NODE* B = face->node_array[1];
	struct NODE* C = face->node_array[2];
	removeNodeFromFace(face,A);
	removeNodeFromFace(face,B);
	removeNodeFromFace(face,C);
	struct FACE* iter=target_face;
	while(iter->numNodes==3){
		iter=iter->next;
	}
	struct NODE* node_array[3];
	node_array[0]=A;node_array[1]=B;node_array[2]=C;
	makeFace(iter,3,face->normal,node_array,NULL);

	replaceFaceFromBody(body,face,target_face);
					
	return;
}

void triangulateBody(struct BODY* body){
	struct FACE* iter=body->face;
	struct FACE* prev=iter;
	while (iter!=NULL){
		prev=iter;
		iter=iter->next;
		if (prev->numNodes>3){
			triangulateFace(body,prev);
		}
	}
	return;	
}

struct FACE* makePolygonFace(int numPoints, float normal[3], float points[][3]){

	struct NODE* node_array[numPoints];
	struct NODE* node0 = (struct NODE*)malloc(sizeof(struct NODE));

	for (int i=1; i<numPoints; i++){
		struct NODE* node1 = (struct NODE*)malloc(sizeof(struct NODE));
		makeNode(node0,points[i-1][0],points[i-1][1],points[i-1][2],node1);
		node_array[i-1]=node0;
		node0=node1;
	}
	makeNode(node0,points[numPoints-1][0],points[numPoints-1][1],points[numPoints-1][2],NULL);
	node_array[numPoints-1]=node0;
	
	struct FACE* face = (struct FACE*)malloc(sizeof(struct FACE*)+sizeof(int)+9*sizeof(float)+sizeof(struct FLOATARRAYLIST*)+numPoints*sizeof(struct NODE*));
	
	makeFace(face,numPoints,normal,node_array,NULL);

	return face;
}

int countNodes(struct BODY* body){
	struct NODE* iter=body->node;
	int num=0;
	while (iter!=NULL){
		num++;
		iter=iter->next;
	}
	return num;
}

int countFaces(struct BODY* body){
	struct FACE* iter=body->face;
	int num=0;
	while (iter!=NULL){
		num++;
		iter=iter->next;
	}
	return num;
}

int getFaceNumber(struct BODY* body, struct FACE* face){ // starting at 0
	struct FACE* iter=body->face;
	int num=0;
	while (iter!=NULL){
		if (iter==face)
			return num;
		num++;
		iter=iter->next;
	}
	return -1;
}

struct BODY* makeBodyFromFace(struct FACE* face){
	struct BODY* body = (struct BODY*)malloc(sizeof(struct BODY));
	body->face=face;
	body->node=face->node_array[0];
	return body;
}

void getBodyNodalCentroid(struct BODY* body, float centroid[3]){
	struct NODE* node=body->node;
	float num = (float) countNodes(body);
	while (node!=NULL){
		centroid[0]+=node->x/num;
		centroid[1]+=node->y/num;
		centroid[2]+=node->z/num;
		node=node->next;
	}
	return;
}

// rotates nodes about a given (face) (num) times, preserving the order
void rotateFaceNodes(struct FACE* face, int num){
	struct NODE* temp;
	for (int j=0;j<num;j++){
		temp=face->node_array[face->numNodes-1];
		for (int i=face->numNodes-1;i>=1;i--){
			face->node_array[i]=face->node_array[i-1];
		}
		face->node_array[0]=temp;
	}
	return;
}

// swaps nodes on a face
void swapFaceNodes(struct FACE* face, int num1, int num2){
	struct NODE* temp=face->node_array[num1];
	face->node_array[num1]=face->node_array[num2];
	face->node_array[num2]=temp;
	// also need to flip normal
	face->normal[0]=-face->normal[0];
	face->normal[1]=-face->normal[1];
	face->normal[2]=-face->normal[2];
	return;
}

// determine if edge a1-b1 intersects edge a2-b2. 
// if so, and mode==true put intersection coordinate in out
bool edgeEdgeIntersection(bool mode, struct NODE* a1, struct NODE* b1, struct NODE* a2, struct NODE* b2, float out[3]){	

	float cp1[3],cp2[3];
	
	float a1b1[3]={b1->x-a1->x,b1->y-a1->y,b1->z-a1->z};
	float a2b2[3]={b2->x-a2->x,b2->y-a2->y,b2->z-a2->z};

	float b1a2[3]={a2->x-b1->x,a2->y-b1->y,a2->z-b1->z};
	float b1b2[3]={b2->x-b1->x,b2->y-b1->y,b2->z-b1->z};

	cross(a1b1,b1a2,cp1);
	cross(a1b1,b1b2,cp2);
	if (dot(cp1,cp2)>0) // orientation test proves no intersection occurs
		return 0;

	float b2a1[3]={a1->x-b2->x,a1->y-b2->y,a1->z-b2->z};
	float b2b1[3]={b1->x-b2->x,b1->y-b2->y,b1->z-b2->z};

	cross(a2b2,b2a1,cp1);
	cross(a2b2,b2b1,cp2);
	if (dot(cp1,cp2)>0) // orientation test proves no intersection occurs
		return 0;

	if (!mode)
		return 1;

	float dxy=((b2->x-a2->x)*(b1->y-a1->y)-(b1->x-a1->x)*(b2->y-a2->y));
	float dxz=((b2->x-a2->x)*(b1->z-a1->z)-(b1->x-a1->x)*(b2->z-a2->z));
	float dzy=((b2->z-a2->z)*(b1->y-a1->y)-(b1->z-a1->z)*(b2->y-a2->y));
	float t;

	if ((fabsf(dxy)>fabsf(dxz))&&(fabsf(dxy)>fabsf(dzy)))
		t=((b2->x-a2->x)*(a2->y-a1->y)+(a1->x-a2->x)*(b2->y-a2->y))/dxy;
	else if ((fabsf(dxz)>fabsf(dxy))&&(fabsf(dxz)>fabsf(dzy)))
		t=((b2->x-a2->x)*(a2->z-a1->z)+(a1->x-a2->x)*(b2->z-a2->z))/dxz;
	else
		t=((b2->z-a2->z)*(a2->y-a1->y)+(a1->z-a2->z)*(b2->y-a2->y))/dzy;

	// parametric point of intersection
	out[0]=a1->x+t*a1b1[0];
	out[1]=a1->y+t*a1b1[1];
	out[2]=a1->z+t*a1b1[2];
	return 1;

}

// determine if edge a1-b1 intersects line thru P0 in direction N3.
// if so, put intersection coordinate in out
bool edgeLineIntersection(struct NODE* a1, struct NODE* b1, float N3[3], float P0[3], float out[3]){	

	// we can more efficiently determine if intersection occurs
	// currently, we are testing if 0<=t<=1
	
	float dxy=(N3[0]*(b1->y-a1->y)-N3[1]*(b1->x-a1->x));
	float dxz=(N3[0]*(b1->z-a1->z)-N3[2]*(b1->x-a1->x));
	float dzy=(N3[2]*(b1->y-a1->y)-N3[1]*(b1->z-a1->z));
	float t;

	if ((fabsf(dxy)>fabsf(dxz))&&(fabsf(dxy)>fabsf(dzy)))
		t=(N3[0]*(P0[1]-a1->y)+N3[1]*(a1->x-P0[0]))/dxy;
	else if ((fabsf(dxz)>fabsf(dxy))&&(fabsf(dxz)>fabsf(dzy)))
		t=(N3[0]*(P0[2]-a1->z)+N3[2]*(a1->x-P0[0]))/dxz;
	else
		t=(N3[2]*(P0[1]-a1->y)+N3[1]*(a1->z-P0[2]))/dzy;

	if ((t<0)||(t>1))
		return 0;

	// parametric point of intersection
	out[0]=a1->x+t*(b1->x-a1->x);
	out[1]=a1->y+t*(b1->y-a1->y);
	out[2]=a1->z+t*(b1->z-a1->z);
	
	return 1;

}

// determine if node lies within a coplanar triangle
bool coplanarNodeWithinTriangle(struct NODE* node, struct FACE* face){
					
	float AB[3], BC[3], CA[3];
	float AP[3], BP[3], CP[3];
	float N1[3], N2[3], N3[3];
	float S1,S2,S3;

	vectorBetweenNodes(face->node_array[0],face->node_array[1],AB);
	vectorBetweenNodes(face->node_array[1],face->node_array[2],BC);
	vectorBetweenNodes(face->node_array[2],face->node_array[0],CA);

	vectorBetweenNodes(face->node_array[0],node,AP);
	vectorBetweenNodes(face->node_array[1],node,BP);
	vectorBetweenNodes(face->node_array[2],node,CP);
					
	cross(AB,face->normal,N1);
	cross(BC,face->normal,N2);
	cross(CA,face->normal,N3);

	S1=dot(AP,N1);
	S2=dot(BP,N2);
	S3=dot(CP,N3);

	if ((S1>0&&S2>0&&S3>0)||(S1<0&&S2<0&&S3<0))
		//we are inside triangle
		return 1;	
	else
		return 0;

}

// reorder an Nx3 point array in order of ascending atan2 about center
void reorderPoints(int numPoints, float points[][3]){

	if (numPoints==4){ // if quadrilateral, easy peasy
		float cp1[3], cp2[3], cp3[3], cp4[3];
		float ac[3]={points[2][0]-points[0][0], // diag 1
					   points[2][1]-points[0][1],
					   points[2][2]-points[0][2]};
		float bd[3]={points[3][0]-points[1][0], // diag 2
					   points[3][1]-points[1][1],
					   points[3][2]-points[1][2]};
		float cd[3]={points[3][0]-points[2][0],
					   points[3][1]-points[2][1],
					   points[3][2]-points[2][2]};
		float cb[3]={points[1][0]-points[2][0],
					   points[1][1]-points[2][1],
					   points[1][2]-points[2][2]};
		float dc[3]={points[2][0]-points[3][0],
					   points[2][1]-points[3][1],
					   points[2][2]-points[3][2]};
		float da[3]={points[0][0]-points[3][0],
					   points[0][1]-points[3][1],
					   points[0][2]-points[3][2]};
		cross(ac,cd,cp1);
		cross(ac,cb,cp2);
		cross(bd,dc,cp3);
		cross(bd,da,cp4);
		if ((dot(cp1,cp2)<0)&&(dot(cp3,cp4)<0))
			return; // diagonals cross, nothing to do
		else { // diagonals don't cross, swap vert 1 & 4
			float tempF[3]={points[0][0],points[0][1],points[0][2]};
			points[0][0]=points[3][0];points[0][1]=points[3][1];points[0][2]=points[3][2];
			points[3][0]=tempF[0];points[3][1]=tempF[1];points[3][2]=tempF[2];
			return;
		}
	}

	// if pentagon or hexagon, slightly harder

	float center[3]={0};
	for (int i=0; i<numPoints; i++){
		center[0]+=points[i][0];	
		center[1]+=points[i][1];	
		center[2]+=points[i][2];	
	}
	center[0]=center[0]/numPoints;
	center[1]=center[1]/numPoints;
	center[2]=center[2]/numPoints;
	// division and atan2 not efficient, but it's OK for now

	float normal[3];
	getNormal(points[0],points[1],points[2],normal);

	float angle[numPoints];
	for (int i=0; i<numPoints; i++){
		angle[i]=getPointAngle(center,points[0],points[i],normal);
	}

	// insertion sort by angle
	float key;
	for(int i=1; i<numPoints; i++){
		key=angle[i];
		float tempF[3]={points[i][0],points[i][1],points[i][2]};
		int j=i-1;
	    while(j>=0&&(angle[j]>key)){
			angle[j+1]=angle[j];
			points[j+1][0]=points[j][0];points[j+1][1]=points[j][1];points[j+1][2]=points[j][2];
			j--;
		}
		angle[j+1]=key;
		points[j+1][0]=tempF[0];points[j+1][1]=tempF[1];points[j+1][2]=tempF[2];
	}

	return;
}

// classify the intersection between triangular faces
// return -1 = error
// return 0 = no intersect
// return 1 = intersect, k-i-l-j 
// return 2 = intersect, k-i-j-l 
// return 3 = intersect, i-k-l-j 
// return 4 = intersect, i-k-j-l
// i-j is the intersection interval for face1
// k-l is the intersection interval for face2
// return 5 = coplanar
int classifyTriangleIntersect(struct FACE* face1, struct FACE* face2){

	float tolerance=0.0001; // tolerance for coplanar classification

	// check if faces are triangles
	if ((face1->numNodes!=3)||(face2->numNodes!=3)){
		printf("OOPS! Nontriangular face in classifyTriangleIntersect(...).\n");
		return -1;
	}

	// check if bounding boxes overlap
	if ((face1->xmin>face2->xmax)||
		(face1->ymin>face2->ymax)||
		(face1->zmin>face2->zmax)||
		(face2->xmin>face1->xmax)||
		(face2->ymin>face1->ymax)||
		(face2->zmin>face1->zmax)){
		return 0; // no bounding box overlap, no intersection
	}

//	printf("inside classify: faceA node addresses: %p %p %p\n",&face1->node_array[0],&face1->node_array[1],&face1->node_array[2]);
//	printf("inside classift: faceB node addresses: %p %p %p\n",&face2->node_array[0],&face2->node_array[1],&face2->node_array[2]);
	
	// check signed distance between face1 plane and face2 nodes
	float Y1=magicDeterminant3(face1->node_array[0],face1->node_array[1],
								face1->node_array[2],face2->node_array[0]);	
	float Y2=magicDeterminant3(face1->node_array[0],face1->node_array[1],
								face1->node_array[2],face2->node_array[1]);	
	float Y3=magicDeterminant3(face1->node_array[0],face1->node_array[1],
								face1->node_array[2],face2->node_array[2]);	

	if (((Y1>0)&&(Y2>0)&&(Y3>0))||((Y1<0)&&(Y2<0)&&(Y3<0))){
		return 0; // no intersection is possible
	}
	else if((fabsf(Y1)<tolerance)&&(fabsf(Y2)<tolerance)&&(fabsf(Y3)<tolerance)){
		// coplanar
		
		float dummy[3];

		// loop over edges of face1 to see if any cross face2 edges
		for (int p=0;p<3;p++) // loop thru face2
			for (int q=0;q<3;q++) // loop thru face2
				if (edgeEdgeIntersection(0,face1->node_array[p],face1->node_array[(p+1)%3],face2->node_array[q],face2->node_array[(q+1)%3],dummy))
					return 5;
		
		// check if a single node of face1 is inside face2
		 if(coplanarNodeWithinTriangle(face1->node_array[0],face2))
		 	 return 5;

		// check if a single node of face2 is inside face1
		 if(coplanarNodeWithinTriangle(face2->node_array[0],face1))
		 	 return 5;

		return 0; // no intersection of coplanar triangles	

	}

	// check signed distance between face2 plane and face1 nodes
	float Y4=magicDeterminant3(face2->node_array[0],face2->node_array[1],
								face2->node_array[2],face1->node_array[0]);	
	float Y5=magicDeterminant3(face2->node_array[0],face2->node_array[1],
								face2->node_array[2],face1->node_array[1]);	
	float Y6=magicDeterminant3(face2->node_array[0],face2->node_array[1],
								face2->node_array[2],face1->node_array[2]);	

	if (((Y4>0)&&(Y5>0)&&(Y6>0))||((Y4<0)&&(Y5<0)&&(Y6<0))){
		return 0; // no intersection is possible
	}

/*	// this will never evaluate true, but I leave it here for completeness
	else if((fabsf(Y4)<tolerance)&&(fabsf(Y5)<tolerance)&&(fabsf(Y6)<tolerance)){
		return 5; // coplanar, handle this later**
	}
*/

	// rotate face nodes until the 0th node is the only node 
	// on that side of the other triangle's plane
	if (((Y5<0)&&(Y4>0)&&(Y6>0))||((Y5>0)&&(Y4<0)&&(Y6<0))){
		rotateFaceNodes(face1,2);
		float Y=magicDeterminant3(face2->node_array[0],face2->node_array[1],
								face2->node_array[2],face1->node_array[0]);
		if (Y<0){ // assure new node0 on face1 in positive subspace of face2
			swapFaceNodes(face2,1,2);
		}
	}
	else if (((Y6<0)&&(Y4>0)&&(Y5>0))||((Y6>0)&&(Y4<0)&&(Y5<0))){
		rotateFaceNodes(face1,1);
		float Y=magicDeterminant3(face2->node_array[0],face2->node_array[1],
								face2->node_array[2],face1->node_array[0]);
		if (Y<0){ // assure new node0 on face1 in positive subspace of face2
			swapFaceNodes(face2,1,2);
		}
	}
	if (((Y2<0)&&(Y1>0)&&(Y3>0))||((Y2>0)&&(Y1<0)&&(Y3<0))){
		rotateFaceNodes(face2,2);
		float Y=magicDeterminant3(face1->node_array[0],face1->node_array[1],
								face1->node_array[2],face2->node_array[0]);
		if (Y<0){ // assure new node0 on face2 in positive subspace of face1
			swapFaceNodes(face1,1,2);
		}
	}
	else if (((Y3<0)&&(Y1>0)&&(Y2>0))||((Y3>0)&&(Y1<0)&&(Y2<0))){
		rotateFaceNodes(face2,1);
		float Y=magicDeterminant3(face1->node_array[0],face1->node_array[1],
								face1->node_array[2],face2->node_array[0]);
		if (Y<0){ // assure new node0 on face2 in positive subspace of face1
			swapFaceNodes(face1,1,2);
		}
	}

	float Y7=magicDeterminant3(face1->node_array[0],face1->node_array[1],
								face2->node_array[0],face2->node_array[1]);	
	float Y8=magicDeterminant3(face1->node_array[0],face1->node_array[2],
								face2->node_array[2],face2->node_array[0]);	
	float Y9=magicDeterminant3(face1->node_array[0],face1->node_array[2],
								face2->node_array[1],face2->node_array[0]);	
	float Y10=magicDeterminant3(face1->node_array[0],face1->node_array[1],
								face2->node_array[2],face2->node_array[0]);	

	// check if screw along a1-b1 turns in direction b2-a2
	// and if screw along a1-c1 turns in direction a2-c2		
	if ((Y7<0)&&(Y8<0)){ // is i<=l and k<=j, aka do with have interval overlap
		if ((Y9>0)&&(Y10>0)) //k-i-l-j                                                         <<<< EPISODE 18, removed = parts of <=
			return 1;
		else if ((Y9>0)&&(Y10<0)) //k-i-j-l
			return 2;
		else if ((Y9<0)&&(Y10>0)) //i-k-l-j
			return 3;
		else if ((Y9<0)&&(Y10<0)) //i-k-j-l
			return 4;
	}

	return 0; // no intersection interval overlap
}

void triangleIntersectionPoints(struct FACE* face1, struct FACE* face2){
		
	int mode=classifyTriangleIntersect(face1,face2);

	//printf("MODE: %d\n\n\n",mode);
	
	if ((mode>0)&&(mode<5)){ // non-coplanar intersection detected

		float N1[3]={face1->normal[0],face1->normal[1],face1->normal[2]};
		float N2[3]={face2->normal[0],face2->normal[1],face2->normal[2]};
		float N3[3];
		cross(N1,N2,N3);
		normalizeVector(N1);
		normalizeVector(N2);
		normalizeVector(N3);
		float P1[3]={face1->node_array[0]->x,face1->node_array[0]->y,face1->node_array[0]->z};
		float P2[3]={face2->node_array[0]->x,face2->node_array[0]->y,face2->node_array[0]->z};
		float e2=pow(N1[0]-dot(N1,N2)*N2[0],2)+pow(N1[1]-dot(N1,N2)*N2[1],2)+pow(N1[2]-dot(N1,N2)*N2[2],2);
		// compute point for intersection line L to pass thru
		float P3[3]={((N1[0]-dot(N1,N2)*N2[0])*(dot(N1,P1)-dot(N1,N2)*dot(N2,P2)))/e2+N2[0]*dot(N2,P2)+N3[0]*dot(N3,P1),
					((N1[1]-dot(N1,N2)*N2[1])*(dot(N1,P1)-dot(N1,N2)*dot(N2,P2)))/e2+N2[1]*dot(N2,P2)+N3[1]*dot(N3,P1),
					((N1[2]-dot(N1,N2)*N2[2])*(dot(N1,P1)-dot(N1,N2)*dot(N2,P2)))/e2+N2[2]*dot(N2,P2)+N3[2]*dot(N3,P1)};
		
		// parametric solution for intersection of line L with rays a1-b1, a1-c1, a2-b2, & a2-c2
		float point1[3], point2[3];

		if (mode==1){ //k-i-l-j intersection, aka intersection from i to l on both triangles
			edgeLineIntersection(face1->node_array[0],face1->node_array[1],N3,P3,point1); // i 
			edgeLineIntersection(face2->node_array[0],face2->node_array[2],N3,P3,point2); // l
		}
		else if (mode==2){ //k-i-j-l intersection, aka intersection from i to j on both triangles
			edgeLineIntersection(face1->node_array[0],face1->node_array[1],N3,P3,point1); // i 
			edgeLineIntersection(face1->node_array[0],face1->node_array[2],N3,P3,point2); // j 
		}
		else if (mode==3){ //i-k-l-j intersection, aka intersection from k to l on both triangles
			edgeLineIntersection(face2->node_array[0],face2->node_array[1],N3,P3,point1); // k 
			edgeLineIntersection(face2->node_array[0],face2->node_array[2],N3,P3,point2); // l 
		}
		else if (mode==4){ //i-k-j-l intersection, aka intersection from k to j on both triangles
			edgeLineIntersection(face2->node_array[0],face2->node_array[1],N3,P3,point1); // k 
			edgeLineIntersection(face1->node_array[0],face1->node_array[2],N3,P3,point2); // j 
		}

		printf("Triangle intersection occurs between (%f,%f,%f) & (%f,%f,%f)\n",point1[0],point1[1],point1[2],point2[0],point2[1],point2[2]);	
		printf("for face1= (%f,%f,%f)->(%f,%f,%f)->(%f,%f,%f)\n",
				face1->node_array[0]->x,face1->node_array[0]->y,face1->node_array[0]->z,
				face1->node_array[1]->x,face1->node_array[1]->y,face1->node_array[1]->z,
				face1->node_array[2]->x,face1->node_array[2]->y,face1->node_array[2]->z);
		printf("for face2= (%f,%f,%f)->(%f,%f,%f)->(%f,%f,%f)\n",
				face2->node_array[0]->x,face2->node_array[0]->y,face2->node_array[0]->z,
				face2->node_array[1]->x,face2->node_array[1]->y,face2->node_array[1]->z,
				face2->node_array[2]->x,face2->node_array[2]->y,face2->node_array[2]->z);

		// episode 18
		struct FLOATARRAYLIST* new_constraint_edge1=(struct FLOATARRAYLIST*)malloc((6)*sizeof(int)+sizeof(struct FLOATARRAYLIST*));
		struct FLOATARRAYLIST* new_constraint_edge2=(struct FLOATARRAYLIST*)malloc((6)*sizeof(int)+sizeof(struct FLOATARRAYLIST*));	

		new_constraint_edge1->numVals=6;
		new_constraint_edge1->array[0]=point1[0]; // encode x y z for both points to store edge information for face1
		new_constraint_edge1->array[1]=point1[1];
		new_constraint_edge1->array[2]=point1[2];
		new_constraint_edge1->array[3]=point2[0];
		new_constraint_edge1->array[4]=point2[1];
		new_constraint_edge1->array[5]=point2[2];
		new_constraint_edge1->next=NULL;
		new_constraint_edge2->numVals=6;
		new_constraint_edge2->array[0]=point1[0]; // encode x y z for both points to store edge information for face2
		new_constraint_edge2->array[1]=point1[1];
		new_constraint_edge2->array[2]=point1[2];
		new_constraint_edge2->array[3]=point2[0];
		new_constraint_edge2->array[4]=point2[1];
		new_constraint_edge2->array[5]=point2[2];
		new_constraint_edge2->next=NULL;

		struct FLOATARRAYLIST* iter=face1->constraint_edges; // store line of intersection in the face1 constraint edge list
		if (iter==NULL)
			face1->constraint_edges=new_constraint_edge1;
		else {
			while (iter->next!=NULL)
				iter=iter->next;
			iter->next=new_constraint_edge1;
		}
		iter=face2->constraint_edges; // store line of intersection in the face2 constraint edge list
		if (iter==NULL)
			face2->constraint_edges=new_constraint_edge2;
		else {
			while (iter->next!=NULL)
				iter=iter->next;
			iter->next=new_constraint_edge2;
		}
		// episode 18


	}
	else if (mode==5){ // coplanar intersection detected
		float intersectionPoints[6][3]; // maximum overlap of 2 triangles is a hexagon
		int numPoints=0;
		// go around triangle 1 and see where edges intersect with triangle 2
		// and where triangle 1 nodes are within triangle 2
		// and where triangle 2 nodes are within triangle 1
		for (int p=0; p<3; p++){
			// check if pth node of T1 is inside T2
			if(coplanarNodeWithinTriangle(face1->node_array[p],face2)){
				intersectionPoints[numPoints][0]=face1->node_array[p]->x;
				intersectionPoints[numPoints][1]=face1->node_array[p]->y;
				intersectionPoints[numPoints++][2]=face1->node_array[p]->z;
			}
			// check if pth node of T2 is inside T1
			if(coplanarNodeWithinTriangle(face2->node_array[p],face1)){
				intersectionPoints[numPoints][0]=face2->node_array[p]->x;
				intersectionPoints[numPoints][1]=face2->node_array[p]->y;
				intersectionPoints[numPoints++][2]=face2->node_array[p]->z;
			}
			for (int q=0; q<3; q++){ 
				// check if pth edge of T1 crosses qth edge of T2
				if (edgeEdgeIntersection(1,face1->node_array[p],face1->node_array[(p+1)%3],face2->node_array[q],face2->node_array[(q+1)%3],intersectionPoints[numPoints])){
					numPoints++;		
				}
			}
		}
		// order intersectionPoints CCW

		if (numPoints>3)
			reorderPoints(numPoints,intersectionPoints);

		printf("Triangle overlap occurs inside");
		for (int i=0; i<numPoints; i++)
			printf(" (%f,%f,%f)",intersectionPoints[i][0],intersectionPoints[i][1],intersectionPoints[i][2]);
		printf("\n");
	}
	return;
}

bool planarPointWithinTriangle(float P[2],float V1[2],float V2[2],float V3[2]){
					
	float AB[2]={V2[0]-V1[0],V2[1]-V1[1]};
	float BC[2]={V3[0]-V2[0],V3[1]-V2[1]};
	float CA[2]={V1[0]-V3[0],V1[1]-V3[1]};
	float AP[2]={P[0]-V1[0],P[1]-V1[1]};
	float BP[2]={P[0]-V2[0],P[1]-V2[1]};
	float CP[2]={P[0]-V3[0],P[1]-V3[1]};

	float N1[2]={AB[1],-AB[0]};
	float N2[2]={BC[1],-BC[0]};
	float N3[2]={CA[1],-CA[0]};
	
	float S1=AP[0]*N1[0]+AP[1]*N1[1];
	float S2=BP[0]*N2[0]+BP[1]*N2[1];
	float S3=CP[0]*N3[0]+CP[1]*N3[1];

	float tolerance=0.0001;

	if ((S1<0&&S2<0&&S3<0)||
		(S1<tolerance&&S2<0&&S3<0)||
		(S2<tolerance&&S1<0&&S3<0)||
		(S3<tolerance&&S1<0&&S2<0)){ // inside triangle
		return 1;	
	}
	else{
		return 0;
	}

}

// return 1 if p3 is left of p1->p2, 0 if right
bool pointDirectionFromLineSegment2D(float p1[2], float p2[2], float p3[2]){
	return ((p3[0]-p1[0])*(p2[1]-p1[1])+(p3[1]-p1[1])*(p1[0]-p2[0]))<0;
}

// return 1 if p1->p2 crosses p3->p4
bool lineSegmentsCross2D(float p1[2], float p2[2], float p3[2], float p4[2]){

	float v12[2]={p2[0]-p1[0],p2[1]-p1[1]};
	float v23[2]={p3[0]-p2[0],p3[1]-p2[1]};
	float v24[2]={p4[0]-p2[0],p4[1]-p2[1]};

	float cp1=v12[0]*v23[1]-v23[0]*v12[1];
	float cp2=v12[0]*v24[1]-v24[0]*v12[1];

	if ((cp1*cp2)>=0) // 1st orientation test proves no intersection possible
		return 0;

	float v34[2]={p4[0]-p3[0],p4[1]-p3[1]};
	float v41[2]={p1[0]-p4[0],p1[1]-p4[1]};
	float v42[2]={p2[0]-p4[0],p2[1]-p4[1]}; // this is the opposite of v24 above, can consolidate for performance

	cp1=v34[0]*v41[1]-v41[0]*v34[1];
	cp2=v34[0]*v42[1]-v42[0]*v34[1];

	if ((cp1*cp2)>=0) // 2nd orientation test proves no intersection possible
		return 0;

	return 1;

}

// returns 1 if quadrilateral p1-p2-p3-p4 is convex
bool quadrilateralConvex2D(float p1[2], float p2[2], float p3[2], float p4[2]){
	float sides[4][2]={{p2[0]-p1[0],p2[1]-p1[1]},
						{p3[0]-p2[0],p3[1]-p2[1]},
						{p4[0]-p3[0],p4[1]-p3[1]},
						{p1[0]-p4[0],p1[1]-p4[1]}};
	float cp;
	bool sign=0;
	for (int i=0;i<4;i++){
		cp=sides[i][0]*sides[(i+1)%4][1]-sides[i][1]*sides[(i+1)%4][0];
		if (i==0)
			sign=(cp>0);
		else if (sign!=(cp>0))
			return 0;
	}
	return 1;
}

// old delaunayTriangulate() function
void retriangulateFaceWithConstraintEdges(struct BODY* body, struct FACE* face){

	printf("lets start retriangulating\n");

	// normalized x-coordinate vector from node 1 to node 2
	float u[3];
	vectorBetweenNodes(face->node_array[0],face->node_array[1],u);
	normalizeVector(u);

	// normalize face normal
	normalizeVector(face->normal);

	// define planar y-coordinate vector
	float v[3];
	cross(face->normal,u,v);

	// count # of points in the cloud for triangulation
	int numPoints=face->numNodes;

	printf("cross products\n");

	struct FLOATARRAYLIST* iterF=face->constraint_edges;
	if (iterF!=NULL){
		printf("hi\n");
		numPoints+=(iterF->numVals)/3;
		while (iterF->next!=NULL){
			numPoints+=(iterF->numVals)/3;
			iterF=iterF->next;
		}
	}

	printf("printing all faces in the body:\n");
	int iii=0;
	struct FACE* iter_face3=body->face;
	while (iter_face3!=NULL){
		printf("%d: (%f,%f,%f)\n",iii++,iter_face3->normal[0],iter_face3->normal[1],iter_face3->normal[2]);
		iter_face3=iter_face3->next;
	}

	printf("u=(%f,%f,%f)\n",u[0],u[1],u[2]);
	printf("v=(%f,%f,%f)\n",v[0],v[1],v[2]);
	printf("n=(%f,%f,%f)\n",face->normal[0],face->normal[1],face->normal[2]);

	// define 2D point cloud
	float (*points)[2]=malloc(numPoints*2*sizeof(float));
	
	// populate the 2D point cloud from the 3D data of nodes
	for (int i=0; i<face->numNodes;i++){ // can skip the 0th node and just put (0,0), but for completeness:
		points[i][0]=u[0]*(face->node_array[i]->x-face->node_array[0]->x)
					+u[1]*(face->node_array[i]->y-face->node_array[0]->y)
					+u[2]*(face->node_array[i]->z-face->node_array[0]->z);
		points[i][1]=v[0]*(face->node_array[i]->x-face->node_array[0]->x)
					+v[1]*(face->node_array[i]->y-face->node_array[0]->y)
					+v[2]*(face->node_array[i]->z-face->node_array[0]->z);
		// we dont have to bother with the z probably
/*		points[i][2]=face->normal[0]*(face->node_array[i]->x-face->node_array[0]->x)
					+face->normal[1]*(face->node_array[i]->y-face->node_array[0]->y)
					+face->normal[2]*(face->node_array[i]->z-face->node_array[0]->z);
*/
	}


	// define the constraint edge list
	struct ARRAYLIST* constraintEdges=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
	struct ARRAYLIST* iter_array=constraintEdges;

	int j=face->numNodes;
	iterF=face->constraint_edges;
	while (iterF!=NULL){
		for (int i=0; i<(iterF->numVals)/6;i++){
			// populate the 2D point cloud from the 3D data of constraint edges
			points[j][0]=u[0]*(iterF->array[6*i]-face->node_array[0]->x)
						+u[1]*(iterF->array[6*i+1]-face->node_array[0]->y)
						+u[2]*(iterF->array[6*i+2]-face->node_array[0]->z);
			points[j][1]=v[0]*(iterF->array[6*i]-face->node_array[0]->x)
						+v[1]*(iterF->array[6*i+1]-face->node_array[0]->y)
						+v[2]*(iterF->array[6*i+2]-face->node_array[0]->z);
			points[j+1][0]=u[0]*(iterF->array[6*i+3]-face->node_array[0]->x)
						+u[1]*(iterF->array[6*i+4]-face->node_array[0]->y)
						+u[2]*(iterF->array[6*i+5]-face->node_array[0]->z);
			points[j+1][1]=v[0]*(iterF->array[6*i+3]-face->node_array[0]->x)
						+v[1]*(iterF->array[6*i+4]-face->node_array[0]->y)
						+v[2]*(iterF->array[6*i+5]-face->node_array[0]->z);
			
			// populate the constraint edge list with IDs
			struct ARRAYLIST* newConstraintEdgeIDs=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
			newConstraintEdgeIDs->numVals=2;
			newConstraintEdgeIDs->array[0]=j;
			newConstraintEdgeIDs->array[1]=j+1;
			newConstraintEdgeIDs->next=NULL;

			iter_array->next=newConstraintEdgeIDs;
			iter_array=newConstraintEdgeIDs;

			j+=2;
		}
		iterF=iterF->next;
	}

	printf("PRE point cloud: \n");
	for (int i=0; i<numPoints; i++){
		printf("(%f,%f)\n",points[i][0],points[i][1]);
	}

// find min and max boundaries of point cloud
	float xmin=0,xmax=0,ymin=0,ymax=0;
	for (int i=0;i<numPoints;i++){
		if (points[i][0]>xmax)
			xmax=points[i][0];
		else if (points[i][0]<xmin)
			xmin=points[i][0];
		if (points[i][1]>ymax)
			ymax=points[i][1];
		else if (points[i][1]<ymin)
			ymin=points[i][1];
	}

	// remap everything (preserving the aspect ratio) to between (0,0)-(1,1)
	float height=ymax-ymin;
	float width=xmax-xmin;
	float d=height; // d=largest dimension
	if (width>d)
		d=width;
	for (int i=0;i<numPoints;i++){
		points[i][0]=(points[i][0]-xmin)/d;
		points[i][1]=(points[i][1]-ymin)/d;
	}

	// sort points by proximity
	int pointOrder[numPoints]; // 017
	for (int i=0;i<numPoints;i++)
		pointOrder[i]=i;
	int NbinRows=(int)ceil(pow(numPoints,0.25));
	int* bins=malloc(numPoints*sizeof(int));
	for (int i=0;i<numPoints;i++){
		int p=(int)(points[i][1]*NbinRows*0.999); // bin row
		int q=(int)(points[i][0]*NbinRows*0.999); // bin column
		if (p%2)
			bins[i]=(p+1)*NbinRows-q;
		else
			bins[i]=p*NbinRows+q+1;
	}
	int key;
	for(int i=1; i<numPoints; i++){ // insertion sort
		key=bins[i];
		float tempF[2]={points[i][0],points[i][1]};
		int tempI=pointOrder[i];
		int j=i-1;
	    while(j>=0&&(bins[j]>key)){
			bins[j+1]=bins[j];
			points[j+1][0]=points[j][0];points[j+1][1]=points[j][1];
			pointOrder[j+1]=pointOrder[j];
			j--;
		}
		bins[j+1]=key;
		points[j+1][0]=tempF[0];points[j+1][1]=tempF[1];
		pointOrder[j+1]=tempI;
	}

	// add big triangle around our point cloud
	points=realloc(points,(3+numPoints)*2*sizeof(float));
	points[numPoints][0]=-100;points[numPoints][1]=-100;
	points[numPoints+1][0]=100;points[numPoints+1][1]=-100;
	points[numPoints+2][0]=0;points[numPoints+2][1]=100;
	numPoints+=3;

	// data structures required
	int (*verts)[3]=malloc(3*sizeof(int));
	verts[0][0]=numPoints-3;
	verts[0][1]=numPoints-2;
	verts[0][2]=numPoints-1;
	int (*tris)[3]=malloc(3*sizeof(int));
	tris[0][0]=-1;
	tris[0][1]=-1;
	tris[0][2]=-1;
	int nT=1;
	int *triangleStack=malloc((numPoints-3)*sizeof(int)); // is this a big enough stack?
	int tos=-1;
	// insert all points and triangulate one by one
	for (int ii=0;ii<(numPoints-3);ii++){
		// find triangle T which contains points[i]
		int j=nT-1; // last triangle created
		while (1){
			if (planarPointWithinTriangle(points[ii],points[verts[j][0]],points[verts[j][1]],points[verts[j][2]])){
				nT+=2;
				// delete triangle T and replace it with three sub-triangles touching P 
				tris=realloc(tris,(nT)*3*sizeof(int));
				verts=realloc(verts,(nT)*3*sizeof(int));
				// vertices of new triangles
				verts[nT-2][0]=ii;
				verts[nT-2][1]=verts[j][1];
				verts[nT-2][2]=verts[j][2];
				verts[nT-1][0]=ii;
				verts[nT-1][1]=verts[j][2];
				verts[nT-1][2]=verts[j][0];
				// update adjacencies of triangles surrounding the old triangle
				// fix adjacency of A
				int adj1=tris[j][0];
				int adj2=tris[j][1];
				int adj3=tris[j][2];
				if (adj1>=0)
					for (int m=0;m<3;m++)
						if (tris[adj1][m]==j){
							tris[adj1][m]=j;
							break;
						}
				if (adj2>=0)
					for (int m=0;m<3;m++)
						if (tris[adj2][m]==j){
							tris[adj2][m]=nT-2;
							break;
						}
				if (adj3>=0)
					for (int m=0;m<3;m++)
						if (tris[adj3][m]==j){
							tris[adj3][m]=nT-1;
							break;
						}
				// adjacencies of new triangles	
				tris[nT-2][0]=j;
				tris[nT-2][1]=tris[j][1];
				tris[nT-2][2]=nT-1;
				tris[nT-1][0]=nT-2;
				tris[nT-1][1]=tris[j][2];
				tris[nT-1][2]=j;
				// replace v3 of containing triangle with P and rotate to v1
				verts[j][2]=verts[j][1];
				verts[j][1]=verts[j][0];
				verts[j][0]=ii;
				// replace 1st and 3rd adjacencies of containing triangle with new triangles
				tris[j][1]=tris[j][0];
				tris[j][2]=nT-2;
				tris[j][0]=nT-1;
				// place each triangle containing P onto a stack, if the edge opposite P has an adjacent triangle
				if (tris[j][1]>=0)
					triangleStack[++tos]=j;
				if (tris[nT-2][1]>=0)
					triangleStack[++tos]=nT-2;
				if (tris[nT-1][1]>=0)
					triangleStack[++tos]=nT-1;
				while (tos>=0){ // looping thru the stack
					int L=triangleStack[tos--];
					float v1[2]={points[verts[L][2]][0],points[verts[L][2]][1]};
					float v2[2]={points[verts[L][1]][0],points[verts[L][1]][1]};
					int oppVert=-1;
					int oppVertID=-1;
					for (int k=0;k<3;k++){
						if ((verts[tris[L][1]][k]!=verts[L][1])
						   &&(verts[tris[L][1]][k]!=verts[L][2])){
							oppVert=verts[tris[L][1]][k];
							oppVertID=k;
							break;
						}
					}
					float v3[2]={points[oppVert][0],points[oppVert][1]};
					float P[2]={points[ii][0],points[ii][1]};
					
					// check if P in circumcircle of triangle on top of stack
					float cosa=((v1[0]-v3[0])*(v2[0]-v3[0])+(v1[1]-v3[1])*(v2[1]-v3[1]));
					float cosb=((v2[0]-P[0])*(v1[0]-P[0])+(v2[1]-P[1])*(v1[1]-P[1]));
					float sina=((v1[0]-v3[0])*(v2[1]-v3[1])-(v1[1]-v3[1])*(v2[0]-v3[0]));
					float sinb=((v2[0]-P[0])*(v1[1]-P[1])-(v2[1]-P[1])*(v1[0]-P[0]));
					
					if (((cosa<0)&&(cosb<0))||
						((-cosa*((v2[0]-P[0])*(v1[1]-P[1])-(v2[1]-P[1])*(v1[0]-P[0])))>
						 (cosb*((v1[0]-v3[0])*(v2[1]-v3[1])-(v1[1]-v3[1])*(v2[0]-v3[0]))))){

						// swap diagonal, and redo triangles L R A & C
						// initial state:
						int R=tris[L][1];
						int C=tris[L][2];
						int A=tris[R][(oppVertID+2)%3];
						// fix adjacency of A
						if (A>=0)
							for (int m=0;m<3;m++)
								if (tris[A][m]==R){
									tris[A][m]=L;
									break;
								}
						// fix adjacency of C
						if (C>=0)
							for (int m=0;m<3;m++)
								if (tris[C][m]==L){
									tris[C][m]=R;
									break;
								}
						// fix vertices and adjacency of R
						for (int m=0;m<3;m++)
							if (verts[R][m]==oppVert){
								verts[R][(m+2)%3]=ii;
								break;
							}
						for (int m=0;m<3;m++)
							if (tris[R][m]==L){
								tris[R][m]=C;
								break;
							}
						for (int m=0;m<3;m++)
							if (tris[R][m]==A){
								tris[R][m]=L;
								break;
							}
						for (int m=0;m<3;m++)
							if (verts[R][0]!=ii){
								int temp1=verts[R][0];
								int temp2=tris[R][0];
								verts[R][0]=verts[R][1];
								verts[R][1]=verts[R][2];
								verts[R][2]=temp1;
								tris[R][0]=tris[R][1];
								tris[R][1]=tris[R][2];
								tris[R][2]=temp2;
							}
						
						// fix vertices and adjacency of L
						verts[L][2]=oppVert;
						for (int m=0;m<3;m++)
							if (tris[L][m]==C){
								tris[L][m]=R;
								break;
							}
						for (int m=0;m<3;m++)
							if (tris[L][m]==R){
								tris[L][m]=A;
								break;
							}
						// add L and R to stack if they have triangles opposite P;
						if (tris[L][1]>=0)
							triangleStack[++tos]=L;
						if (tris[R][1]>=0)
							triangleStack[++tos]=R;
					}
				}
				break;
			}
		
			// adjust j in the direction of target point ii
			float AB[2]={points[verts[j][1]][0]-points[verts[j][0]][0],points[verts[j][1]][1]-points[verts[j][0]][1]};
			float BC[2]={points[verts[j][2]][0]-points[verts[j][1]][0],points[verts[j][2]][1]-points[verts[j][1]][1]};
			float CA[2]={points[verts[j][0]][0]-points[verts[j][2]][0],points[verts[j][0]][1]-points[verts[j][2]][1]};
			float AP[2]={points[ii][0]-points[verts[j][0]][0],points[ii][1]-points[verts[j][0]][1]};
			float BP[2]={points[ii][0]-points[verts[j][1]][0],points[ii][1]-points[verts[j][1]][1]};
			float CP[2]={points[ii][0]-points[verts[j][2]][0],points[ii][1]-points[verts[j][2]][1]};
			float N1[2]={AB[1],-AB[0]};
			float N2[2]={BC[1],-BC[0]};
			float N3[2]={CA[1],-CA[0]};
			float S1=AP[0]*N1[0]+AP[1]*N1[1];
			float S2=BP[0]*N2[0]+BP[1]*N2[1];
			float S3=CP[0]*N3[0]+CP[1]*N3[1];
			if ((S1>0)&&(S1>=S2)&&(S1>=S3))
				j=tris[j][0];
			else if ((S2>0)&&(S2>=S1)&&(S2>=S3))
				j=tris[j][1];
			else if ((S3>0)&&(S3>=S1)&&(S3>=S2))
				j=tris[j][2];
		}
	}
	
	// count how many triangles we have that dont involve supertriangle vertices
	int nT_final=nT;
	int *renumberAdj=calloc(nT,sizeof(int));
	bool *deadTris=calloc(nT,sizeof(bool));
	for (int i=0;i<nT;i++)		
		if ((verts[i][0]>=(numPoints-3))
			||(verts[i][1]>=(numPoints-3))
			||(verts[i][2]>=(numPoints-3))){
			deadTris[i]=1;
			renumberAdj[i]=nT-(nT_final--);
		}
		else 
			renumberAdj[i]=nT-(nT_final);

	// delete any triangles that contain the supertriangle vertices
	int (*verts_final)[3]=malloc(3*nT_final*sizeof(int));
	int (*tris_final)[3]=malloc(3*nT_final*sizeof(int));
	numPoints-=3;
	int index=0;
	for (int i=0;i<nT;i++)		
		if ((verts[i][0]<(numPoints))
			&&(verts[i][1]<(numPoints))
			&&(verts[i][2]<(numPoints))){
			verts_final[index][0]=verts[i][0];
			verts_final[index][1]=verts[i][1];
			verts_final[index][2]=verts[i][2];
			tris_final[index][0]=(1-deadTris[tris[i][0]])*tris[i][0]-deadTris[tris[i][0]];
			tris_final[index][1]=(1-deadTris[tris[i][1]])*tris[i][1]-deadTris[tris[i][1]];
			tris_final[index++][2]=(1-deadTris[tris[i][2]])*tris[i][2]-deadTris[tris[i][2]];
		}
	for (int i=0;i<nT_final;i++){
		if (tris_final[i][0]>=0)
			tris_final[i][0]-=renumberAdj[tris_final[i][0]];
		if (tris_final[i][1]>=0)
			tris_final[i][1]-=renumberAdj[tris_final[i][1]];
		if (tris_final[i][2]>=0)
			tris_final[i][2]-=renumberAdj[tris_final[i][2]];
	}

	printf("Post-Triangulation, Pre-Constraint Vertices:\n");
	for (int i=0;i<nT_final;i++)
		printf("[%d,%d,%d]\n",verts_final[i][0],verts_final[i][1],verts_final[i][2]);
	
	// start video 017

	struct ARRAYLIST* iter=constraintEdges;
	while (iter!=NULL){ // loop thru all constraint edge loops
		for (int i=0;i<(iter->numVals-1);i++){ // loop thru edge in the constraint edge loop
			int vert1, vert2;
			for (int j=0;j<numPoints;j++){ 
				if (pointOrder[j]==iter->array[i])
					vert1=j;
				if (pointOrder[j]==iter->array[i+1])
					vert2=j;
			}
			
			// generate a list of triangles that each vertex is contained in, if we have a lot of constraint edges, it would be good to pull this out of the loop
			int vert1_tris[numPoints+3]; // should be long enough
			int vert1_tris_ind[numPoints+3]; // should be long enough
			int vert2_tris[numPoints+3]; // should be long enough
			int ind1=0,ind2=0;
			for (int j=0;j<nT_final;j++){ // loop thru all triangles to see which triangles contains our important vertices
				if (verts_final[j][0]==vert1){
					vert1_tris_ind[ind1]=0;
					vert1_tris[ind1++]=j;
				}
				else if (verts_final[j][1]==vert1){
					vert1_tris_ind[ind1]=1;
					vert1_tris[ind1++]=j;
				}
				else if (verts_final[j][2]==vert1){
					vert1_tris_ind[ind1]=2;
					vert1_tris[ind1++]=j;
				}
				if (verts_final[j][0]==vert2||verts_final[j][1]==vert2||verts_final[j][2]==vert2)
					vert2_tris[ind2++]=j;
			}

			bool edgeFound=0;
			int triTraj=-1;
			int triVertexInd=-1;
			for (int j=0;j<ind1;j++){ // look thru all triangles containing vert1, looking for vert2, or until you find an edge that intersects vert1-vert2
				if (verts_final[vert1_tris[j]][0]==vert2||verts_final[vert1_tris[j]][1]==vert2||verts_final[vert1_tris[j]][2]==vert2){ // exact edge found
					edgeFound=1;
					break;
				}
				else if	(pointDirectionFromLineSegment2D(points[vert1], // is vert2 to the left of both edges adjacent to vert1 on triangle j
						points[verts_final[vert1_tris[j]][(vert1_tris_ind[j]+1)%3]],
						points[vert2])&&
						pointDirectionFromLineSegment2D(points[verts_final[vert1_tris[j]][(vert1_tris_ind[j]+2)%3]],
						points[vert1],
						points[vert2])){
					triTraj=vert1_tris[j];
					triVertexInd=vert1_tris_ind[j];
					break;
				}
			}
			if (edgeFound){ // constraint edge already in vertex array, do nothing
			
			}
			else { // constraint edge NOT found in vertex array
				// start looking for intersections in triangle triTraj
				struct ARRAYLIST* intersections=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
				struct ARRAYLIST* iter2=intersections;
				iter2->next=NULL; // add the first intersection edge details
				iter2->numVals=-1;

				while (1){
					if ((verts_final[triTraj][0]==vert2)|| // we made it to the triangle containing vert2
						(verts_final[triTraj][1]==vert2)||
						(verts_final[triTraj][2]==vert2)){
						break;
					}
					for (int j=0;j<3;j++){ // check if vert2 is to the right of any edge in the current triangle

						if (!pointDirectionFromLineSegment2D(points[verts_final[triTraj][j]], // if it is, it's a candidate for intersection
							points[verts_final[triTraj][(j+1)%3]], // because the entry edge will fail this test, so no double counting
							points[vert2])){

							if (lineSegmentsCross2D(points[vert1],points[vert2],points[verts_final[triTraj][j]],points[verts_final[triTraj][(j+1)%3]])){
								if (iter2->numVals==-1){
									iter2->next=NULL;
									iter2->numVals=2;
									iter2->array[0]=verts_final[triTraj][j];
									iter2->array[1]=verts_final[triTraj][(j+1)%3];
								}
								else{
									struct ARRAYLIST* nextIntersection=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
									iter2->next=nextIntersection;
									iter2=iter2->next;
									iter2->next=NULL;
									iter2->numVals=2;
									iter2->array[0]=verts_final[triTraj][j];
									iter2->array[1]=verts_final[triTraj][(j+1)%3];
								}
									triTraj=tris_final[triTraj][j];
								break;
							}
						}
					}
				}
				// loop thru edges that need to be removed, can't save the triangle index because we are killing triangles :(
				struct ARRAYLIST* iter3=intersections;
				struct ARRAYLIST* iter99=intersections;
		
				int triangle0=-1, triangle1=-1;
				// new edges that we are adding to the triangulation
				struct ARRAYLIST* newEdges=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
				struct ARRAYLIST* iter4=newEdges;
				newEdges->numVals=-1;
				while (iter3!=NULL){
					// find triangles that contain this edge (one forward, one backward)
					int triangle0_other_ind=-1;
					int triangle1_other_ind=-1;
					for (int j=0;j<nT_final;j++){
						if ((verts_final[j][0]==iter3->array[1])&&(verts_final[j][1]==iter3->array[0])){
							triangle1=j;
							triangle1_other_ind=2;
							break;
						}							
						if ((verts_final[j][1]==iter3->array[1])&&(verts_final[j][2]==iter3->array[0])){
							triangle1=j;
							triangle1_other_ind=0;
							break;
						}							
						if ((verts_final[j][2]==iter3->array[1])&&(verts_final[j][0]==iter3->array[0])){
							triangle1=j;
							triangle1_other_ind=1;
							break;
						}							
					}
					for (int j=0;j<nT_final;j++){
						if ((verts_final[j][0]==iter3->array[0])&&(verts_final[j][1]==iter3->array[1])){
							triangle0=j;
							triangle0_other_ind=2;
							break;
						}							
						if ((verts_final[j][1]==iter3->array[0])&&(verts_final[j][2]==iter3->array[1])){
							triangle0=j;
							triangle0_other_ind=0;
							break;
						}							
						if ((verts_final[j][2]==iter3->array[0])&&(verts_final[j][0]==iter3->array[1])){
							triangle0=j;
							triangle0_other_ind=1;
							break;
						}							
					}
					
					if (quadrilateralConvex2D(points[verts_final[triangle0][(triangle0_other_ind+2)%3]],
											points[verts_final[triangle0][triangle0_other_ind]],
											points[verts_final[triangle0][(triangle0_other_ind+1)%3]],
											points[verts_final[triangle1][triangle1_other_ind]])){

						// triangle convex, swap diagonal
						verts_final[triangle0][(triangle0_other_ind+2)%3]=verts_final[triangle1][triangle1_other_ind];
						verts_final[triangle1][(triangle1_other_ind+2)%3]=verts_final[triangle0][triangle0_other_ind];

						// fix primary triangle adjacencies
						tris_final[triangle0][(triangle0_other_ind+1)%3]=tris_final[triangle1][(triangle1_other_ind+2)%3];
						tris_final[triangle1][(triangle1_other_ind+1)%3]=tris_final[triangle0][(triangle0_other_ind+2)%3];


						tris_final[triangle0][(triangle0_other_ind+2)%3]=triangle1;
						tris_final[triangle1][(triangle1_other_ind+2)%3]=triangle0;
						// fix 2 nearby triangle adjacencies
						int tri2Fix0=tris_final[triangle0][triangle0_other_ind];
						int tri2Fix1=tris_final[triangle1][triangle1_other_ind];
						for (int m=0;m<3;m++)
							if (tris_final[tri2Fix0][m]==triangle0){
								tris_final[tri2Fix0][m]=triangle1;
								break;
							}
						for (int m=0;m<3;m++)
							if (tris_final[tri2Fix1][m]==triangle1){
								tris_final[tri2Fix1][m]=triangle0;
								break;
							}

						triangle0_other_ind++; // rotate around 1 vtx
						triangle1_other_ind++; // rotate around 1 vtx



						// if the new diagonal still intersects the constraint edge, add it to the list
						if (lineSegmentsCross2D(points[vert1],points[vert2],
												points[verts_final[triangle0][(triangle0_other_ind+1)%3]],
												points[verts_final[triangle1][(triangle1_other_ind+1)%3]])){
							struct ARRAYLIST* nextIntersection=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
							iter2->next=nextIntersection;
							iter2=iter2->next;
							iter2->next=NULL;
							iter2->numVals=2;
							iter2->array[0]=verts_final[triangle0][(triangle0_other_ind+1)%3];
							iter2->array[1]=verts_final[triangle1][(triangle1_other_ind+1)%3];
						}
						else {
							// if diagonal doesnt intersect the constraint edge, add to newEdge list
							if (iter4->numVals==-1){ // first new edge
								iter4->next=NULL;
								iter4->numVals=2; // 2 vertices	
								iter4->array[0]=verts_final[triangle0][(triangle0_other_ind+1)%3];
								iter4->array[1]=verts_final[triangle1][(triangle1_other_ind+1)%3];
							}
							else {
								struct ARRAYLIST* nextEdge=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
								iter4->next=nextEdge;
								iter4=iter4->next;
								iter4->next=NULL;
								iter4->numVals=2; // 2 vertices	
								iter4->array[0]=verts_final[triangle0][(triangle0_other_ind+1)%3];
								iter4->array[1]=verts_final[triangle1][(triangle1_other_ind+1)%3];							
							}
						}
					}
					else {
						// quad not convex, put diagonal on the end of the linked list
						struct ARRAYLIST* iter5=intersections;
						struct ARRAYLIST* prev5=NULL;
						while (iter5->next!=NULL){
							if (iter5==iter3){ // remove the old diagonal from the linked list
								if (prev5==NULL){
									intersections=iter3->next; // free the skipped one? (leak here?)
								}
								else {
									prev5->next=iter3->next; // again free the skipped one? (leak here?)
								}
							}
							prev5=iter5;
							iter5=iter5->next;
						}
						struct ARRAYLIST* nextIntersection=(struct ARRAYLIST*)malloc((2+1)*sizeof(int)+sizeof(struct ARRAYLIST*));
						iter5->next=nextIntersection;
						iter5=iter5->next;
						iter5->next=NULL;
						iter5->numVals=2;
						iter5->array[0]=verts_final[triangle0][(triangle0_other_ind+1)%3];
						iter5->array[1]=verts_final[triangle1][(triangle1_other_ind+1)%3];

					}
				iter3=iter3->next;
				}
			
				int swaps=1;
				while (swaps>0){ // until no further swaps take place
					swaps=0;
					// restore the delaunay triangulation on the newEdges
					// loop over newEdges
					struct ARRAYLIST* iter6=newEdges;
					while (iter6!=NULL){

						// if the new edge is NOT the constraint edge
						if (!(((iter6->array[0]==vert1)&&
							(iter6->array[1]==vert2))||
							((iter6->array[0]==vert2)&&
							(iter6->array[1]==vert1)))){
							// identify lone vertex indices for the triangles which share the new edge, can keep an edge adjacency array to speed this up
							
							// find triangles that contain this edge (one forward, one backward)
							for (int j=0;j<nT_final;j++){
								if (((verts_final[j][0]==iter6->array[1])&&(verts_final[j][1]==iter6->array[0]))||
									((verts_final[j][1]==iter6->array[1])&&(verts_final[j][2]==iter6->array[0]))||
									((verts_final[j][2]==iter6->array[1])&&(verts_final[j][0]==iter6->array[0]))){
									triangle0=j;
									break;
								}
							}
							for (int j=0;j<nT_final;j++){
								if (((verts_final[j][0]==iter6->array[0])&&(verts_final[j][1]==iter6->array[1]))||
									((verts_final[j][1]==iter6->array[0])&&(verts_final[j][2]==iter6->array[1]))||
									((verts_final[j][2]==iter6->array[0])&&(verts_final[j][0]==iter6->array[1]))){
									triangle1=j;
									break;
								}
							}

							int triangle0_lone_vtx_ind=-1;
							int triangle1_lone_vtx_ind=-1;

							for (int j=0;j<3;j++) // find the lone vertex in triangle0
								if ((verts_final[triangle0][j]!=iter6->array[0])&&
								   (verts_final[triangle0][j]!=iter6->array[1])){
									triangle0_lone_vtx_ind=j;
								}
							for (int j=0;j<3;j++) // find the lone vertex in triangle1
								if ((verts_final[triangle1][j]!=iter6->array[0])&&
								   (verts_final[triangle1][j]!=iter6->array[1])){
									triangle1_lone_vtx_ind=j;
								}

							// if delaunay condition is not satisfied for triangles that share this edge, swap the diagonal
							float v1a[2]={points[verts_final[triangle0][(triangle0_lone_vtx_ind)]][0],
									 points[verts_final[triangle0][(triangle0_lone_vtx_ind)]][1]};
							float v1b[2]={points[verts_final[triangle0][(triangle0_lone_vtx_ind+1)%3]][0],
									 points[verts_final[triangle0][(triangle0_lone_vtx_ind+1)%3]][1]};
							float v1c[2]={points[verts_final[triangle0][(triangle0_lone_vtx_ind+2)%3]][0],
									 points[verts_final[triangle0][(triangle0_lone_vtx_ind+2)%3]][1]};	
							float v2a[2]={points[verts_final[triangle1][(triangle1_lone_vtx_ind)]][0],
									 points[verts_final[triangle1][(triangle1_lone_vtx_ind)]][1]};
							float cosa1=((v1b[0]-v1a[0])*(v1c[0]-v1a[0])+(v1b[1]-v1a[1])*(v1c[1]-v1a[1]));
							float cosa2=((v1c[0]-v2a[0])*(v1b[0]-v2a[0])+(v1c[1]-v2a[1])*(v1b[1]-v2a[1]));
							float cosb1=((v1c[0]-v2a[0])*(v1b[0]-v2a[0])+(v1c[1]-v2a[1])*(v1b[1]-v2a[1]));
							float cosb2=((v1b[0]-v1a[0])*(v1c[0]-v1a[0])+(v1b[1]-v1a[1])*(v1c[1]-v1a[1]));
							if (((((cosa1<0)&&(cosb1<0))|| // if triangle 0 vertex is inside triangle 1 circumcircle
								((-cosa1*((v1c[0]-v2a[0])*(v1b[1]-v2a[1])-(v1c[1]-v2a[1])*(v1b[0]-v2a[0])))>
								 (cosb1*((v1b[0]-v1a[0])*(v1c[1]-v1a[1])-(v1b[1]-v1a[1])*(v1c[0]-v1a[0]))))))|| // or
								 ((((cosa2<0)&&(cosb2<0))|| // if triangle 1 vertex is inside triangle 0 circumcircle
								((-cosa2*((v1b[0]-v1a[0])*(v1c[1]-v1a[1])-(v1b[1]-v1a[1])*(v1c[0]-v1a[0])))>
								 (cosb2*((v1c[0]-v2a[0])*(v1b[1]-v2a[1])-(v1c[1]-v2a[1])*(v1b[0]-v2a[0]))))))){

								verts_final[triangle0][(triangle0_lone_vtx_ind+2)%3]=verts_final[triangle1][triangle1_lone_vtx_ind];
								verts_final[triangle1][(triangle1_lone_vtx_ind+2)%3]=verts_final[triangle0][triangle0_lone_vtx_ind];

								// update triangle0 and triangle1 adjacencies
								tris_final[triangle0][(triangle0_lone_vtx_ind+1)%3]=tris_final[triangle1][(triangle1_lone_vtx_ind+2)%3];
								tris_final[triangle1][(triangle1_lone_vtx_ind+1)%3]=tris_final[triangle0][(triangle0_lone_vtx_ind+2)%3];
								tris_final[triangle0][(triangle0_lone_vtx_ind+2)%3]=triangle1;
								tris_final[triangle1][(triangle1_lone_vtx_ind+2)%3]=triangle0;
								// fix 2 nearby triangle adjacencies
								int tri2Fix0=tris_final[triangle0][triangle0_lone_vtx_ind];
								int tri2Fix1=tris_final[triangle1][triangle1_lone_vtx_ind];
								for (int m=0;m<3;m++)
									if (tris_final[tri2Fix0][m]==triangle0){
										tris_final[tri2Fix0][m]=triangle1;
										break;
									}
								for (int m=0;m<3;m++)
									if (tris_final[tri2Fix1][m]==triangle1){
										tris_final[tri2Fix1][m]=triangle0;
										break;
									}

								iter6->array[0]=verts_final[triangle0][triangle0_lone_vtx_ind]; // swap diagonal in new edge list
								iter6->array[1]=verts_final[triangle1][triangle1_lone_vtx_ind]; // swap diagonal in new edge list

								triangle0_lone_vtx_ind++; // rotate the lone vtx +1
								triangle1_lone_vtx_ind++; // rotate the lone vtx +1

								swaps++; // keep track of number of swaps

							}
						}
						iter6=iter6->next;
					}
				}
			}
		}
		iter=iter->next;
	}

	// end video 017

	// undo the mapping
	for (int i=0;i<numPoints;i++){
		points[i][0]=points[i][0]*d+xmin;
		points[i][1]=points[i][1]*d+ymin;
	}

	printf("\nPost-Constraint Vertices:\n");
	for (int i=0;i<nT_final;i++)
		printf("[%d,%d,%d]\n",verts_final[i][0],verts_final[i][1],verts_final[i][2]);
	printf("\nPoint Cloud at the end of DelaunayTriangulation:\n");
	for (int i=0;i<numPoints;i++)
		printf("%d: (%f,%f)\n",i,points[i][0],points[i][1]);
	printf("address of points:%p\n",&points);


	// episode 18
	
	// remap back to 3D
	float (*points3D)[3]=malloc(numPoints*3*sizeof(float)); // do i really need to malloc this?
	for (int i=0;i<numPoints;i++){
		points3D[i][0]=points[i][0]*u[0]+points[i][1]*v[0]+face->node_array[0]->x;
		points3D[i][1]=points[i][0]*u[1]+points[i][1]*v[1]+face->node_array[0]->y;
		points3D[i][2]=points[i][0]*u[2]+points[i][1]*v[2]+face->node_array[0]->z;
	}

	printf("remapped back to 3D:\n");	
		for (int i=0;i<numPoints;i++)
			printf("%d: (%f,%f,%f)\n",i,points3D[i][0],points3D[i][1],points3D[i][2]);
	

	// loop thru points and make whatever new nodes we need
	struct NODE* nodePointers[numPoints];
		//=malloc(numNodes*sizeof(struct NODE*));
	struct NODE* iter_node;
	for (int i=0;i<numPoints;i++){
		bool nodeExists=0;
		iter_node=body->node;
		while (iter_node!=NULL){
			if ((iter_node->x==points3D[i][0])&&
				(iter_node->y==points3D[i][1])&&
				(iter_node->z==points3D[i][2])){
					nodeExists=true;
					break;
			}
			iter_node=iter_node->next;
		}
		if (nodeExists){
			nodePointers[i]=iter_node;
		}
		else {
			struct NODE* newNode=(struct NODE*)malloc(sizeof(struct NODE));
			makeNode(newNode,points3D[i][0],points3D[i][1],points3D[i][2],NULL);
			addNode(body,newNode);
			nodePointers[i]=newNode;
		}
	}

	/*
	printf("created all the new nodes i think\n");

	printf("struct NODE* coords:\n");	
		for (int i=0;i<numPoints;i++)
			printf("%d: (%f,%f,%f)\n",i,nodePointers[i]->x,nodePointers[i]->y,nodePointers[i]->z);

	printf("printing all nodes in the body:\n");
	int ii=0;
	struct NODE* iter_node2=body->node;
	while (iter_node2!=NULL){
		printf("%d: (%f,%f,%f)\n",ii++,iter_node2->x,iter_node2->y,iter_node2->z);
		iter_node2=iter_node2->next;
	}
*/
	// add in new faces
	struct FACE* next_face=face->next;

	for (int i=0;i<(nT_final);i++){ // loop over all triangles
		struct FACE* new_face=(struct FACE*)malloc(sizeof(struct FACE*)+sizeof(int)+9*sizeof(float)+sizeof(struct FLOATARRAYLIST*)+3*sizeof(struct NODE*));
		struct NODE* node_array[3]={nodePointers[verts_final[i][0]],
								nodePointers[verts_final[i][1]],
								nodePointers[verts_final[i][2]]};
		makeFace(new_face,3,face->normal,node_array,next_face);
		next_face=new_face;
	}
	printf("created all the new faces i think\n");	

	// connect the linked list on the front end
	
	struct FACE* iter_face=body->face;
	while (iter_face->next!=face){
		iter_face=iter_face->next;
	}
	iter_face->next=next_face;
	
	printf("connect the linked list\n");
	
	// kill the old face
//	free(face);

	// episode 18

	return;
}

// episode 18

struct BODY* evaluateCSG(int operation, struct BODY* bodyA, struct BODY* bodyB){

	printf("bodyA at beginning\n");
	printBodyElements(bodyA);
//	printf("bodyB\n");
//	printBodyElements(bodyB);
	
	struct DRAWSTYLE* ds=defaultDrawstyle();
	float lookFrom[3]={10,10,10};
	float lookAt[3]={0,0,0};
//	drawBody(800,600,lookAt,lookFrom,bodyA,ds);
	
	// initialize resultant newBody
	struct BODY* newBody = (struct BODY*)malloc(sizeof(struct BODY));

	// THE LOOP BELOW HANDLES ALL THE NON-INTERSECTED FACES
	// loop thru all faces in bodyA
	//	loop thru all faces in bodyB
	//		if faceB is completely inside bodyA
	//			if union, skip this face 
	//			if intersection or difference, add this face to newBody
	//		if faceB is completely outside bodyA
	//			if union, add this face to newBody
	//			if intersection or difference, skip this face
	//		if faceA is completely inside bodyB
	//			if union or difference, skip this face 
	//			if intersection, add this face to newBody
	//		if faceA is completely outside bodyB
	//			if union or difference, add this face to newBody
	//			if intersection, skip this face

	
	// find all the intersects between faces in the bodies
	struct FACE* faceA=bodyA->face;
	struct FACE* faceB;
	while(faceA!=NULL){
		struct FACE* faceA_copy=deepCopyFace(faceA);
		faceB=bodyB->face;
		while(faceB!=NULL){
			printf("checking face:\n");
			struct FACE* faceB_copy=deepCopyFace(faceB);
			triangleIntersectionPoints(faceA_copy,faceB_copy);
			faceA->constraint_edges=faceA_copy->constraint_edges;
			faceB->constraint_edges=faceB_copy->constraint_edges;

			//free copyB
			faceB=faceB->next;
		}
		//free copyA
		faceA=faceA->next;
	}


	printf("bodyA after intersection computation\n");
	printBodyElements(bodyA);
	

	// retriangulate the faces that did intersect
	faceA=bodyA->face;
	while(faceA!=NULL){
		if (faceA->constraint_edges!=NULL){
			retriangulateFaceWithConstraintEdges(bodyA,faceA);
		}
		faceA=faceA->next;
	}

	faceA=bodyA->face;
	while(faceA!=NULL){
		ensureProperTriangleNodeOrder(faceA);
		faceA=faceA->next;
	}

	faceB=bodyB->face;
	while(faceB!=NULL){
		if (faceB->constraint_edges!=NULL){
			retriangulateFaceWithConstraintEdges(bodyB,faceB);
		}
		faceB=faceB->next;
	}

	faceA=bodyA->face;
	while(faceA!=NULL){
		ensureProperTriangleNodeOrder(faceA);
		faceA=faceA->next;
	}

	printf("bodyA\n");
	printBodyElements(bodyA);
	printf("bodyB\n");
	printBodyElements(bodyB);
	
	drawBody(800,600,lookAt,lookFrom,bodyA,ds);
	
	return newBody;
}
