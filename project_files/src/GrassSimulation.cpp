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


	mGrassScene->setShader(grassShader, GRASS_SHADER_ID, MVP); //mGrassScene->setShader(grassShader, GRASS_SHADER_ID,VP);
	mGrassScene->setTransform(IdentityMatrix(), GRASS_SHADER_ID);
	mGrassScene->setDrawMethod(GRASS_SHADER_ID,POINTS);

	mTerrain->setExternalModels(mGrassScene);


    /** Grass mask **/
	TextureData textureData;
    LoadTGATextureData((char*)"textures/test2.tga", &textureData);

    /**  Upload buffer coordinates **/
    uploadBufferCoordinates(mGrassScene,&textureData,GRASS_SHADER_ID);

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
/*
	GLuint heightMapTexture;
	LoadTGATextureSimple("textures/fft-terrain4.tga",&heightMapTexture);*/
    mGrassScene->setTexture(mTerrain->getTextureData()->texID,GRASS_SHADER_ID,"u_HeightMap");

	/***** END WATCH OUT **********************************************************************************/



    /** Noise **/
    GLuint grassNoiseTexture;
    LoadTGATextureSimple((char*)"textures/noise.tga", &grassNoiseTexture);
    mGrassScene->setTexture(grassNoiseTexture,GRASS_SHADER_ID,"u_GrassNoise");
}

void GrassSimulation::setTerrain(Terrain * terrain){
    mTerrain = terrain;
}



void GrassSimulation::uploadBufferCoordinates(ModelObject * modelobject,TextureData* maskTexture, GLuint shaderId){


	GLuint textureWidth = maskTexture->width; // OBS 256!
	GLuint textureHeight = maskTexture->height;
	
    GLuint DIM = 300; /* Antal vertices per rad*/

    GLuint vertexCount = DIM*DIM;
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (FLOATS_PER_TEXEL) * vertexCount);
	GLuint indexCount = 0;

     for(GLuint y = 0; y < DIM; ++y)			//y = [0, 99]
        for(GLuint x = 0; x < DIM; ++x){		//x = [0, 99]
            float xPos = x/(float)(DIM-1);		//xPos = [0, 1]
		//	printf("x = %d, xPos = %f \n", x, xPos);
            float yPos = y/(float)(DIM-1);		//yPos = [0, 1]
		//	printf("y = %d, yPos = %f \n", y, yPos);

			GLuint texCoordX = (int)(xPos*(textureWidth-1)); //OBS texCoordX = [0, 256] ta minus 1?
			//printf("texCoordX = %d \n", texCoordX);
			GLuint texCoordY = (int)(yPos*(textureHeight - 1));


			GLuint mask = maskTexture->imageData[(texCoordX + texCoordY * maskTexture->width) * (maskTexture->bpp/8)]; // 0 -> 255
			if(mask == 255){
            	vertexArray[indexCount++] = xPos;
            	vertexArray[indexCount++] = yPos;
            	vertexArray[indexCount++] = 0;	
			}
        }

	size_t numBytes = sizeof(GLfloat) * indexCount;

	// DEBUG
	printf("sizeof(GLfloat) = %d \n", (int) sizeof(GLfloat));
	printf("indexCount = %d \n", (int) indexCount);
	printf("numBytes = %d \n", (int)numBytes);
	printf("maskTexure->width = %d \n", maskTexture->width);
	printf("maskTexure->bpp = %d \n", maskTexture->bpp);
	printf("maskTexure->bpp/8 = %d \n", maskTexture->bpp/8);
	printf("textureWidth = %d \n", textureWidth);

	/** Create array with a length that matches the number of masked vertices **/
	GLfloat * maskedArray = (GLfloat *) malloc(numBytes);

	/** Copy masked vertices **/
	memcpy(maskedArray,vertexArray,numBytes);

	/** Free original buffer **/
	free(vertexArray);



	Model * m = new Model();

	glGenVertexArrays(1,&m->vao);
	glBindVertexArray(m->vao);

	glGenBuffers(1, &m->vb);
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glBufferData(GL_ARRAY_BUFFER, numBytes, maskedArray, GL_STATIC_DRAW);
	

	m->numVertices = indexCount/3;

	/** Free data */
	free(maskTexture->imageData);
	free(maskedArray);
	
	modelobject->setModel(m,shaderId);
	


}

void GrassSimulation::draw(mat4 projectionMatrix,mat4 viewMatrix){

	glDisable(GL_CULL_FACE);
	mGrassScene->draw(GRASS_SHADER_ID,projectionMatrix,viewMatrix);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


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
	//printf("Vind: %f \n", angle);
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
