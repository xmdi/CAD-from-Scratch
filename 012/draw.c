#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

typedef GLXContext (*glXCreateContextAttribsARBProc)
	(Display*, GLXFBConfig, GLXContext, Bool, const int*);

const char *vertexShaderSource = "#version 330 core\n"
    "in vec3 aPos;\n"
	"uniform vec3 u1;\n"
    "uniform vec3 u2;\n"
    "uniform vec3 u3;\n"
    "uniform float scaling;\n"
    "uniform vec3 lookAt;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = vec4((u1.x*(aPos.x-lookAt.x)+u1.y*(aPos.y-lookAt.y)+u1.z*(aPos.z-lookAt.z))*scaling,(u2.x*(aPos.x-lookAt.x)+u2.y*(aPos.y-lookAt.y)+u2.z*(aPos.z-lookAt.z))*scaling,(u3.x*(aPos.x-lookAt.x)+u3.y*(aPos.y-lookAt.y)+u3.z*(aPos.z-lookAt.z))*scaling,1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "uniform vec3 rgb;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(rgb.x,rgb.y,rgb.z,1.0f);\n"
    "}\0";

float u1_new[3],u2_new[3],u3_new[3];

struct DRAWSTYLE* defaultDrawstyle(){
	struct DRAWSTYLE* drawStyle = (struct DRAWSTYLE*)malloc(sizeof(struct DRAWSTYLE));
	struct COLOR* fill = (struct COLOR*)malloc(sizeof(struct COLOR));
	struct COLOR* line = (struct COLOR*)malloc(sizeof(struct COLOR));
	struct COLOR* bg   = (struct COLOR*)malloc(sizeof(struct COLOR));

	fill->R=1.0f;
	fill->G=1.0f;
	fill->B=0.1f;
	drawStyle->fillColor=fill;

	line->R=1.0f;
	line->G=0.1f;
	line->B=0.1f;
	drawStyle->lineColor=line;
	
	bg->R=0.1f;
	bg->G=0.1f;
	bg->B=0.1f;
	drawStyle->bgColor=bg;

	drawStyle->mode=2; // both lines and fills

	drawStyle->lineWidth=3.0f;

	return drawStyle;
}

int RGB(unsigned char r,unsigned char g, unsigned char b){
	return (r<<16)+(g<<8)+(b);
}

void printNodeCount(struct BODY* body){
	printf("QUERY -> COUNT NODES = %d\n",countNodes(body));
	return;
}

void printFaceCount(struct BODY* body){
	printf("QUERY -> COUNT FACES = %d\n",countFaces(body));
	return;
}

void recomputeViewFrame(float lookFrom[3],float lookAt[3]){

	// view frame axes
	float u1[3],u2[3];
	float u3[3]={lookAt[0]-lookFrom[0],lookAt[1]-lookFrom[1],lookAt[2]-lookFrom[2]};	
	if (u3[0]||u3[1]){
		u1[0]=-u3[1];u1[1]=u3[0];u1[2]=0;
		u2[0]=-u3[2]*u3[0];u2[1]=-u3[2]*u3[1];u2[2]=u3[0]*u3[0]+u3[1]*u3[1];
	}
	else{
		u1[0]=1;u1[1]=0;u1[2]=0;
		u2[0]=0;u2[1]=u3[2];u2[2]=0;
	}
	// normalize view frame axes
	normalizeVector(u1);
	normalizeVector(u2);
	normalizeVector(u3);

	u1_new[0]=u1[0];u1_new[1]=u1[1];u1_new[2]=u1[2];
	u2_new[0]=u2[0];u2_new[1]=u2[1];u2_new[2]=u2[2];
	u3_new[0]=u3[0];u3_new[1]=u3[1];u3_new[2]=u3[2];

	return;
}

void viewFromPosX(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=1;
	lookFrom[1]=0;
	lookFrom[2]=0;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromPosY(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=0;
	lookFrom[1]=1;
	lookFrom[2]=0;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromPosZ(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=0;
	lookFrom[1]=0;
	lookFrom[2]=1;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromNegX(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=-1;
	lookFrom[1]=0;
	lookFrom[2]=0;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromNegY(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=0;
	lookFrom[1]=-1;
	lookFrom[2]=0;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromNegZ(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=0;
	lookFrom[1]=0;
	lookFrom[2]=-1;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewIsometric(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=1;
	lookFrom[1]=1;
	lookFrom[2]=1;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}

void populateSubButtons(struct MENUBUTTON* menuButton, struct MENUBUTTON* subButtons[]){
	for (int i=0;i<menuButton->numSubButtons;i++)
		menuButton->subButtons[i]=subButtons[i];
	return;
}

struct MENUBUTTON* createMenuButton(int width,int height,int dx,int dy,
									int RGB_background,int RGB_border,int RGB_font,
									int numSubButtons,int showChildren,
									char* text, int nchars, Font font,
									Display* disp, Window win,
									int whichFunc, void (*func)){
		struct MENUBUTTON* menuButton=(struct MENUBUTTON*)malloc(7*sizeof(int)+sizeof(XTextItem)+sizeof(Window)+
								3*sizeof(GC)+sizeof(void*)+numSubButtons*sizeof(struct MENUBUTTON*));
		menuButton->width=width;
		menuButton->height=height;
		menuButton->dx=dx;
		menuButton->dy=dy;
		menuButton->numSubButtons=numSubButtons;
		menuButton->whichFunc=whichFunc;
		if (whichFunc==1)
			menuButton->func.bodyFunc=func;
		else if(whichFunc==2)
			menuButton->func.viewFunc=func;
		else
			menuButton->func.bodyFunc=NULL;

		XTextItem textItem={text,nchars,0,font};
		menuButton->textItem=textItem;
		
		Window window=XCreateSimpleWindow(disp,win,dx,dy,width,height,0,RGB_border,RGB_background);
		menuButton->window=window;
		
		GC gcFont=XCreateGC(disp,menuButton->window,0,0);
		GC gcBord=XCreateGC(disp,menuButton->window,0,0);
		GC gcFill=XCreateGC(disp,menuButton->window,0,0);
		XSetForeground(disp,gcFont,RGB_font);
		XSetForeground(disp,gcBord,RGB_border);
		XSetForeground(disp,gcFill,RGB_background);
		menuButton->gcFont=gcFont;
		menuButton->gcBord=gcBord;
		menuButton->gcFill=gcFill;

		return menuButton;
}

struct MENUBUTTON* populateMenuItems(Display* disp, Window win, Font font){
	struct MENUBUTTON* mainMenu=createMenuButton(1000,20,0,0,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),2,1,"",0,font,disp,win,0,NULL);
	struct MENUBUTTON* queryButton=createMenuButton(80,20,0,0,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),2,0,"Query",5,font,disp,win,0,NULL);// maybe change window
	struct MENUBUTTON* viewButton=createMenuButton(80,20,80,0,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),7,0,"View",4,font,disp,win,0,NULL);
	struct MENUBUTTON* menuItems[2]={queryButton,viewButton};
	populateSubButtons(mainMenu,menuItems);

	void (*func1)(struct BODY*)=&printNodeCount;
	void (*func2)(struct BODY*)=&printFaceCount;
	struct MENUBUTTON* countNodes=createMenuButton(120,20,0,20,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"Count Nodes",11,font,disp,win,1,*func1);
	struct MENUBUTTON* countFaces=createMenuButton(120,20,0,40,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"Count Faces",11,font,disp,win,1,*func2);
	struct MENUBUTTON* queryItems[2]={countNodes,countFaces};
	populateSubButtons(queryButton,queryItems);

	void (*func3)(float[3],float[3])=&viewFromPosX;
	void (*func4)(float[3],float[3])=&viewFromPosY;
	void (*func5)(float[3],float[3])=&viewFromPosZ;
	void (*func6)(float[3],float[3])=&viewFromNegX;
	void (*func7)(float[3],float[3])=&viewFromNegY;
	void (*func8)(float[3],float[3])=&viewFromNegZ;
	void (*func9)(float[3],float[3])=&viewIsometric;
	struct MENUBUTTON* viewPosX=createMenuButton(120,20,80,20,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"From (+X)",9,font,disp,win,2,*func3);
	struct MENUBUTTON* viewPosY=createMenuButton(120,20,80,40,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"From (+Y)",9,font,disp,win,2,*func4);
	struct MENUBUTTON* viewPosZ=createMenuButton(120,20,80,60,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"From (+Z)",9,font,disp,win,2,*func5);
	struct MENUBUTTON* viewNegX=createMenuButton(120,20,80,80,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"From (-X)",9,font,disp,win,2,*func6);
	struct MENUBUTTON* viewNegY=createMenuButton(120,20,80,100,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"From (-Y)",9,font,disp,win,2,*func7);
	struct MENUBUTTON* viewNegZ=createMenuButton(120,20,80,120,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"From (-Z)",9,font,disp,win,2,*func8);
	struct MENUBUTTON* viewIso =createMenuButton(120,20,80,140,RGB(230,230,230),RGB(0,0,0),RGB(0,0,0),0,0,"Isometric",9,font,disp,win,2,*func9);
	struct MENUBUTTON* viewItems[7]={viewPosX,viewPosY,viewPosZ,viewNegX,viewNegY,viewNegZ,viewIso};
	populateSubButtons(viewButton,viewItems);

	return mainMenu;
}

void drawBody(int w, int h, float lookAt[3], float lookFrom[3], struct BODY* body, struct DRAWSTYLE* drawStyle){

	// view frame axes
	float u1[3],u2[3];
	float u3[3]={lookAt[0]-lookFrom[0],lookAt[1]-lookFrom[1],lookAt[2]-lookFrom[2]};	
	if (u3[0]||u3[1]){
		u1[0]=-u3[1];u1[1]=u3[0];u1[2]=0;
		u2[0]=-u3[2]*u3[0];u2[1]=-u3[2]*u3[1];u2[2]=u3[0]*u3[0]+u3[1]*u3[1];
	}
	else{
		u1[0]=1;u1[1]=0;u1[2]=0;
		u2[0]=0;u2[1]=u3[2];u2[2]=0;
	}
	
	// normalize view frame axes
	normalizeVector(u1);
	normalizeVector(u2);
	normalizeVector(u3);

	// open xwindow
	Display* disp=0;
	Window win=0;
	Window renderWin=0;
	disp=XOpenDisplay(0);
	win=XCreateSimpleWindow(disp,DefaultRootWindow(disp),10,10,w,h,0,RGB(0,0,0),RGB(255,255,255));
	renderWin=XCreateSimpleWindow(disp,win,0,20,w,h-20,0,RGB(0,0,0),RGB(255,255,255));

	static int attribs[]={GLX_RENDER_TYPE,GLX_RGBA_BIT,GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,GLX_DOUBLEBUFFER,true,GLX_RED_SIZE,1,GLX_GREEN_SIZE,1,GLX_BLUE_SIZE,1,GLX_DEPTH_SIZE,24,None};
	int num_fbc=0;
	GLXFBConfig *fbc=glXChooseFBConfig(disp,DefaultScreen(disp),attribs,&num_fbc);

	glXCreateContextAttribsARBProc glxCreateContextAttribsARB=0;
	glxCreateContextAttribsARB=(glXCreateContextAttribsARBProc) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

	static int context[]={GLX_CONTEXT_MAJOR_VERSION_ARB,4,GLX_CONTEXT_MINOR_VERSION_ARB,2,None};

	GLXContext ctx=glxCreateContextAttribsARB(disp,fbc[0],NULL,true,context);

	XMapWindow(disp,win);
	XMapWindow(disp,renderWin);

	XStoreName(disp,win,"Viewer");
	glXMakeCurrent(disp,renderWin,ctx);

	XFontStruct *fontStruct=XLoadQueryFont(disp,"lucidasans-12");
	struct MENUBUTTON* menuButton=populateMenuItems(disp,win,fontStruct->fid);
	XMapWindow(disp,menuButton->window);

	for (int i=0;i<menuButton->numSubButtons;i++){
		XMapWindow(disp,menuButton->subButtons[i]->window);
		XFillRectangle(disp,menuButton->subButtons[i]->window,menuButton->subButtons[i]->gcFill,0,0,menuButton->subButtons[i]->width,menuButton->subButtons[i]->height);
		XDrawRectangle(disp,menuButton->subButtons[i]->window,menuButton->subButtons[i]->gcBord,0,0,menuButton->subButtons[i]->width-1,menuButton->subButtons[i]->height);
		XDrawText(disp,menuButton->subButtons[i]->window,menuButton->subButtons[i]->gcFont,8,15,&menuButton->subButtons[i]->textItem,1);
	}

	// compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    char infoLog[512];
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n",infoLog);
    }
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	int numNodes = countNodes(body);
	int numFaces = countFaces(body);

	float vertices[3*numNodes];
	unsigned int indices[3*numFaces];
			
	struct FACE* iter_face=body->face;
	struct NODE* iter_node=NULL;

	int k=0,l=0;

	while (iter_face!=NULL){ // loop over the faces in body to populate the indices vector
		if (iter_face->numNodes==3){ // if the face is a triangle
			for (int i=0;i<3;i++){ // loop over all points in face
				iter_node=body->node;
				for (int j=0;j<numNodes;j++){ // loop thru the nodes in the body, until we have the ID of that given node
					if (iter_face->node_array[i]==iter_node){
						indices[k++]=j;
						break;
					}
					iter_node=iter_node->next;
				}
			}
		}
		else{ // print error
			printf("OOPS! Nontriangular face in drawBody(...BODY...).\n");
		}
		iter_face=iter_face->next;
	}
	
	iter_node=body->node;

	while (iter_node!=NULL){
		vertices[l++]=iter_node->x;
		vertices[l++]=iter_node->y;
		vertices[l++]=iter_node->z;
		iter_node=iter_node->next;
	}

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	GLint u1_loc = glGetUniformLocation(shaderProgram,"u1");
	GLint u2_loc = glGetUniformLocation(shaderProgram,"u2");
	GLint u3_loc = glGetUniformLocation(shaderProgram,"u3");
	GLint rgb_loc = glGetUniformLocation(shaderProgram,"rgb");
	GLint scaling_loc = glGetUniformLocation(shaderProgram,"scaling");
	GLint lookAt_loc = glGetUniformLocation(shaderProgram,"lookAt");
	
	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	//glDepthRange(-5.0f,1.0f);

	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CW);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glPolygonOffset(1.0f,1.0f);

	if ((drawStyle->mode==0) || (drawStyle->mode==2))
		glLineWidth(drawStyle->lineWidth);

    glUseProgram(shaderProgram); 

	XEvent event;
	XSelectInput(disp,win,ButtonPressMask|ButtonReleaseMask|Button1MotionMask);

	float scaling=0.5f;
	bool left_clicking=0;
	int x_start, y_start;
	float theta_u1, theta_u2, mag;
	//float u1_new[3], u2_new[3], u3_new[3];
	float u1_pend[3], u2_pend[3], u3_pend[3];

	u1_new[0]=u1[0];u1_new[1]=u1[1];u1_new[2]=u1[2];
	u2_new[0]=u2[0];u2_new[1]=u2[1];u2_new[2]=u2[2];
	u3_new[0]=u3[0];u3_new[1]=u3[1];u3_new[2]=u3[2];

	/*printf("u1: (%f,%f,%f)\n",u1_new[0],u1_new[1],u1_new[2]);
	printf("u2: (%f,%f,%f)\n",u2_new[0],u2_new[1],u2_new[2]);
	printf("u3: (%f,%f,%f)\n",u3_new[0],u3_new[1],u3_new[2]);*/

	while(1){
		glClearColor(drawStyle->bgColor->R,drawStyle->bgColor->G,drawStyle->bgColor->B,1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   
		glUniform3f(u1_loc,u1_new[0],u1_new[1],u1_new[2]);
		glUniform3f(u2_loc,u2_new[0],u2_new[1],u2_new[2]);
		glUniform3f(u3_loc,u3_new[0],u3_new[1],u3_new[2]);
		glUniform1f(scaling_loc,scaling);
		glUniform3f(lookAt_loc,lookAt[0],lookAt[1],lookAt[2]);
		glBindVertexArray(VAO);
		
		if ((drawStyle->mode==1) || (drawStyle->mode==2)){
			glEnable(GL_POLYGON_OFFSET_FILL);
			glUniform3f(rgb_loc,drawStyle->fillColor->R,drawStyle->fillColor->G,drawStyle->fillColor->B);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glDrawElements(GL_TRIANGLES, 3*numFaces, GL_UNSIGNED_INT, 0);
		}

		if ((drawStyle->mode==0) || (drawStyle->mode==2)){
			glDisable(GL_POLYGON_OFFSET_FILL);
			glUniform3f(rgb_loc,drawStyle->lineColor->R,drawStyle->lineColor->G,drawStyle->lineColor->B);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glDrawElements(GL_TRIANGLES, 3*numFaces, GL_UNSIGNED_INT, 0);
		}

		glXSwapBuffers(disp,renderWin);

		// mouse event handling
		XNextEvent(disp,&event);
		
		if (event.xbutton.subwindow==renderWin){		
			if ((event.xbutton.button==Button1) && (event.xbutton.type==ButtonPress)) {
				left_clicking=1;
				x_start=event.xbutton.x;
				y_start=event.xbutton.y;
			}
			if ((event.xbutton.button==Button1) && (event.xbutton.type==ButtonRelease)) {
				left_clicking=0;
				u1[0]=u1_new[0];u1[1]=u1_new[1];u1[2]=u1_new[2];
				u2[0]=u2_new[0];u2[1]=u2_new[1];u2[2]=u2_new[2];
				u3[0]=u3_new[0];u3[1]=u3_new[1];u3[2]=u3_new[2];
			}
			if ((event.xbutton.button==Button4) && (event.xbutton.type==ButtonPress)){
				scaling=scaling*0.9f;
			}
			if ((event.xbutton.button==Button5) && (event.xbutton.type==ButtonPress)){
				scaling=scaling/0.9f;
			}
		}
		else{
			for (int i=0;i<menuButton->numSubButtons;i++){
				if ((event.xbutton.subwindow==menuButton->subButtons[i]->window) && (event.xbutton.button==Button1) && (event.xbutton.type==ButtonPress)){
					if (menuButton->subButtons[i]->showChildren){
						for (int j=0;j<menuButton->subButtons[i]->numSubButtons;j++){
							XUnmapWindow(disp,menuButton->subButtons[i]->subButtons[j]->window);			
							menuButton->subButtons[i]->showChildren=0;
						}
					}
					else {
						for (int j=0;j<menuButton->subButtons[i]->numSubButtons;j++){
							XMapWindow(disp,menuButton->subButtons[i]->subButtons[j]->window);			
							XFillRectangle(disp,menuButton->subButtons[i]->subButtons[j]->window,menuButton->subButtons[i]->subButtons[j]->gcFill,0,0,menuButton->subButtons[i]->subButtons[j]->width,menuButton->subButtons[i]->subButtons[j]->height);
							XDrawRectangle(disp,menuButton->subButtons[i]->subButtons[j]->window,menuButton->subButtons[i]->subButtons[j]->gcBord,0,0,menuButton->subButtons[i]->subButtons[j]->width-1,menuButton->subButtons[i]->subButtons[j]->height);
							XDrawText(disp,menuButton->subButtons[i]->subButtons[j]->window,menuButton->subButtons[i]->subButtons[j]->gcFont,8,15,&menuButton->subButtons[i]->subButtons[j]->textItem,1);
							menuButton->subButtons[i]->showChildren=1;
						}
					}
					break;
				}
				for (int j=0;j<menuButton->subButtons[i]->numSubButtons;j++){
					if ((event.xbutton.subwindow==menuButton->subButtons[i]->subButtons[j]->window) && (event.xbutton.button==Button1) && (event.xbutton.type==ButtonPress)){
						if (menuButton->subButtons[i]->subButtons[j]->whichFunc==1){
							menuButton->subButtons[i]->subButtons[j]->func.bodyFunc(body);	
						}
						else if (menuButton->subButtons[i]->subButtons[j]->whichFunc==2){
							menuButton->subButtons[i]->subButtons[j]->func.viewFunc(lookFrom,lookAt);	
							u1[0]=u1_new[0];u1[1]=u1_new[1];u1[2]=u1_new[2];
							u2[0]=u2_new[0];u2[1]=u2_new[1];u2[2]=u2_new[2];
							u3[0]=u3_new[0];u3[1]=u3_new[1];u3[2]=u3_new[2];
						}
						break;
					}
				}
			}
		}

		if (left_clicking && (event.xmotion.type==MotionNotify)){

			theta_u1=(event.xmotion.y-y_start)/((float) h)*15.0f/3.14159;
			theta_u2=(event.xmotion.x-x_start)/((float) w)*15.0f/3.14159;

			// rotate u3 around u2 by theta_u2
			mag=u2[0]*u3[0]+u2[1]*u3[1]+u2[2]*u3[2];
			u3_new[0]=u3[0]*cos(theta_u2)+(u2[1]*u3[2]-u2[2]*u3[1])*sin(theta_u2)+u2[0]*mag*(1-cos(theta_u2));
			u3_new[1]=u3[1]*cos(theta_u2)+(u2[2]*u3[0]-u2[0]*u3[2])*sin(theta_u2)+u2[1]*mag*(1-cos(theta_u2));
			u3_new[2]=u3[2]*cos(theta_u2)+(u2[0]*u3[1]-u2[1]*u3[0])*sin(theta_u2)+u2[2]*mag*(1-cos(theta_u2));

			// rotate u1 around u2 by theta_u2
			mag=u2[0]*u1[0]+u2[1]*u1[1]+u2[2]*u1[2];
			u1_new[0]=u1[0]*cos(theta_u2)+(u2[1]*u1[2]-u2[2]*u1[1])*sin(theta_u2)+u2[0]*mag*(1-cos(theta_u2));
			u1_new[1]=u1[1]*cos(theta_u2)+(u2[2]*u1[0]-u2[0]*u1[2])*sin(theta_u2)+u2[1]*mag*(1-cos(theta_u2));
			u1_new[2]=u1[2]*cos(theta_u2)+(u2[0]*u1[1]-u2[1]*u1[0])*sin(theta_u2)+u2[2]*mag*(1-cos(theta_u2));

			// normalize
			normalizeVector(u1_new);
			normalizeVector(u3_new);

			u1_pend[0]=u1_new[0];u1_pend[1]=u1_new[1];u1_pend[2]=u1_new[2];
			u2_pend[0]=u2[0];u2_pend[1]=u2[1];u2_pend[2]=u2[2];
			u3_pend[0]=u3_new[0];u3_pend[1]=u3_new[1];u3_pend[2]=u3_new[2];

			// rotate u3 around u1 by theta_u1
			mag=u1_pend[0]*u3_pend[0]+u1_pend[1]*u3_pend[1]+u1_pend[2]*u3_pend[2];
			u3_new[0]=u3_pend[0]*cos(theta_u1)+(u1_pend[1]*u3_pend[2]-u1_pend[2]*u3_pend[1])*sin(theta_u1)+u1_pend[0]*mag*(1-cos(theta_u1));
			u3_new[1]=u3_pend[1]*cos(theta_u1)+(u1_pend[2]*u3_pend[0]-u1_pend[0]*u3_pend[2])*sin(theta_u1)+u1_pend[1]*mag*(1-cos(theta_u1));
			u3_new[2]=u3_pend[2]*cos(theta_u1)+(u1_pend[0]*u3_pend[1]-u1_pend[1]*u3_pend[0])*sin(theta_u1)+u1_pend[2]*mag*(1-cos(theta_u1));

			// rotate u2 around u1 by theta_u1
			mag=u2_pend[0]*u1_pend[0]+u2_pend[1]*u1_pend[1]+u2_pend[2]*u1_pend[2];
			u2_new[0]=u2_pend[0]*cos(theta_u1)+(u1_pend[1]*u2_pend[2]-u1_pend[2]*u2_pend[1])*sin(theta_u1)+u2_pend[0]*mag*(1-cos(theta_u1));
			u2_new[1]=u2_pend[1]*cos(theta_u1)+(u1_pend[2]*u2_pend[0]-u1_pend[0]*u2_pend[2])*sin(theta_u1)+u2_pend[1]*mag*(1-cos(theta_u1));
			u2_new[2]=u2_pend[2]*cos(theta_u1)+(u1_pend[0]*u2_pend[1]-u1_pend[1]*u2_pend[0])*sin(theta_u1)+u2_pend[2]*mag*(1-cos(theta_u1));

			// normalize
			normalizeVector(u2_new);
			normalizeVector(u3_new);


		}
	
	}

	return;
}
