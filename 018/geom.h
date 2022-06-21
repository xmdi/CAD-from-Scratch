#pragma once
struct NODE {
	struct NODE* next;	
	float x;
	float y;
	float z;
};

struct EDGE {
	struct EDGE* next;
	int numNodes;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	struct NODE* node_array[];
};

struct FACE {
	struct FACE* next;
	int numNodes;
	float normal[3];
	float xmin, xmax, ymin, ymax, zmin, zmax;
	struct FLOATARRAYLIST* constraint_edges; // episode 18
	struct NODE* node_array[];
};

struct BODY {
	struct NODE* node;
	struct EDGE* edge;
	struct FACE* face;
	float xmin, xmax, ymin, ymax, zmin, zmax;
};

struct ARRAYLIST {
	struct ARRAYLIST* next;
	int numVals;
	int array[];
}; 

// episode 18
struct FLOATARRAYLIST {
	struct FLOATARRAYLIST* next;
	int numVals;
	float array[];
}; 

struct CSG_OPERATION {
	int operation; // 1 = union, 2 = intersection, 3 = difference
	struct BODY* bodyA;
	struct BODY* bodyB;
};
// episode 18


int nodeArc(float nodes[][3], int start, float x0, float y0, float z0, float theta_0, float theta_1, float r, int n);
int extrudeClosedCurve(float nodes[][3], int triangles[][3], int numNodesCurve, float depth);
void makeClip(float t, float w, float l, float r, float g, int n);

struct BODY* makeRectangularPrism(float x0, float y0, float z0, float l, float w, float h);
void makeNode(struct NODE* node, float x, float y, float z, struct NODE* next);
void makeFace(struct FACE* face, int numNodes, float normal[3], struct NODE* node_array[], struct FACE* next);
void printBodyElements(struct BODY* body);
struct NODE* getClosestNode(struct BODY* body, float x0, float y0, float z0);
void removeNodeFromFace(struct FACE* face, struct NODE* node);
void replaceFaceFromBody(struct BODY* body, struct FACE* face, struct FACE* new_faces);
void triangulateFace(struct BODY* body, struct FACE* face);
void triangulateBody(struct BODY* body);
struct FACE* makePolygonFace(int numPoints, float normal[3], float points[][3]);
int countNodes(struct BODY* body);
int countFaces(struct BODY* body);
int getFaceNumber(struct BODY* body, struct FACE* face);
struct BODY* makeBodyFromFace(struct FACE* face);
void getBodyNodalCentroid(struct BODY* body, float centroid[3]);
int classifyTriangleIntersect(struct FACE* face1, struct FACE* face2);
void triangleIntersectionPoints(struct FACE* face1, struct FACE* face2);
void retriangulateFaceWithConstraintEdges(struct BODY* body, struct FACE* face);
struct BODY* evaluateCSG(int operation, struct BODY* bodyA, struct BODY* bodyB);
