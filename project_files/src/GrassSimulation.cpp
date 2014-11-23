#include "GrassSimulation.h"

GrassSimulation::GrassSimulation()
{
	/** Initalize grass model object **/
    mGrassScene = new ModelObject(); //Notera: ModelObject är lite feldöpt. Den borde heta Scene object. Eftersom den kan hålla flera olika modeller/shaders osv.

	/* Jocke */
	/** Load shader **/
	GLuint grassShader = loadShadersG("shaders/grass.vert", "shaders/grass.frag", "shaders/grass.gs");
	mGrassScene->setShader(grassShader, GRASS_SHADER_ID,VP);

	 /**  Assign texture handles to the model object **/
    /*GLuint grassMaskTexture;
	LoadTGATextureSimple((char*)"texture.tga",&grassMaskTexture);
    mGrassScene->setTexture(grassMaskTexture,GRASS_SHADER_ID,(const char*)"u_GrassMask");

    GLuint heightMapTexture;
    LoadTGATextureSimple((char*)"terrain.tga",&heightMapTexture);
    mGrassScene->setTexture(heightMapTexture,GRASS_SHADER_ID,(const char*)"u_HeightMap");
*/
    /**  Upload buffer coordinates **/
    uploadBufferCoordinates(mGrassScene,GRASS_SHADER_ID);

    /** Upload uniform parameters **/
    mGrassScene->setUniform(1.0f,GRASS_SHADER_ID,"u_Wind");

    GLfloat * arrayTest = (GLfloat*)malloc(sizeof(GLfloat)*3);
    arrayTest[0] = 1;
    arrayTest[1] = 1;
    arrayTest[2] = 1;
    mGrassScene->setUniform(arrayTest,3,GRASS_SHADER_ID,"u_CenterPosition");

	/* End Jocke*/

}

void GrassSimulation::uploadBufferCoordinates(ModelObject * modelobject,GLuint shaderId){

    GLuint DIM = 4;

    GLuint vertexCount = DIM*DIM;
	GLuint triangleCount = (DIM-1) * (DIM-1)* 2;
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (FLOATS_PER_TEXEL) * vertexCount);
	GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount*VERTICES_PER_TRIANGLE);

     for(GLuint y = 0; y < DIM; ++y)
        for(GLuint x = 0; x < DIM; ++x){

            float xPos = x/(float)(DIM-1);
            float yPos = y/(float)(DIM-1);
            vertexArray[(x + y * DIM)*3 + 0] = xPos;
            vertexArray[(x + y * DIM)*3 + 1] = yPos;
            vertexArray[(x + y * DIM)*3 + 2] = 0;
        }

	 for (GLuint y = 0; y < DIM-1; y++)
        for (GLuint x = 0; x < DIM-1; x++)
		{
		// Triangle 1
			indexArray[(x + y * (DIM-1))*6 + 0] = x + y * DIM;
			indexArray[(x + y * (DIM-1))*6 + 1] = x + (y+1) * DIM;
			indexArray[(x + y * (DIM-1))*6 + 2] = x+1 + y * DIM;
		// Triangle 2
			indexArray[(x + y * (DIM-1))*6 + 3] = x+1 + y * DIM;
			indexArray[(x + y * (DIM-1))*6 + 4] = x + (y+1) * DIM;
			indexArray[(x + y * (DIM-1))*6 + 5] = x+1 + (y+1) * DIM;
		}
        modelobject->LoadDataToModel(
			vertexArray,
			NULL,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3,
			shaderId);

}

void GrassSimulation::draw(mat4 projectionMatrix,mat4 viewMatrix){
    mGrassScene->draw(projectionMatrix,viewMatrix);
}

void GrassSimulation::update(){
      GLfloat temp = 2.0f;
      mGrassScene->replaceUniform(&temp,"u_Wind");

}

GrassSimulation::~GrassSimulation()
{
    //dtor
}
