#pragma once
#include <X11/Xlib.h>

struct COLOR{
	float R; // 0 to 1
	float G;
	float B;
};

struct DRAWSTYLE{
	struct COLOR* fillColor;
	struct COLOR* selectColor;
	struct COLOR* lineColor;
	struct COLOR* bgColor;
	char mode; // 0=lines only, 1=fills only, 2=both
	float lineWidth;
};

struct MENUBUTTON{
	int height;
	int width;
	int dx;
	int dy;
	int numSubButtons;
	int showChildren;
	XTextItem textItem;
	Window window;
	GC gcFill;
	GC gcBord;
	GC gcFont;
	int whichFunc;
	union {
		void (*bodyFunc)(struct BODY*);
		void (*viewFunc)(float[3],float[3]);
	} func;
	struct MENUBUTTON* subButtons[];
};

struct DRAWSTYLE* defaultDrawstyle(void);
void drawBody(int w, int h, float lookAt[3], float lookFrom[3], struct BODY* body, struct DRAWSTYLE* drawStyle);
