#include "GPUClothSimulation.h"

GPUClothSimulation::GPUClothSimulation(GLuint * w,GLuint *h)
{
    /**
    HACK:  w,h pointers to variables that are assigned when
           glut-reshape function is called. These number
           is used when setting the viewport in
           GPUClothSimulation::enableFbo
    **/
    mScreenWidth = w;
    mScreenHeight = h;
    /**
    ** Create & Initialize ModelObjects:
    ** - Shader
    ** - Buffer Coordinates (Texture Coordinates)
    ** - Meta data, Grid dimension and scale/size
    **/

    mGPUClothScene = new ModelObject();
    uploadBufferCoordinates(mGPUClothScene,GPU_SHADER_CLOTH);

    GLuint clothModelShader = loadShadersG("cloth.vert","cloth.frag","cloth.gs");
    mGPUClothScene->setShader(clothModelShader,GPU_SHADER_CLOTH,VP);


    mGPUComputation = new ModelObject();
    uploadSquareModelData(mGPUComputation,GPU_SHADER_COMPUTE_POSITION);

    GLuint verletShader = loadShaders("mass_spring_verlet.vert","mass_spring_verlet.frag");
    mGPUComputation->setShader(verletShader,GPU_SHADER_COMPUTE_POSITION,NONE);

    /**
    **  Initializing two FBO for the ping pong pang computation
    **  of the cloth element position
    **/
    FBOstruct * fboComputation1 = new FBOstruct();
    generatePositionBuffer(fboComputation1);
    FBOstruct * fboComputation2 = new FBOstruct();
    generatePositionBuffer(fboComputation2);
    FBOstruct * fboComputation3 = new FBOstruct();
    generatePositionBuffer(fboComputation3);

    /** Add them to vector container **/
    ListFBOPosition.push_back(fboComputation1);
    ListFBOPosition.push_back(fboComputation2);
    ListFBOPosition.push_back(fboComputation3);

    /** Set initial textures **/
    mGPUClothScene->setTexture(fboComputation1->texid,GPU_SHADER_COMPUTE_POSITION,(const char *)"u_MassPos_Tex");

    mGPUComputation->setTexture(fboComputation2->texid,GPU_SHADER_COMPUTE_POSITION,((const char *)"u_CurrentPosition"));
    mGPUComputation->setTexture(fboComputation3->texid,GPU_SHADER_COMPUTE_POSITION,((const char *)"u_PreviousPosition"));

    /** Index of the FBO contains the most recent position of the cloth masses **/
    mActiveFBOIndex = 0;

    GLfloat * meta = (GLfloat*)malloc(sizeof(GLfloat)*2);
	meta[0] = GPU_CLOTH_RES;
	meta[1] = GPU_CLOTH_SIZE;

    /** Upload uniforms **/
    mGPUClothScene->setUniform(meta,2,GPU_SHADER_CLOTH,(const char*) "u_Resolution");

    /** Upload grid resolution **/
    mGPUComputation->setUniform(meta,2,GPU_SHADER_COMPUTE_POSITION,(const char*) "u_Resolution");

    /** Upload system props **/
    mGPUComputation->setUniform(GpuSystemDeltaTime,GPU_SHADER_COMPUTE_POSITION,(const char*)"u_DeltaTime");
    mGPUComputation->setUniform(GpuSystemDamping,GPU_SHADER_COMPUTE_POSITION,(const char*)"u_SystemDamping");
    mGPUComputation->setUniform(GpuSystemGravity,GPU_SHADER_COMPUTE_POSITION,(const char*)"u_Gravity");

    /** Upload spring props **/
    mGPUComputation->setUniform(GpuSpringDamping,GPU_SHADER_COMPUTE_POSITION,(const char*)"u_SpringDamping");
    mGPUComputation->setUniform(GpuSpringConstantStruct,GPU_SHADER_COMPUTE_POSITION,(const char*)"u_SpringConstant");
    mGPUComputation->setUniform(GpuSpringConstantBend,GPU_SHADER_COMPUTE_POSITION,(const char*)"u_SpringConstantBend");
    mGPUComputation->setUniform(GpuRestLengthStruct,GPU_SHADER_COMPUTE_POSITION,(const char*)"u_RestLengthStruct");
    mGPUComputation->setUniform((const float)sqrt(1.0*GpuRestLengthStruct),GPU_SHADER_COMPUTE_POSITION,(const char*)"u_RestLengthShear");
    mGPUComputation->setUniform(GpuRestLengthBend,GPU_SHADER_COMPUTE_POSITION,(const char*)"u_RestLengthBend");


}
GPUClothSimulation::~GPUClothSimulation(){

}

void GPUClothSimulation::enableFbo(FBOstruct * fbo){
    if(fbo == NULL){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, *mScreenWidth, *mScreenWidth);
    }else{
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
        glViewport(0, 0, fbo->width, fbo->height);
		//glClear(GL_COLOR_BUFFER_BIT);
    }

}

FBOstruct*  GPUClothSimulation::getActiveFBO(){
    return ListFBOPosition.at(mActiveFBOIndex);
}

void GPUClothSimulation::shiftFBO(){

    mActiveFBOIndex = (mActiveFBOIndex + 1)% NumberOfFBOs;

    GLuint idx1 = (mActiveFBOIndex+2) % NumberOfFBOs;
    GLuint idx2 = (mActiveFBOIndex+1) % NumberOfFBOs;

    enableFbo(getActiveFBO());//,NULL,NULL);
    mGPUComputation->replaceTexture(ListFBOPosition.at(idx1)->texid,((const char *)"u_CurrentPosition"));
    mGPUComputation->replaceTexture(ListFBOPosition.at(idx2)->texid,((const char *)"u_PreviousPosition"));

    printError("Shift FBO");
}

void GPUClothSimulation::draw(mat4 projectionMatrix, mat4 viewMatrix){
    /** Computing shaders **/
    GLuint numIterations = 1;
    for(GLuint i = 0; i < numIterations; i++){
        shiftFBO();
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        mGPUComputation->draw(GPU_SHADER_COMPUTE_POSITION,projectionMatrix,viewMatrix);

        /** TODO:
        shiftFBO();
        mGPUComputation->draw(GPU_SHADER_EVALUATE_SPRINGS,projectionMatrix,null);
        **/
    }
    printError("GPU Compute");

    /** Render Cloth **/
    enableFbo(NULL);
    mGPUClothScene->replaceTexture(getActiveFBO()->texid,(const char *)"u_MassPos_Tex");
    mGPUClothScene->draw(projectionMatrix,viewMatrix);
    printError("Cloth Gen Position Buffer Coordinates");

}

/**
The spring state are encoded according to following bit table:

  STRUCTURAL SRPINGS:

  RIGHT STRUCTURAL SPRING   =  s & 1
  UPPER STRUCTURAL SPRING   =  s & 2
  LEFT  STRUCTURAL SPRING   =  s & 4
  DOWN  STRUCTURAL SPRING   =  s & 8

  SHEAR SPRINGS:

  UP RIGHT SHEAR SPRING     =  s & 16
  UP LEFT SHEAR SPRING      =  s & 32
  DOWN LEFT SHEAR SPRING    =  s & 64
  DOWN RIGHT SHEAR SPRING   =  s & 128

  BEND SPRINGS:

  RIGHT BEND SPRING         =  s & 256
  UP BEND SPRING            =  s & 512
  LEFT BEND SPRING          =  s & 1024
  DOWN BEND SPRING          =  s & 2048

**/
GLulong GPUClothSimulation::getSpringState(GLuint x,GLuint y){

        GLint springIndices[] = {
            // Struct
            1 ,0,
            0 ,1,
            -1,0,
            0,-1,
            //Shear
            1 ,1,
            -1,1,
            -1,-1,
            1 ,-1,
            //Bend
            2,0,
            0,2,
            -2,0,
            0 ,-2
        };
        GLulong springState = 0;
        GLuint i;
        for(i = 0; i < 12; i++){
            GLuint state = i; /** Valid spring state codes: [1:13] , therefor offset i with 1 **/
            GLint xOffset = x + springIndices[2*i];
            GLint yOffset = y + springIndices[2*i+1];
            if( yOffset < GPU_CLOTH_DIM  && yOffset >= 0 && xOffset < GPU_CLOTH_DIM  && xOffset >= 0 ){
                GLulong temp = (GLulong)pow(2,state);
                springState |= temp;
            }
        }
           return springState;
/*
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
        }*/

}
/**
Upload square model data ( code ripped from lab1 in TSBK03)
**/
void GPUClothSimulation::uploadSquareModelData(ModelObject * modelObj,GLuint shaderId){
    GLfloat square[] = {
                    -1,-1,0,
                    -1,1, 0,
                    1,1, 0,
                    1,-1, 0};
    GLfloat squareTexCoord[] = {
                     0, 0,
                     0, 1,
                     1, 1,
                     1, 0};

    GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};
    modelObj->LoadDataToModel(
        square,
        NULL,
        squareTexCoord,
        NULL,
        squareIndices,
        4,
        2*3,
        shaderId);

    printError("Cloth Gen Position Buffer Coordinates ");
}

 /**
 * Generate array buffer containing texture coordinates.
 * These are used when sampling from the Position and Spring buffers/map
 **/
void GPUClothSimulation::uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId) {


    int vertexCount = GPU_CLOTH_DIM*GPU_CLOTH_DIM;
    int triangleCount = (GPU_CLOTH_DIM-1) * (GPU_CLOTH_DIM-1)* 2;
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (GPU_FLOATS_PER_POSITION) * vertexCount);

    for(GLuint y = 0; y < GPU_CLOTH_DIM; ++y)
        for(GLuint x = 0; x < GPU_CLOTH_DIM; ++x){

            float xPos = x/(float)GPU_CLOTH_RES;
            float yPos = y/(float)GPU_CLOTH_RES;
            vertexArray[(x + y * GPU_CLOTH_DIM)*3 + 0] = xPos;
            vertexArray[(x + y * GPU_CLOTH_DIM)*3 + 1] = yPos;
            vertexArray[(x + y * GPU_CLOTH_DIM)*3 + 2] = 0;

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
        vertexArray,
        NULL,
        NULL,
        NULL,
        indexArray,
        vertexCount,
        triangleCount*3,
        shaderId);

    printError("Cloth Gen Position Buffer Coordinates ");
    }
/**
 * Generate an FBO containing Mass positions. The initial position are also
 * uploaded.
 **/

 void GPUClothSimulation::generatePositionBuffer(FBOstruct* fbo){
    printError("Cloth BIND FBO PRE");
    //Set set dimensions
	fbo->width = GPU_CLOTH_DIM;
	fbo->height = GPU_CLOTH_DIM;

	//Generate frame buffer and texture
    glGenFramebuffers(1, &fbo->fb);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
	glGenTextures(1, &fbo->texid);
	glBindTexture(GL_TEXTURE_2D, fbo->texid);
    printError("Cloth BIND FBO Gen Buf");
	//Select clamp and nearest configuration
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int vertexCount = GPU_CLOTH_DIM*GPU_CLOTH_DIM;
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (GPU_FLOATS_PER_POSITION) * vertexCount);
    GLfloat len = GPU_CLOTH_RES/2.0;
    for(GLuint y = 0; y < GPU_CLOTH_DIM; ++y)
        for(GLuint x = 0; x < GPU_CLOTH_DIM; ++x){
        vertexArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_POSITION + 0] = GPU_CLOTH_SIZE*(GLfloat)(x - len)/len;
        vertexArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_POSITION + 1] = GPU_CLOTH_SIZE*(GLfloat)(y - len)/len;
        vertexArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_POSITION + 2] = 0;
        //Following assignment expecting an implicit interpret cast. Might not be the case on all systems...
        vertexArray[(x + y * GPU_CLOTH_DIM)*GPU_FLOATS_PER_POSITION + 3] = getSpringState(x,y);
    }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GPU_CLOTH_DIM,GPU_CLOTH_DIM, 0, GL_RGBA, GL_FLOAT, vertexArray);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texid, 0);
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        printf("ERROR FBO \n");
    }
	free(vertexArray);
    printError("Cloth Fbo Gen Buf");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

