#include "GPUSimulation.h"

GPUSimulation::GPUSimulation(GLuint* w,GLuint* h, const char * vertexShader, const char * fragmentShader)
{
    mScreenWidth = w;
    mScreenHeight = h;

}

GPUSimulation::~GPUSimulation(){

}

void GPUSimulation::intializeSimulation(SimulationData_Type * simulationData){

    mSimulationData = *simulationData;

    mGPUSimulation = new ModelObject();
    uploadSquareModelData(mGPUSimulation,GPU_SHADER_COMPUTE);

    GLuint verletShader = loadShaders(mSimulationData.VertexShader,mSimulationData.FragmentShader);
    mGPUSimulation->setShader(verletShader,GPU_SHADER_COMPUTE,NONE);

    FRAME_ATTACHMENT[0] = GL_COLOR_ATTACHMENT0;
    FRAME_ATTACHMENT[1] = GL_COLOR_ATTACHMENT1;

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

    /** Set initial textures **/
    mGPUSimulation->setTexture(fboComputation2->texids[0],GPU_SHADER_COMPUTE,((const char *)"u_CurrentPosition"));
    mGPUSimulation->setTexture(fboComputation2->texids[1],GPU_SHADER_COMPUTE,((const char *)"u_PreviousPosition"));

    /** Index of the FBO contains the most recent position of the cloth masses **/
    mActiveFBOIndex = 0;
/*
    GLfloat * meta = (GLfloat*)malloc(sizeof(GLfloat)*2);
	meta[0] = mSimulationData.GridDimension;
	meta[1] = mSimulationData.GridSize;

	*/

}

void GPUSimulation::setSimulationTexture(GLuint texid,const char * uniformName){
    mGPUSimulation->setTexture(texid,GPU_SHADER_COMPUTE,uniformName);
}

void GPUSimulation::replaceSimulationTexture(GLuint texid,const char * uniformName){
    mGPUSimulation->replaceTexture(texid,uniformName);
}

void GPUSimulation::setSimulationConstant(GLfloat constant, const char *uniformName){
   mGPUSimulation->setUniform(constant,GPU_SHADER_COMPUTE,uniformName);
}

void GPUSimulation::setSimulationConstant(GLfloat* constant,GLuint sizeConstant, const char *uniformName){
   mGPUSimulation->setUniform(constant,sizeConstant,GPU_SHADER_COMPUTE,uniformName);
}

void GPUSimulation::replaceSimulationConstant(GLfloat constant, const char *uniformName){
   mGPUSimulation->replaceUniform(&constant,uniformName);
}

void GPUSimulation::replaceSimulationConstant(GLfloat* constant, const char *uniformName){
   mGPUSimulation->replaceUniform(constant,uniformName);
}

/**
* Enables current FBO
**/
void GPUSimulation::enableFbo(FBOstruct * fbo){
    if(fbo == NULL){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        glViewport(0, 0, *mScreenWidth, *mScreenWidth);
    }else{
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
        glDrawBuffers(2,FRAME_ATTACHMENT);
        glViewport(0, 0, fbo->width, fbo->height);
		//glClear(GL_COLOR_BUFFER_BIT);
    }
}

/**
* Get the Frame Buffer Object that was used in the last computation
**/
FBOstruct*  GPUSimulation::getActiveFBO(){
    return ListFBOPosition.at(mActiveFBOIndex);
}

void GPUSimulation::shiftFBO(){

    mActiveFBOIndex = (mActiveFBOIndex + 1)% NumberOfFBOs;
    GLuint idx1 = (mActiveFBOIndex+1) % NumberOfFBOs;

    enableFbo(getActiveFBO());

    mGPUSimulation->replaceTexture(ListFBOPosition.at(idx1)->texids[0],((const char *)"u_CurrentPosition"));
    mGPUSimulation->replaceTexture(ListFBOPosition.at(idx1)->texids[1],((const char *)"u_PreviousPosition"));

    printError("Shift FBO");
}

/**
* Performs GPU computation, running the specified shaders.
* Returning FBO pointer where the most recent result
* reside.
**/
FBOstruct* GPUSimulation::simulate(GLuint numIterations){
    mat4 emptyMat;
    for(GLuint i = 0; i < numIterations; i++){
            shiftFBO();
            mGPUSimulation->draw(GPU_SHADER_COMPUTE,emptyMat,emptyMat);
    }
    enableFbo(NULL);
    return getActiveFBO();
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
GLulong GPUSimulation::getSpringState(GLuint x,GLuint y){

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
            GLuint xOffset = x + springIndices[2*i];
            GLuint yOffset = y + springIndices[2*i+1];
            if( yOffset < mSimulationData.GridDimension  && yOffset >= 0 && xOffset < mSimulationData.GridDimension  && xOffset >= 0 ){
                GLulong temp = (GLulong)pow(2,state);
                springState |= temp;
            }
        }
        return springState;


}
/**
Upload square model data ( code ripped from lab1 in TSBK03)
**/
void GPUSimulation::uploadSquareModelData(ModelObject * modelObj,GLuint shaderId){
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
void GPUSimulation::uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId) {

    GLuint GRID_DIM = mSimulationData.GridDimension;
    GLuint GRID_RES  = mSimulationData.GridDimension-1;
    int vertexCount = GRID_DIM*GRID_DIM;

    int triangleCount = (GRID_RES) * (GRID_RES)* 2;
    GLfloat vertexArray[(GPU_FLOATS_PER_POSITION) * vertexCount];// = (GLfloat *) malloc(sizeof(GLfloat) * (GPU_FLOATS_PER_POSITION) * vertexCount);

    for(GLuint y = 0; y < GRID_DIM; ++y)
        for(GLuint x = 0; x < GRID_DIM; ++x){

            float xPos = x/(float)GRID_RES;
            float yPos = y/(float)GRID_RES;
            vertexArray[(x + y * GRID_DIM)*3 + 0] = xPos;
            vertexArray[(x + y * GRID_DIM)*3 + 1] = yPos;
            vertexArray[(x + y * GRID_DIM)*3 + 2] = 0;

    }

    GLuint indexArray[triangleCount*VERTICES_PER_TRIANGLE];// = (GLuint *)malloc(sizeof(GLuint) * triangleCount*VERTICES_PER_TRIANGLE);
    for (GLuint y = 0; y < GRID_DIM-1; y++)
    for (GLuint x = 0; x < GRID_DIM-1; x++)
    {
    // Triangle 1
        indexArray[(x + y * GRID_RES)*INDICES_PER_QUAD + 0] = x + y * GRID_DIM;
        indexArray[(x + y * GRID_RES)*INDICES_PER_QUAD + 1] = x + (y+1) * GRID_DIM;
        indexArray[(x + y * GRID_RES)*INDICES_PER_QUAD + 2] = x+1 + y * GRID_DIM;
    // Triangle 2
        indexArray[(x + y *  GRID_RES)*INDICES_PER_QUAD + 3] = x+1 + y * GRID_DIM;
        indexArray[(x + y *  GRID_RES)*INDICES_PER_QUAD + 4] = x + (y+1) * GRID_DIM;
        indexArray[(x + y *  GRID_RES)*INDICES_PER_QUAD + 5] = x+1 + (y+1) * GRID_DIM;
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

 void GPUSimulation::generatePositionBuffer(FBOstruct* fbo){

    GLuint GRID_DIM       = mSimulationData.GridDimension;
    GLuint GRID_RES       = mSimulationData.GridDimension-1;
    GLuint GRID_SIZE      = mSimulationData.GridSize;

    GLfloat * GRID_OFFSET  = mSimulationData.GridOffset;
    bool isUpward = mSimulationData.isUpward;

    /** Generate start value for position buffer **/
    int vertexCount = GRID_DIM*GRID_DIM;
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (GPU_FLOATS_PER_POSITION) * vertexCount);
    GLfloat len = GRID_RES/2.0;
    for(GLuint y = 0; y < GRID_DIM; ++y)
        for(GLuint x = 0; x < GRID_DIM; ++x){
        vertexArray[(x + y * GRID_DIM)*GPU_FLOATS_PER_POSITION + 0] = GRID_OFFSET[0] + GRID_SIZE*(GLfloat)(x - len)/len;
        vertexArray[(x + y * GRID_DIM)*GPU_FLOATS_PER_POSITION + 1] = GRID_OFFSET[1] + (!isUpward)?0:GRID_SIZE*(GLfloat)(y - len)/len;
        vertexArray[(x + y * GRID_DIM)*GPU_FLOATS_PER_POSITION + 2] = GRID_OFFSET[2] + (isUpward)?0:GRID_SIZE*(GLfloat)(y - len)/len;

        /** Following assignment expecting an interpret cast. Might not be the case on all systems... **/
        vertexArray[(x + y * GRID_DIM)*GPU_FLOATS_PER_POSITION + 3] = getSpringState(x,y);
    }



    printError("Cloth BIND FBO PRE");
    //Set set dimensions
	fbo->width = GRID_DIM;
	fbo->height = GRID_DIM;

	/** Generate frame buffer and texture **/
    glGenFramebuffers(1, &fbo->fb);

    /** Num textures/render targets */
    GLuint numRendersTargets = 2;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
	fbo->texids = new GLuint[numRendersTargets];
	glGenTextures(numRendersTargets, fbo->texids);

    printError("Cloth BIND FBO Gen Buf");

    for(GLuint i = 0; i < numRendersTargets; i++){

        /** Bind texture **/
        glBindTexture(GL_TEXTURE_2D, fbo->texids[i]);

        /** Select clamp and nearest configuration **/
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        /** Upload initial data **/
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GRID_DIM,GRID_DIM, 0, GL_RGBA, GL_FLOAT, vertexArray);

        /** Attach texture to frame buffer **/
        glFramebufferTexture2D(GL_FRAMEBUFFER, FRAME_ATTACHMENT[i], GL_TEXTURE_2D, fbo->texids[i], 0);

    }

    /** Check frame buffer status **/
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        printf("ERROR FBO \n");
    }
	free(vertexArray);
    printError("Cloth Fbo Gen Buf");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

 }


