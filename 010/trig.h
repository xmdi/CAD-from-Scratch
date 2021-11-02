#pragma once
void getNormal(float p1[3], float p2[3], float p3[3], float normal[3]);
void normalizeVector(float vec[3]);
float dot(float x[3], float y[3]);
void cross(float x[3], float y[3], float out[3]);
void vectorBetweenNodes(struct NODE* tail, struct NODE* head, float out[3]);
float getAngle(struct NODE* node1, struct NODE* node2, struct NODE* node3, float normal[3]);
