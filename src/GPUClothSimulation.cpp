#include "GPUClothSimulation.h"

GPUClothSimulation::GPUClothSimulation()
{

    /**
    ** Create & Initialize ModelObjects:
    ** - Shader
    ** - Buffer Coordinates (Texture Coordinates)
    ** - Meta data, Grid dimension and scale/size
    **/


    mGPUClothScene = new ModelObject();
    GLuint clothModelShader = loadShadersG("cloth.vert","cloth.frag","cloth.gs");
    mGPUClothScene->setShader(clothModelShader,GPU_CLOTH_SHADER);
    uploadBufferCoordinates(mGPUClothScene,GPU_CLOTH_SHADER);

    mGPUPositionComputation = new ModelObject();
    GLuint verletShader = loadShaders("Mass_Spring_Verlet.vert","Mass_Spring_Verlet.frag");
    mGPUPositionComputation->setShader(verletShader,GPU_COMPUTE_SHADER);
    uploadBufferCoordinates(mGPUClothScene,GPU_COMPUTE_SHADER);

    GLfloat * meta = (GLfloat*)malloc(sizeof(GLfloat)*2);
	meta[0] = GPU_CLOTH_RES;
	meta[1] = GPU_CLOTH_SIZE;
    mGPUClothScene->setUniform(meta,2,GPU_CLOTH_SHADER,"u_Meta_Data");
    mGPUPositionComputation->setUniform(meta,2,GPU_COMPUTE_SHADER,"u_Meta_Data");

    /**
    **  Initializing two FBO for the ping pong computation
    **  of the cloth element position
    **/
    FBOstruct * fboComputation1 = new FBOstruct();
    generatePositionBuffer(fboComputation1);
    FBOstruct * fboComputation2 = new FBOstruct();
    generatePositionBuffer(fboComputation2);

    /** Add them to vector container **/
    ListFBOPosition.push_back(fboComputation1);
    ListFBOPosition.push_back(fboComputation2);



}
GPUClothSimulation::~GPUClothSimulation(){

}


/**
The spring state are encoded according to following bit table:
  LEFT  STRUCTURAL SPRING =  s & 1
  UPPER STRUCTURAL SPRING =  s & 2
  UP LEFT SHEAR SPRING    =  s & 4
  DOWN LEFT SHEAR SPRING  =  s & 8
  LEFT BEND SPRING        =  s & 16
  UP BEND SPRING          =  s & 32
**/
GLubyte GPUClothSimulation::getSpringState(GLuint x,GLuint y){
        GLubyte springState = 0;

        if(y < GPU_CLOTH_DIM - 1) {
            //upper vertical structural spring
            springState |= SPRING_STRUCT_UP;
        }
        if(y < GPU_CLOTH_DIM - 2) {
            //upper vertical bend spring
            springState |= SPRING_BEND_UP;
        }

        if(x < GPU_CLOTH_DIM - 1){
            //Right horizontal structural spring
              springState |= SPRING_STRUCT_RIGHT;
        }

        if(x < GPU_CLOTH_DIM - 2){
            //Right horizontal bend spring
             springState |= SPRING_BEND_RIGHT;
        }

        if( y < GPU_CLOTH_DIM - 1 && x < GPU_CLOTH_DIM - 1){
            //Right upper shear spring
            springState |= SPRING_SHEAR_UP_RIGHT;
        }
        if( y > 0 && x < GPU_CLOTH_DIM - 1){
            //Right lower shear spring
            springState |= SPRING_SHEAR_DOWN_RIGHT;
        }
        return springState;
}

 /**
 * Generate array buffer containing texture coordinates.
 * These are used when sampling from the Position and Spring buffers/map
 **/
void GPUClothSimulation::uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId) {


     int vertexCount = GPU_CLOTH_DIM*GPU_CLOTH_DIM;
	int triangleCount = (GPU_CLOTH_DIM-1) * (GPU_CLOTH_DIM-1)* 2;
    GLfloat *vertexCoordinateArray = (GLfloat *) malloc(sizeof(GLfloat) * (GPU_FLOATS_PER_TEXEL) * vertexCount);

     for(GLuint y = 0; y < GPU_CLOTH_DIM; ++y)
        for(GLuint x = 0; x < GPU_CLOTH_DIM; ++x){
            float xPos = x/(float)GPU_CLOTH_RES;
            float yPos = y/(float)GPU_CLOTH_RES;
            vertexCoordinateArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_TEXEL + 0] = xPos;
            vertexCoordinateArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_TEXEL + 1] = yPos;
    }


    GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount*VERTICES_PER_TRIANGLE);
	 for (GLuint y = 0; y < GPU_CLOTH_DIM-1; y++)
        for (GLuint x = 0; x < GPU_CLOTH_DIM-1; x++)
		{
		// Triangle 1
			indexArray[(x + y * (GPU_CLOTH_DIM-1))*INDICES_PER_QUAD + 0] = x + y * GPU_CLOTH_DIM;
			indexArray[(x + y * (GPU_CLOTH_DIM-1))*INDICES_PER_QUAD + 1] = x + (y+1) * GPU_CLOTH_DIM;
			indexArray[(x + y * (GPU_CLOTH_DIM-1))*INDICES_PER_QUAD + 2] = x+1 + y * GPU_CLOTH_DIM;
		// Triangle 2
			indexArray[(x + y * (GPU_CLOTH_DIM-1))*INDICES_PER_QUAD + 3] = x+1 + y * GPU_CLOTH_DIM;
			indexArray[(x + y * (GPU_CLOTH_DIM-1))*INDICES_PER_QUAD + 4] = x + (y+1) * GPU_CLOTH_DIM;
			indexArray[(x + y * (GPU_CLOTH_DIM-1))*INDICES_PER_QUAD + 5] = x+1 + (y+1) * GPU_CLOTH_DIM;
		}
        modelObj->LoadDataToModel(
			vertexCoordinateArray,
			NULL,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3,
			shaderId);
    }
/**
 * Generate an FBO containing Mass positions. The initial position are also
 * uploaded.
 **/

 void GPUClothSimulation::generatePositionBuffer(FBOstruct* fbo){

    //Create fbo struct
    fbo = (FBOstruct*)malloc(sizeof(FBOstruct));

    //Set set dimensions
	fbo->width = GPU_CLOTH_DIM;
	fbo->height = GPU_CLOTH_DIM;

	//Generate frame buffer and texture
    glGenFramebuffers(1, &fbo->fb);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
	glGenTextures(1, &fbo->texid);
	glBindTexture(GL_TEXTURE_2D, fbo->texid);

	//Select clamp and nearest configuration
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int vertexCount = GPU_CLOTH_DIM*GPU_CLOTH_DIM;
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (GPU_FLOATS_PER_TEXEL) * vertexCount);
    GLfloat len = GPU_CLOTH_RES/2.0;
    for(GLuint y = 0; y < GPU_CLOTH_DIM; ++y)
        for(GLuint x = 0; x < GPU_CLOTH_DIM; ++x){
        vertexArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_TEXEL + 0] = GPU_CLOTH_SIZE*(GLfloat)(x - len)/len;
        vertexArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_TEXEL + 1] = GPU_CLOTH_SIZE*(GLfloat)(y - len)/len;
        vertexArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_TEXEL + 2] = 0;
        vertexArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_TEXEL + 3] = getSpringState(x,y);
    }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GPU_CLOTH_DIM,GPU_CLOTH_DIM, 0, GL_RGBA, GL_FLOAT, vertexArray);
	free(vertexArray);
    printError("Cloth Fbo Spring error: ");

 }
 /**
 * Generate an FBO containing spring states. The initial states are also
 * uploaded.
 **/
void GPUClothSimulation::generateSpringBuffer(FBOstruct* fbo){

    //Set set dimensions
	fbo->width = GPU_CLOTH_DIM;
	fbo->height = GPU_CLOTH_DIM;

	//Generate frame buffer and texture
    glGenFramebuffers(1, &fbo->fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
	glGenTextures(1, &fbo->texid);
	glBindTexture(GL_TEXTURE_2D, fbo->texid);

	//Select clamp and nearest configuration
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //Load initial spring data
    int vertexCount = GPU_CLOTH_DIM*GPU_CLOTH_DIM;
    GLfloat *initialSpringState = (GLfloat *) malloc(sizeof(GLfloat) * vertexCount);
    for(GLuint y = 0; y < GPU_CLOTH_DIM; ++y)
        for(GLuint x = 0; x < GPU_CLOTH_DIM; ++x){
        initialSpringState[(x + y * GPU_CLOTH_DIM) + 0] = getSpringState(x,y);
    }
    // Load data to texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, GPU_CLOTH_DIM,GPU_CLOTH_DIM, 0,GL_RED, GL_FLOAT, initialSpringState);

	//Free memory
    free(initialSpringState);

    //Bind default/screen frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    printError("Cloth 1  FBO SPRING");
}

