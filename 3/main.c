#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glext.h>

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

void getNormal(float p1[3], float p2[3], float p3[3], float normal[3]){
	float v1[3]={p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]};
	float v2[3]={p3[0]-p1[0],p3[1]-p1[1],p3[2]-p1[2]};
	normal[0]=v1[1]*v2[2]-v1[2]*v2[1];
	normal[1]=v1[2]*v2[0]-v1[0]*v2[2];
	normal[2]=v1[0]*v2[1]-v1[1]*v2[0];
	return;
}

void writeSTL_binary(int numNodes, int numTriangles, float nodes[][3], int triangles[][3], char* *filename){
	FILE *f=fopen(*filename,"w");
	char header[80]={0};
	char attribute_byte_count[2]={0};
	fwrite(header,1,80,f);
	fwrite(&numTriangles,4,1,f);
	for (int i=0; i<numTriangles; i++){
		float normal[3];
		getNormal(nodes[triangles[i][0]],nodes[triangles[i][1]],nodes[triangles[i][2]],normal);
		fwrite(normal,4,3,f);
		for (int j=0; j<3; j++){
			fwrite(nodes[triangles[i][j]],4,3,f);
		}
		fwrite(attribute_byte_count,1,2,f);
	}
	return;
}

void writeSTL_ASCII(int numNodes, int numTriangles, float nodes[][3], int triangles[][3], char* *filename){
	FILE *f=fopen(*filename,"w");
	fprintf(f,"solid \n");
	for (int i=0; i<numTriangles; i++){
		float normal[3];
		getNormal(nodes[triangles[i][0]],nodes[triangles[i][1]],nodes[triangles[i][2]],normal);
		fprintf(f,"facet normal %f %f %f\n",normal[0],normal[1],normal[2]);
		fprintf(f,"\touter loop\n");
		for (int j=0; j<3; j++){
			fprintf(f,"\t\tvertex %f %f %f\n",nodes[triangles[i][j]][0],nodes[triangles[i][j]][1],nodes[triangles[i][j]][2]);
		}
		fprintf(f,"\tendloop\n");
	}
	fprintf(f,"endsolid \n");
	return;
}

void draw(int w, int h, float lookAt[3], float lookFrom[3], int numNodes, int numTriangles, float nodes[][3], int triangles[][3]){

	// view frame axes
	float u3[3]={lookAt[0]-lookFrom[0],lookAt[1]-lookFrom[1],lookAt[2]-lookFrom[2]};
	float u1[3]={-u3[1],u3[0],0};
	float u2[3]={-u3[2]*u3[0],-u3[2]*u3[1],u3[0]*u3[0]+u3[1]*u3[1]};

	// normalize view frame axes
	float mag_u1=pow(u1[0]*u1[0]+u1[1]*u1[1]+u1[2]*u1[2],.5);
	float mag_u2=pow(u2[0]*u2[0]+u2[1]*u2[1]+u2[2]*u2[2],.5);
	float mag_u3=pow(u3[0]*u3[0]+u3[1]*u3[1]+u3[2]*u3[2],.5);
	u1[0]=u1[0]/mag_u1;	u1[1]=u1[1]/mag_u1;	u1[2]=u1[2]/mag_u1;
	u2[0]=u2[0]/mag_u2;	u2[1]=u2[1]/mag_u2;	u2[2]=u2[2]/mag_u2;
	u3[0]=u3[0]/mag_u3;	u3[1]=u3[1]/mag_u3;	u3[2]=u3[2]/mag_u3;

	printf("look from: %f %f %f\n",lookFrom[0],lookFrom[1],lookFrom[2]);
	printf("look at: %f %f %f\n",lookAt[0],lookAt[1],lookAt[2]);
	printf("u1: %f %f %f\n",u1[0],u1[1],u1[2]);
	printf("u2: %f %f %f\n",u2[0],u2[1],u2[2]);
	printf("u3: %f %f %f\n",u3[0],u3[1],u3[2]);

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

    glUseProgram(shaderProgram); // out of loop or....?

	while(1){
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
        
		glUniform3f(u1_loc,u1[0],u1[1],u1[2]);
		glUniform3f(u2_loc,u2[0],u2[1],u2[2]);
		glUniform3f(u3_loc,u3[0],u3[1],u3[2]);
		glUniform1f(scaling_loc,.5);
		glUniform3f(lookAt_loc,lookAt[0],lookAt[1],lookAt[2]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3*numTriangles, GL_UNSIGNED_INT, 0);
		glXSwapBuffers(disp,win);
	};

	return;
}


int main(){
	float nodes[9][3]={{0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,0,1},{1,0,1},{1,1,1},{0,1,1},{.5,.5,1.5}};
	int triangles[14][3]={{0,1,5},{0,5,4},{1,2,6},{1,6,5},{2,3,7},{2,7,6},{3,0,4},{3,4,7},{4,5,8},{5,6,8},{6,7,8},{7,4,8},{0,3,2},{0,2,1}};
	char* filename="house.stl";
	//writeSTL_ASCII(9,14,nodes,triangles,&filename);
	writeSTL_binary(9,14,nodes,triangles,&filename);
	
	float lookAt[3]={.5,0,.5};
	float lookFrom[3]={.5,-5,.5};
	draw(800,600,lookAt,lookFrom,9,14,nodes,triangles);

	return 0;
}
