#pragma once
struct COLOR{
	float R; // 0 to 1
	float G;
	float B;
};

struct DRAWSTYLE{
	struct COLOR* fillColor;
	struct COLOR* lineColor;
	struct COLOR* bgColor;
	char mode; // 0=lines only, 1=fills only, 2=both
	float lineWidth;
};

struct DRAWSTYLE* defaultDrawstyle(void);
void drawBody(int w, int h, float lookAt[3], float lookFrom[3], struct BODY* body, struct DRAWSTYLE* drawStyle);
