struct NODE {
	struct NODE* next;	
	float x;
	float y;
	float z;
};

struct EDGE {
	struct EDGE* next;
	int numNodes;
	struct NODE* node_array[];
};

struct FACE {
	struct FACE* next;
	int numNodes;
	struct NODE* node_array[];
};

struct BODY {
	struct NODE* node;
	struct EDGE* edge;
	struct FACE* face;
};

int nodeArc(float nodes[][3], int start, float x0, float y0, float z0, float theta_0, float theta_1, float r, int n);
int extrudeClosedCurve(float nodes[][3], int triangles[][3], int numNodesCurve, float depth);
void makeClip(float t, float w, float l, float r, float g, int n);

struct BODY* makeRectangularPrism(float x0, float y0, float z0, float l, float w, float h);
void makeNode(struct NODE* node, float x, float y, float z, struct NODE* next);
void makeFace(struct FACE* face, int numNodes, struct NODE* node_array[], struct FACE* next);
void printBodyElements(struct BODY* body);
struct NODE* getClosestNode(struct BODY* body, float x0, float y0, float z0);
