#include "GrassSimulation.h"

GrassSimulation::GrassSimulation()
{
}

void GrassSimulation::initialize(){

    /** Assign default values **/
    previousTime = -1;

	/** Initalize grass model object **/
    mGrassScene = new ModelObject(); //Notera: ModelObject är lite feldöpt. Den borde heta Scene object. Eftersom den kan hålla flera olika modeller/shaders osv.

	/** Load shader **/
	GLuint grassShader = loadShadersG("shaders/grass.vert", "shaders/grass.frag", "shaders/grass.gs");
	mGrassScene->setShader(grassShader, GRASS_SHADER_ID,VP);

    /**  Upload buffer coordinates **/
    uploadBufferCoordinates(mGrassScene,GRASS_SHADER_ID);

    /** Upload terrain parameters **/
    TerrainMetaData* terrainData = mTerrain->getTerrainMetaData();
    mGrassScene->setUniformFloat(1.0f, GRASS_SHADER_ID, "u_Wind");

	angle = 0.0f;

	/***** START WATCH OUT **********************************************************************************/

    GLfloat * gridOffset = (GLfloat*)malloc(sizeof(GLfloat)*3);
    gridOffset[0] = -terrainData->TerrainSize/2.0;
    gridOffset[1] = 0;
    gridOffset[2] = -terrainData->TerrainSize/2.0;
    mGrassScene->setUniformFloat(gridOffset,3,GRASS_SHADER_ID,"u_GridOffset");
    //mGrassScene->setUniform(terrainData->TerrainResolution,"u_GridResolution");
    mGrassScene->setUniformFloat(terrainData->TerrainSize,GRASS_SHADER_ID,"u_GridSize");
    mGrassScene->setUniformFloat(terrainData->HeightScale,GRASS_SHADER_ID,"u_GridHeightScale");

    /** Set hight map texture **/
    mGrassScene->setTexture(mTerrain->getTextureData()->texID,GRASS_SHADER_ID,"u_HeightMap");

	/***** END WATCH OUT **********************************************************************************/

    /** Grass mask **/
    GLuint grassMaskTexture;
    LoadTGATextureSimple((char*)"textures/test.tga", &grassMaskTexture);
    mGrassScene->setTexture(grassMaskTexture,GRASS_SHADER_ID,"u_GrassMask");

    /** Noise **/
    GLuint grassNoiseTexture;
    LoadTGATextureSimple((char*)"textures/noise.tga", &grassNoiseTexture);
    mGrassScene->setTexture(grassNoiseTexture,GRASS_SHADER_ID,"u_GrassNoise");
}

void GrassSimulation::setTerrain(Terrain * terrain){
    mTerrain = terrain;
}

void GrassSimulation::uploadBufferCoordinates(ModelObject * modelobject,GLuint shaderId){

    GLuint DIM = 300; /* Antal vertices per rad*/

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
			vertexArray, /* in_Position */
			NULL,          /* in_Normal */
			NULL,           /* in_TextureCoordinate */
			NULL,           /* in_Colasofsgm */
			indexArray,
			vertexCount,
			triangleCount*3,
			shaderId);

}

void GrassSimulation::draw(mat4 projectionMatrix,mat4 viewMatrix){
    mGrassScene->draw(projectionMatrix,viewMatrix);
}

void GrassSimulation::update(){

	//GLfloat temp = 2.0f;
	//mGrassScene->replaceUniform(&temp,"u_Wind");


   /* if(previousTime < 0){
        previousTime = glutGet(GLUT_ELAPSED_TIME);
        return;
    }

    GLfloat newTime = (GLfloat) glutGet(GLUT_ELAPSED_TIME);
    GLfloat elapsedTime = newTime-previousTime;
	*/

	GLfloat dt = 0.005f;
	GLfloat pi = 3.141592653589793;

	angle += dt;
	if (angle >= 2*pi) {
		angle = 0;
	}
	printf("Vind: %f \n", angle);
	mGrassScene->replaceUniformFloat(&angle,GRASS_SHADER_ID,"u_Wind");
/*
    GLuint numIterations = (GLuint)(elapsedTime/(dt*1.0f));
    if(numIterations > 0){
        previousTime = newTime;
        applyForces();
        integrate();
        checkCollision();

    }
*/
}

GrassSimulation::~GrassSimulation()
{
    //dtor
}
