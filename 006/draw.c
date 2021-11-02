#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include "trig.h"

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
    "	gl_Position = vec4((u1.x*(aPos.x-lookAt.x)+u1.y*(aPos.y-lookAt.y)+u1.z*(aPos.z-lookAt.z))*scaling,(u2.x*(aPos.x-lookAt.x)+u2.y*(aPos.y-lookAt.y)+u2.z*(aPos.z-lookAt.z))*scaling,0,1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\0";

void draw(int w, int h, float lookAt[3], float lookFrom[3], int numNodes, int numTriangles, float nodes[][3], int triangles[][3]){

	// view frame axes
	float u3[3]={lookAt[0]-lookFrom[0],lookAt[1]-lookFrom[1],lookAt[2]-lookFrom[2]};
	float u1[3]={-u3[1],u3[0],0};
	float u2[3]={-u3[2]*u3[0],-u3[2]*u3[1],u3[0]*u3[0]+u3[1]*u3[1]};

	// normalize view frame axes
	normalizeVector(u1);
	normalizeVector(u2);
	normalizeVector(u3);

	// open xwindow
	Display* disp=0;
	Window win=0;
	disp=XOpenDisplay(0);
	win=XCreateSimpleWindow(disp,DefaultRootWindow(disp),10,10,w,h,0,0,0);
	static int attribs[]={GLX_RENDER_TYPE,GLX_RGBA_BIT,GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,GLX_DOUBLEBUFFER,true,GLX_RED_SIZE,1,GLX_GREEN_SIZE,1,GLX_BLUE_SIZE,1,None};
	int num_fbc=0;
	GLXFBConfig *fbc=glXChooseFBConfig(disp,DefaultScreen(disp),attribs,&num_fbc);

	glXCreateContextAttribsARBProc glxCreateContextAttribsARB=0;
	glxCreateContextAttribsARB=(glXCreateContextAttribsARBProc) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

	static int context[]={GLX_CONTEXT_MAJOR_VERSION_ARB,4,GLX_CONTEXT_MINOR_VERSION_ARB,2,None};

	GLXContext ctx=glxCreateContextAttribsARB(disp,fbc[0],NULL,true,context);

	XMapWindow(disp,win);
	XStoreName(disp,win,"Viewer");
	glXMakeCurrent(disp,win,ctx);

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

	float vertices[3*numNodes];
	unsigned int indices[3*numTriangles];
	for (int i=0; i<numNodes; i++) {
		vertices[3*i]=nodes[i][0];
		vertices[3*i+1]=nodes[i][1];
		vertices[3*i+2]=nodes[i][2];
	}
	for (int i=0; i<numTriangles; i++) {
		indices[3*i]=triangles[i][0];
		indices[3*i+1]=triangles[i][1];
		indices[3*i+2]=triangles[i][2];
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
	GLint scaling_loc = glGetUniformLocation(shaderProgram,"scaling");
	GLint lookAt_loc = glGetUniformLocation(shaderProgram,"lookAt");
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glUseProgram(shaderProgram); 

	XEvent event;
	XSelectInput(disp,win,ButtonPressMask|ButtonReleaseMask|Button1MotionMask);

	float scaling=0.5f;
	bool left_clicking=0;
	int x_start, y_start;
	float theta_u1, theta_u2, mag;
	float u1_new[3], u2_new[3], u3_new[3];
	float u1_pend[3], u2_pend[3], u3_pend[3];

	u1_new[0]=u1[0];u1_new[1]=u1[1];u1_new[2]=u1[2];
	u2_new[0]=u2[0];u2_new[1]=u2[1];u2_new[2]=u2[2];
	u3_new[0]=u3[0];u3_new[1]=u3[1];u3_new[2]=u3[2];

	/*printf("u1: (%f,%f,%f)\n",u1_new[0],u1_new[1],u1_new[2]);
	printf("u2: (%f,%f,%f)\n",u2_new[0],u2_new[1],u2_new[2]);
	printf("u3: (%f,%f,%f)\n",u3_new[0],u3_new[1],u3_new[2]);*/

	while(1){
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
        
		glUniform3f(u1_loc,u1_new[0],u1_new[1],u1_new[2]);
		glUniform3f(u2_loc,u2_new[0],u2_new[1],u2_new[2]);
		glUniform3f(u3_loc,u3_new[0],u3_new[1],u3_new[2]);
		glUniform1f(scaling_loc,scaling);
		glUniform3f(lookAt_loc,lookAt[0],lookAt[1],lookAt[2]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3*numTriangles, GL_UNSIGNED_INT, 0);
		glXSwapBuffers(disp,win);


		// mouse event handling
		XNextEvent(disp,&event);
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
		if (left_clicking && (event.xmotion.type==MotionNotify)){

			theta_u1=(event.xmotion.y-y_start)/((float) h)*15.0f/3.14159;
			theta_u2=(event.xmotion.x-x_start)/((float) w)*15.0f/3.14159;

			// rotate u3 around u2 by theta_u2
			mag=u2[0]*u3[0]+u2[1]*u3[1]*u2[2]*u3[2];
			u3_new[0]=u3[0]*cos(theta_u2)+(u2[1]*u3[2]-u2[2]*u3[1])*sin(theta_u2)+u2[0]*mag*(1-cos(theta_u2));
			u3_new[1]=u3[1]*cos(theta_u2)+(u2[2]*u3[0]-u2[0]*u3[2])*sin(theta_u2)+u2[1]*mag*(1-cos(theta_u2));
			u3_new[2]=u3[2]*cos(theta_u2)+(u2[0]*u3[1]-u2[1]*u3[0])*sin(theta_u2)+u2[2]*mag*(1-cos(theta_u2));

			// rotate u1 around u2 by theta_u2
			mag=u2[0]*u1[0]+u2[1]*u1[1]*u2[2]*u1[2];
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
			mag=u1_pend[0]*u3_pend[0]+u1_pend[1]*u3_pend[1]*u1_pend[2]*u3_pend[2];
			u3_new[0]=u3_pend[0]*cos(theta_u1)+(u1_pend[1]*u3_pend[2]-u1_pend[2]*u3_pend[1])*sin(theta_u1)+u1_pend[0]*mag*(1-cos(theta_u1));
			u3_new[1]=u3_pend[1]*cos(theta_u1)+(u1_pend[2]*u3_pend[0]-u1_pend[0]*u3_pend[2])*sin(theta_u1)+u1_pend[1]*mag*(1-cos(theta_u1));
			u3_new[2]=u3_pend[2]*cos(theta_u1)+(u1_pend[0]*u3_pend[1]-u1_pend[1]*u3_pend[0])*sin(theta_u1)+u1_pend[2]*mag*(1-cos(theta_u1));

			// rotate u2 around u1 by theta_u1
			mag=u2_pend[0]*u1_pend[0]+u2_pend[1]*u1_pend[1]*u2_pend[2]*u1_pend[2];
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
