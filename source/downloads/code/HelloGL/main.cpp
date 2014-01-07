#include <iostream>
using namespace std;

/*
 * Installed by building glew source
 * Also need to be included first because it defines all "gl" stuffs
 *
 * About glew, normally you don't require glew on MacOS due the way how the
 * OpenGL layer is implemented in MacOS. You are restricted to the OpenGL
 * features MacOS provides/exposes. So either the features are available via
 * the headers of MacOS or not. There header would be #include <OpenGL/gl3.h>
 * where also the naming is missleading, it does not mean only OpenGL 3,
 * it is the same like with the context.
 */
//#include <GL/glew.h>

#include "LoadShaders.h"
#include <GLUT/GLUT.h>

#define BUFFER_OFFSET(x)	((const void*)(x))

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

const GLuint NumVertices = 6;

void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	
	GLfloat vertices[NumVertices][2] = {
		{ -0.90, -0.90 },
		{  0.85, -0.90 },
		{ -0.90,  0.85 },
		{  0.90, -0.85 },
		{  0.90,  0.90 },
		{ -0.85,  0.90 }
	};
	
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
		     vertices, GL_STATIC_DRAW);
	
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "simple.vert" },
		{ GL_FRAGMENT_SHADER, "simple.frag" },
		{ GL_NONE, NULL }
	};
	
	GLuint program = LoadShaders(shaders);
	glUseProgram(program);
	
	glVertexAttribPointer(vPosition, 2, GL_FLOAT,
			      GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	
	glFlush();
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow(argv[0]);
	
	/*
	 * Normally don't need GLEW on MacOSX.
	 * But set glewExperimental to TRUE if you want to use it.
	 * Otherwise some funcs like "glGenVertexArray" will be NULL.
	 */
	/* glewExperimental = GL_TRUE;
	if (glewInit()) {
		cerr << "Unable to initialize GLEW... exiting" << endl;
		exit(EXIT_FAILURE);
	} */
	
	cout << "Supported GLSL version is: " <<
		glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	
	init();
	
	glutDisplayFunc(display);
	
	glutMainLoop();
}