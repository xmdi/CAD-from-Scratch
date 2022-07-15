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

void makeFace(struct FACE* face, int numNodes, float normal[3], struct NODE* node_array[], struct FACE* next){

	float xmin=node_array[0]->x;
	float xmax=node_array[0]->x;
	float ymin=node_array[0]->y;
	float ymax=node_array[0]->y;
	float zmin=node_array[0]->z;
	float zmax=node_array[0]->z;

	for (int i=0; i<numNodes; i++){
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

	face -> xmin = xmin;
	face -> xmax = xmax;
	face -> ymin = ymin;
	face -> ymax = ymax;
	face -> zmin = zmin;
	face -> zmax = zmax;

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

	struct FACE* face1 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face2 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face3 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face4 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face5 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + 4 * sizeof(struct NODE*));
	struct FACE* face6 = (struct FACE*)malloc(sizeof(struct FACE*) +  sizeof(float)*9+sizeof(int) + 4 * sizeof(struct NODE*));
	
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
	struct FACE* target_face=(struct FACE*)malloc(sizeof(struct FACE*)+sizeof(int)+9*sizeof(float)+3*sizeof(struct NODE*));
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
							struct FACE* next_face=(struct FACE*)malloc(sizeof(struct FACE*)+sizeof(int)+9*sizeof(float)+3*sizeof(struct NODE*));
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
	
	struct FACE* face = (struct FACE*)malloc(sizeof(struct FACE*)+sizeof(int)+9*sizeof(float)+numPoints*sizeof(struct NODE*));
	
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
		y4 = y5;
	}
	else if (((Y6<0)&&(Y4>0)&&(Y5>0))||((Y6>0)&&(Y4<0)&&(Y5<0))){
		rotateFaceNodes(face1,1);
		y4 = y6;
	}
	if (((Y2<0)&&(Y1>0)&&(Y3>0))||((Y2>0)&&(Y1<0)&&(Y3<0))){
		rotateFaceNodes(face2,2);
		y1 = y2;
	}
	else if (((Y3<0)&&(Y1>0)&&(Y2>0))||((Y3>0)&&(Y1<0)&&(Y2<0))){
		rotateFaceNodes(face2,1);
		y1 = y3;
	}
	
	// assure new node0 on face2 in positive subspace of face1
	if (Y4<0){
		swapFaceNodes(face2 ,1,2);
	}
	
	// assure new node0 on face2 in positive subspace of face1
	if (Y1<0){
		swapFaceNodes(face1,1,2);
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
	if ((Y7<=0)&&(Y8<=0)){ // is i<=l and k<=j, aka do with have interval overlap
		if ((Y9>0)&&(Y10>0)) //k-i-l-j
			return 1;
		else if ((Y9>0)&&(Y10<=0)) //k-i-j-l
			return 2;
		else if ((Y9<=0)&&(Y10>0)) //i-k-l-j
			return 3;
		else if ((Y9<=0)&&(Y10<=0)) //i-k-j-l
			return 4;
	}

	return 0; // no intersection interval overlap
}

void triangleIntersectionPoints(struct FACE* face1, struct FACE* face2){
		
	int mode=classifyTriangleIntersect(face1,face2);
	
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
			edgeLineIntersection(face1->node_array[0],face1->node_array[2],N3,P3,point1); // i 
			edgeLineIntersection(face2->node_array[0],face2->node_array[2],N3,P3,point2); // l
		}
		else if (mode==2){ //k-i-j-l intersection, aka intersection from i to j on both triangles
			edgeLineIntersection(face1->node_array[0],face1->node_array[2],N3,P3,point1); // i 
			edgeLineIntersection(face1->node_array[0],face1->node_array[1],N3,P3,point2); // j 
		}
		else if (mode==3){ //i-k-l-j intersection, aka intersection from k to l on both triangles
			edgeLineIntersection(face2->node_array[0],face2->node_array[1],N3,P3,point1); // k 
			edgeLineIntersection(face2->node_array[0],face2->node_array[2],N3,P3,point2); // l 
		}
		else if (mode==4){ //i-k-j-l intersection, aka intersection from k to j on both triangles
			edgeLineIntersection(face2->node_array[0],face2->node_array[1],N3,P3,point1); // k 
			edgeLineIntersection(face1->node_array[0],face1->node_array[1],N3,P3,point2); // j 
		}
		printf("Triangle intersection occurs between (%f,%f,%f) & (%f,%f,%f)\n",point1[0],point1[1],point1[2],point2[0],point2[1],point2[2]);	
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

