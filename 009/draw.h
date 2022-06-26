#pragma once
void draw(int w, int h, float lookAt[3], float lookFrom[3], int numNodes, int numTriangles, float nodes[][3], int triangles[][3]);
void drawBody(int w, int h, float lookAt[3], float lookFrom[3], struct BODY* body);
