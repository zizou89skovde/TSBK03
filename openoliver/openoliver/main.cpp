
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include "common/GL_utilities.h"
#include <direct.h>
#define GetCurrentDir _getcwd
// initial width and heights
#define W 512
#define H 512

void OnTimer(int value);

//----------------------Globals-------------------------------------------------
GLuint phongshader = 0;
//-------------------------------------------------------------------------------------

void init(void)
{
	//dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	//printError("GL inits");
	char DIR[100];
	GetCurrentDir(DIR, 100);
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	printf("Current directory %s\n", DIR);
	// Load and compile shaders
	//phongshader = loadShaders("../openoliver/shaders/phong.vert", "../openoliver/shaders/phong.frag");  // renders with light (used for initial renderin of teapot)
	//phongshader = loadShaders("C:\\Users\Jonsson\Documents\Visual Studio 2013\Projects\openoliver\openoliver\phong.vert", "C:\\Users\Jonsson\Documents\Visual Studio 2013\Projects\openoliver\openoliver\phong.frag");  // renders with light (used for initial renderin of teapot)
	phongshader = loadShaders("phong.vert", "phong.frag");

	//printError("init shader");

	glutTimerFunc(5, &OnTimer, 0);
}

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(5, &OnTimer, value);
}

//-------------------------------callback functions------------------------------------------
void display(void)
{
	// This function is called whenever it is time to render
	//  a new frame; due to the idle()-function below, this
	//  function will get called several times per second

	// Clear framebuffer & zbuffer
	glClearColor(0.1, 0.1, 0.3, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader program
	glUseProgram(phongshader);

	//glUniformMatrix4fv(glGetUniformLocation(phongshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	//glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);
	//glUniform3fv(glGetUniformLocation(phongshader, "camPos"), 1, &cam.x);
	//glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//	glFlush(); // Can cause flickering on some systems. Can also be necessary to make drawing complete.
	glClearColor(0.0, 1.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	GLfloat ratio = (GLfloat)w / (GLfloat)h;
}


// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new
// frame
void idle()
{
	glutPostRedisplay();
}

//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(W, H);

	glutInitContextVersion(3, 2);
	glutCreateWindow("Render to texture with FBO");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	GLenum err = glewInit();
	if (GLEW_OK != err)	{
		printf("Error in the face!\n");
	}
	init();
	glutMainLoop();
	exit(0);
}
