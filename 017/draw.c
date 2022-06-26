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
	struct COLOR* select = (struct COLOR*)malloc(sizeof(struct COLOR));
	struct COLOR* line = (struct COLOR*)malloc(sizeof(struct COLOR));
	struct COLOR* bg   = (struct COLOR*)malloc(sizeof(struct COLOR));

	fill->R=1.0f;
	fill->G=1.0f;
	fill->B=0.1f;
	drawStyle->fillColor=fill;

	select->R=0.1f;
	select->G=1.0f;
	select->B=0.1f;
	drawStyle->selectColor=select;

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
	lookFrom[0]=lookAt[0]+1;
	lookFrom[1]=lookAt[1];
	lookFrom[2]=lookAt[2];
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromPosY(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=lookAt[0];
	lookFrom[1]=lookAt[1]+1;
	lookFrom[2]=lookAt[2];
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromPosZ(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=lookAt[0];
	lookFrom[1]=lookAt[1];
	lookFrom[2]=lookAt[2]+1;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromNegX(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=lookAt[0]-1;
	lookFrom[1]=lookAt[1];
	lookFrom[2]=lookAt[2];
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromNegY(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=lookAt[0];
	lookFrom[1]=lookAt[1]-1;
	lookFrom[2]=lookAt[2];
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewFromNegZ(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=lookAt[0];
	lookFrom[1]=lookAt[1];
	lookFrom[2]=lookAt[2]-1;
	recomputeViewFrame(lookFrom,lookAt);
	return;
}
void viewIsometric(float lookFrom[3], float lookAt[3]){
	lookFrom[0]=lookAt[0]+1;
	lookFrom[1]=lookAt[1]+1;
	lookFrom[2]=lookAt[2]+1;
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


struct FACE* facePicker(struct BODY* body, int h, int w, int x_cursor, int y_cursor, float lookFrom[3], float scaling){

	float x_screenSpace=(x_cursor-((float)w)/2)/(((float)w)/2);
	float y_screenSpace=-(y_cursor-((float)h)/2)/(((float)h)/2);

	float cursor3D[3];
	cursor3D[0]=lookFrom[0]+x_screenSpace*u1_new[0]/scaling+y_screenSpace*u2_new[0]/scaling;
	cursor3D[1]=lookFrom[1]+x_screenSpace*u1_new[1]/scaling+y_screenSpace*u2_new[1]/scaling;
	cursor3D[2]=lookFrom[2]+x_screenSpace*u1_new[2]/scaling+y_screenSpace*u2_new[2]/scaling;

	struct FACE* iter=body->face;
	struct FACE* closest_face=NULL;
	float dist;

	float t;
	float P[3];

	struct NODE* PP = (struct NODE*)malloc(sizeof(struct NODE));
	struct NODE* A; struct NODE* B; struct NODE* C;
	float AB[3], BC[3], CA[3];
	float AP[3], BP[3], CP[3];
	float N1[3], N2[3], N3[3];
	float S1,S2,S3,S4,S5,S6;

	while (iter!=NULL){

		t=(iter->normal[0]*(iter->node_array[0]->x-cursor3D[0])+
			iter->normal[1]*(iter->node_array[0]->y-cursor3D[1])+
			iter->normal[2]*(iter->node_array[0]->z-cursor3D[2]))/
			(u3_new[0]*iter->normal[0]+u3_new[1]*iter->normal[1]+u3_new[2]*iter->normal[2]);
		
		P[0]=cursor3D[0]+t*u3_new[0];
		P[1]=cursor3D[1]+t*u3_new[1];
		P[2]=cursor3D[2]+t*u3_new[2];

		A=iter->node_array[0];
		B=iter->node_array[1];
		C=iter->node_array[2];

		PP->x=P[0];
		PP->y=P[1];
		PP->z=P[2];

		vectorBetweenNodes(A,B,AB);
		vectorBetweenNodes(B,C,BC);
		vectorBetweenNodes(C,A,CA);
		vectorBetweenNodes(A,PP,AP);
		vectorBetweenNodes(B,PP,BP);
		vectorBetweenNodes(C,PP,CP);
		
		cross(AB,iter->normal,N1);
		cross(BC,iter->normal,N2);
		cross(CA,iter->normal,N3);

		S1=dot(AP,N1);
		S2=dot(BP,N1);
		S3=dot(BP,N2);
		S4=dot(CP,N2);
		S5=dot(CP,N3);
		S6=dot(AP,N3);

		if ((S1>0 && S2>0 && S3>0 && S4>0 && S5>0 && S6>0) || 
				(S1<0 && S2<0 && S3<0 && S4<0 && S5<0 && S6<0)){
			//we are inside triangle
			if ((closest_face==NULL) || (dist>t)){				
				closest_face=iter;
				dist=t;
			}
		}
		iter=iter->next;	
	}
	return closest_face;
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
	XSelectInput(disp,win,ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask);

	float scaling=0.5f;
	bool mid_click=0;
	bool control_press=0;
	bool started_panning=0;
	int x_rotating, y_rotating;
	int x_panning, y_panning;
	float theta_u1, theta_u2, mag;
	float u1_pend[3], u2_pend[3], u3_pend[3];
	float lookAt_old[3], lookFrom_old[3];
	float lookMag=pow(pow(lookAt[0]-lookFrom[0],2)+pow(lookAt[1]-lookFrom[1],2)+pow(lookAt[2]-lookFrom[2],2),0.5f);


	u1_new[0]=u1[0];u1_new[1]=u1[1];u1_new[2]=u1[2];
	u2_new[0]=u2[0];u2_new[1]=u2[1];u2_new[2]=u2[2];
	u3_new[0]=u3[0];u3_new[1]=u3[1];u3_new[2]=u3[2];

	Window dummyWin;
	int dummyInt;
	unsigned int dummyUInt;
	int x_cursor,y_cursor;

	struct FACE* selected_face;
	int selected_face_number;
	bool is_face_selected=0;

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
			if (is_face_selected){
				glDrawElements(GL_TRIANGLES, 3*selected_face_number, GL_UNSIGNED_INT, 0);
				glUniform3f(rgb_loc,drawStyle->selectColor->R,drawStyle->selectColor->G,drawStyle->selectColor->B);
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT,(const GLvoid*)(size_t)(12*selected_face_number));
				glUniform3f(rgb_loc,drawStyle->fillColor->R,drawStyle->fillColor->G,drawStyle->fillColor->B);
				glDrawElements(GL_TRIANGLES, 3*(numFaces-selected_face_number-1), GL_UNSIGNED_INT,(const GLvoid*)(size_t)(12*(selected_face_number+1)));
			}
			else{
				glDrawElements(GL_TRIANGLES, 3*numFaces, GL_UNSIGNED_INT, 0);
			}
		}
		
		if ((drawStyle->mode==0) || (drawStyle->mode==2)){
			glDisable(GL_POLYGON_OFFSET_FILL);
			glUniform3f(rgb_loc,drawStyle->lineColor->R,drawStyle->lineColor->G,drawStyle->lineColor->B);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glDrawElements(GL_TRIANGLES, 3*numFaces, GL_UNSIGNED_INT, 0);
		}

		glXSwapBuffers(disp,renderWin);

		// mouse & key event handling
		while(XPending(disp)){
			XNextEvent(disp,&event);
			if ((event.type==KeyPress)&&(event.xkey.keycode==0x25)){
				control_press=1;
				lookFrom_old[0]=lookFrom[0];lookFrom_old[1]=lookFrom[1];lookFrom_old[2]=lookFrom[2];
				lookAt_old[0]=lookAt[0];lookAt_old[1]=lookAt[1];lookAt_old[2]=lookAt[2];
			}
			if ((event.type==KeyRelease)&&(event.xkey.keycode==0x25)){
				control_press=0;
				started_panning=0;
			}
			if (event.xbutton.subwindow==renderWin){	
				if ((event.xbutton.button==Button1) && (event.xbutton.type==ButtonPress)) {
					XQueryPointer(disp,win,&dummyWin,&dummyWin,&dummyInt,&dummyInt,&x_cursor,&y_cursor,&dummyUInt);
					selected_face=facePicker(body,h-20,w,x_cursor,y_cursor-20,lookFrom,scaling);
					selected_face_number=getFaceNumber(body,selected_face);			
					if (selected_face_number>=0){
						is_face_selected=1;
						printf("you just clicked on face #%d\n",selected_face_number);
					}
					else{
						is_face_selected=0;	
					}
				}
				if ((event.xbutton.button==Button2) && (event.xbutton.type==ButtonPress)) {
					mid_click=1;
					x_rotating=event.xbutton.x;
					y_rotating=event.xbutton.y;
				}
				if ((event.xbutton.button==Button2) && (event.xbutton.type==ButtonRelease)) {
					mid_click=0;
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
		}
		
		if (mid_click){
			XQueryPointer(disp,win,&dummyWin,&dummyWin,&dummyInt,&dummyInt,&x_cursor,&y_cursor,&dummyUInt);
			if (control_press){
				if (started_panning){
					lookAt[0]=lookAt_old[0]+(-(x_cursor-x_panning)*u1[0]+(y_cursor-y_panning)*u2[0])/(.5f*w*scaling);
					lookAt[1]=lookAt_old[1]+(-(x_cursor-x_panning)*u1[1]+(y_cursor-y_panning)*u2[1])/(.5f*w*scaling);
					lookAt[2]=lookAt_old[2]+(-(x_cursor-x_panning)*u1[2]+(y_cursor-y_panning)*u2[2])/(.5f*w*scaling);
					lookFrom[0]=lookFrom_old[0]+(-(x_cursor-x_panning)*u1[0]+(y_cursor-y_panning)*u2[0])/(.5f*(h-20)*scaling);
					lookFrom[1]=lookFrom_old[1]+(-(x_cursor-x_panning)*u1[1]+(y_cursor-y_panning)*u2[1])/(.5f*(h-20)*scaling);
					lookFrom[2]=lookFrom_old[2]+(-(x_cursor-x_panning)*u1[2]+(y_cursor-y_panning)*u2[2])/(.5f*(h-20)*scaling);
					x_rotating=x_panning;
					y_rotating=y_panning;
				}
				else {
					x_panning=x_cursor;
					y_panning=y_cursor;
					started_panning=1;
				}

			}
			else{
				theta_u1=(y_cursor-y_rotating)/((float) h)*15.0f/3.14159;
				theta_u2=(x_cursor-x_rotating)/((float) w)*15.0f/3.14159;

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
				
				lookFrom[0]=lookAt[0]-u3_new[0]*lookMag;
				lookFrom[1]=lookAt[1]-u3_new[1]*lookMag;
				lookFrom[2]=lookAt[2]-u3_new[2]*lookMag;

			}
		}
	}

	return;
}
