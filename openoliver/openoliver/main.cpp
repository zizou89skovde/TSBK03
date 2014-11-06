
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include "common/GL_utilities.h"
#include <direct.h>
#include "ModelObject.h"
#include "LoadTGA.h"
#include "ClothSimulation.h"
#include "GrassSimulation.h"
#include "Terrain.h"
#include "KeyMouseHandler.h"
#define GetCurrentDir _getcwd
// initial width and heights
#define W 512
#define H 512

void OnTimer(int value);
KeyMouseHandler mKeyMouseHandler;
//----------------------Globals-------------------------------------------------
GLuint phongshader = 0;
mat4 projectionMatrix;
mat4 viewMatrix;
//-------------------------------------------------------------------------------------

// Cloth simulation
ClothSimulation *mClothSimulation;
GrassSimulation *mGrassSimulation;
Terrain *mTerrain;
void init(void)
{
	//dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	/*viewMatrix =  lookAt(
            5, 20, 5,
			0, -1, 0,
			0, 0, 1
			);*/

    mTerrain = new Terrain();
    printError("init terrain");
/*
    mClothSimulation = new ClothSimulation();
	printError("init cloth simulation");

	mGrassSimulation = new GrassSimulation();
	printError("init cloth simulation");
*/
    // Create key/mouse handler
    //mKeyMouseHandler = KeyMouseHandler();

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

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//	glFlush(); // Can cause flickering on some systems. Can also be necessary to make drawing complete.
	glClearColor(0.0, 1.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//    mClothSimulation->draw(projectionMatrix,viewMatrix);
    viewMatrix = mKeyMouseHandler.getViewMatrix();
    mTerrain->draw(projectionMatrix,viewMatrix);

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	GLfloat ratio = (GLfloat)w / (GLfloat)h;
	projectionMatrix = perspective(90, ratio, 1.0, 800);
}


// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new
// frame
void idle()
{
	glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{
    mKeyMouseHandler.keyPress(key, 0, 0);
}

void mouseClick (int button, int state, int x, int y)
{
    if (state == GLUT_UP)
        mKeyMouseHandler.mouseUp();
}

void mouse (int x, int y)
{
    mKeyMouseHandler.mouseHandle(x, y);
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
    if (GLEW_OK != err)
    {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    if (glewIsSupported("GL_VERSION_1_4  GL_ARB_point_sprite"))
    {
        fprintf(stdout, "Status: GL_VERSION 1_4");
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printError ("pre init");

	init();
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouse);
	glutMainLoop();
	exit(0);
}

