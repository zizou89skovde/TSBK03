
#ifdef WIN32
    #include <windows.h>
    #include <GL/glew.h>
    #include <GL/glut.h>
    #include <GL/freeglut_ext.h>
    #include <direct.h>
#else
    #include <GL/gl.h>
    //#include "MicroGlut.h"
    //#include <GL/glut.h>
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>



#include "common/GL_utilities.h"

#include "ModelObject.h"
#include "LoadTGA.h"
#include "CPUClothSimulation.h"
#include "GPUClothSimulation.h"
#include "GPUWaterSimulation.h"

#include "GrassSimulation.h"
#include "Terrain.h"
#include "KeyMouseHandler.h"

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

GLuint WIDTH;
GLuint HEIGHT;

// Cloth simulation
SimulationClass * clothSimulation;
GrassSimulation * mGrassSimulation;
GPUWaterSimulation * waterSimulation;
Terrain *mTerrain;
void init(void)
{
	//dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

    GLuint texId;
    LoadTGATextureSimple((char*)"textures/terrain2.tga",&texId);

    //clothSimulation =  new GPUWaterSimulation(&WIDTH,&HEIGHT,"shaders/water_verlet.vert","shaders/water_verlet.frag"); //new GPUClothSimulation(&WIDTH,&HEIGHT,"shaders/mass_spring_verlet.vert","shaders/mass_spring_verlet.frag");

    //waterSimulation = new GPUWaterSimulation(&WIDTH,&HEIGHT,"shaders/water_verlet.vert","shaders/water_verlet.frag");
    printError("init cloth simulation");

    mKeyMouseHandler.mClothSimulation = clothSimulation;
    mTerrain = new Terrain();
/*
    mGrassSimulation = new GrassSimulation();




    printError("init terrain");

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
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
	//	glFlush(); // Can cause flickering on some systems. Can also be necessary to make drawing complete.
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    viewMatrix = mKeyMouseHandler.getViewMatrix();
    mTerrain->draw(projectionMatrix,viewMatrix);
    //mGrassSimulation->draw(projectionMatrix,viewMatrix);
    clothSimulation->draw(projectionMatrix,viewMatrix);
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
    WIDTH = w;
    HEIGHT = h;
    GLint curfbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curfbo);
    if(curfbo == 0){
        glViewport(0, 0, w, h);
    }

	GLfloat ratio = (GLfloat)w / (GLfloat)h;


	projectionMatrix = perspective(90, ratio, 1.0, 800);
	printError("Reshape");
}


// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new
// frame
void idle()
{

 //   clothSimulation->update();

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

	glutInitContextVersion(4, 1);
	glutCreateWindow("Render to texture with FBO");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
#ifdef WIN32
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
#endif
	init();
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouse);
	glutMainLoop();
	exit(0);
}
