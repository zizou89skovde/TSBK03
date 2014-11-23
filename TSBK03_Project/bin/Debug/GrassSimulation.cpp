#include "GrassSimulation.h"

GrassSimulation::GrassSimulation()
{
	/** Initalize grass model object **/
    mGrassScene = new ModelObject(); //Notera: ModelObject är lite feldöpt. Den borde heta Scene object. Eftersom den kan hålla flera olika modeller/shaders osv.

   /** Sphere Model **/
    Model* modelSphere = LoadModelPlus((char *)"sphere.obj");
    mGrassScene->setModel(modelSphere,GRASS_SPHERE_SHADER_ID);

    /** Sphere Shader **/
    GLuint sphereShader = loadShaders("sphere.vert", "sphere.frag");
    mGrassScene->setShader(sphereShader,GRASS_SPHERE_SHADER_ID);

    /** Sphere Transform **/
    mat4 transform2 = T(0,0,0); 
    mGrassScene->setTransform(transform2,GRASS_SPHERE_SHADER_ID);

	/* Jocke */

	/** Load shader **/
	//GLuint grassShader = loadShadersG("grass.vert", "grass.frag", "grass.gs");
	//mGrassScene->setShader(sphereShader, GRASS_SHADER_ID);

	 /**  Assign texture handles to the model object **/
    // mGrassScene->setTexture(...
	

	/* End Jocke*/

}

void GrassSimulation::draw(mat4 projectionMatrix,mat4 viewMatrix){
    mGrassScene->draw(projectionMatrix,viewMatrix);
}

void GrassSimulation::update(){
    
}

GrassSimulation::~GrassSimulation()
{
    //dtor
}
