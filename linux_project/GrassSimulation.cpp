#include "GrassSimulation.h"

GrassSimulation::GrassSimulation()
{

    mGrassScene = new ModelObject(); 
//Notera: ModelObject är lite feldöpt. Den borde heta Scene object. Eftersom den kan hålla flera olika modeller/shaders osv.

   /** Sphere Model **/
    Model* modelSphere = LoadModelPlus((char *)"sphere.obj");
    mGrassScene->setModel(modelSphere,GRASS_SPHERE_SHADER_ID);

    /** Sphere Shader **/
    GLuint sphereShader = loadShaders("sphere.vert", "sphere.frag");
    mGrassScene->setShader(sphereShader,GRASS_SPHERE_SHADER_ID);

    /** Sphere Transform **/
    mat4 transform2 = T(0,0,0); 
    mGrassScene->setTransform(transform2,GRASS_SPHERE_SHADER_ID);

    


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
