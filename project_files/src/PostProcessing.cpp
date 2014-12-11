#include "PostProcessing.h"

PostProcessing::PostProcessing(GLuint * w, GLuint * h)
{
    mScreenWidth = w;
    mScreenHeight = h;

    initializePostProcessing();


}

void PostProcessing::initializePostProcessing(){

    /** Create FBO's **/
	generateDepthFBO(&mLightDepthFBO,mFBOSize,mFBOSize);
	generateDepthFBO(&mSceneDepthFBO,mFBOSize,mFBOSize);
    mLightFBO = initFBO(mFBOSize,mFBOSize,0);

	/** Create model object */
  	mPostProcessingModel = new ModelObject();

	/** Load light volume shader */
#ifdef DEBUG
    GLuint lightVolumeShader = loadShadersG("shaders/lightvolume_debug.vert","shaders/lightvolume_debug.frag","shaders/lightvolume_debug.gs");
#else
	GLuint lightVolumeShader = loadShaders("shaders/lightvolume.vert","shaders/lightvolume.frag");
#endif
    mPostProcessingModel->setShader(lightVolumeShader,SHADER_LIGHT_VOLUME,MVP);

    /** Set Transform */
    mPostProcessingModel->setTransform(IdentityMatrix(),SHADER_LIGHT_VOLUME);

	/** Generate frustum mesh **/
	generateFrustumMesh(mFar,mNear,mLightResolutiuon);

	/** Set light depth buffer as texture for the light volume shader */
	mPostProcessingModel->setTexture(mLightDepthFBO.depth,SHADER_LIGHT_VOLUME,"u_LightDepthMap");
	mPostProcessingModel->setTexture(mSceneDepthFBO.depth,SHADER_LIGHT_VOLUME,"u_SceneDepthMap");

	/** Upload VP-matrix for light */
	mPostProcessingModel->setUniformMatrix(IdentityMatrix(),SHADER_LIGHT_VOLUME,"LightTextureMatrix");


	mLightProjectionMatrix = perspective(90, mRatio, mNear, mFar);
    lightLookAt(vec3(0,0,-1),vec3(1,0,-1));

 	/** Light Parameters **/
    mPostProcessingModel->setUniformFloat(mNear,SHADER_LIGHT_VOLUME,"u_LightNear");
    mPostProcessingModel->setUniformFloat(mFar,SHADER_LIGHT_VOLUME,"u_LightFar");
#ifndef DEBUG
    /** Screen Parameters **/
    mPostProcessingModel->setUniformFloat(512,SHADER_LIGHT_VOLUME,"u_ScreenWidth");
    mPostProcessingModel->setUniformFloat(512,SHADER_LIGHT_VOLUME,"u_ScreenHeight");

    /** Camera Parameters **/
    mPostProcessingModel->setUniformFloat(1,SHADER_LIGHT_VOLUME,"u_CameraNear");
    mPostProcessingModel->setUniformFloat(80,SHADER_LIGHT_VOLUME,"u_CameraFar");
#endif
/*************************************************************************************/


    /** Sphere Shader **/
    GLuint sphereShader = loadShaders("shaders/sphere.vert", "shaders/sphere.frag");
    mPostProcessingModel->setShader(sphereShader,SHADER_SPHERE,MVP);

    /** Sphere Model **/
    Model* modelSphere = LoadModelPlus((char *)"models/skydome.obj");
	free(modelSphere->texCoordArray);
	modelSphere->texCoordArray = NULL;
    mPostProcessingModel->setModel(modelSphere,SHADER_SPHERE);

    /** Sphere Transform **/
    mat4 transform2 = T(0,0,0)*S(4,4,4);
    mPostProcessingModel->setTransform(transform2,SHADER_SPHERE);

   /*************************************************************************************/
    /** Full-screen shader**/
    GLuint quadShader = loadShaders("shaders/screen_quad.vert", "shaders/screen_quad.frag");
    mPostProcessingModel->setShader(quadShader,SHADER_SCREEN_QUAD,NONE);

  	/** Full-screen quad model **/
    uploadSquareModelData(mPostProcessingModel,SHADER_SCREEN_QUAD);

    /** Set texture **/
    mPostProcessingModel->setTexture(mLightFBO->texid,SHADER_SCREEN_QUAD,"u_Texture");



    mTime = 0.0f;
#ifdef SHADOW_MAP
   /*************************************************************************************/
   /** Shadow Map (Full screen quad shader) **/
   	GLuint shadowShader = loadShaders("shaders/shadows.vert", "shaders/shadows.frag");
    mPostProcessingModel->setShader(shadowShader,SHADER_SHADOW_MAP,NONE);

	/** Full-screen quad model **/
	uploadSquareModelData(mPostProcessingModel,SHADER_SHADOW_MAP);

    /** Set texture **/
    mPostProcessingModel->setTexture(mLightDepthFBO.depth,SHADER_SHADOW_MAP,"u_LightDepth");
    mPostProcessingModel->setTexture(mSceneDepthFBO.depth,SHADER_SHADOW_MAP,"u_SceneDepth");

    /** Upload uniform matrices **/
    mPostProcessingModel->setUniformMatrix(IdentityMatrix(),SHADER_SHADOW_MAP,"LightTextureMatrix");

    float emptyFloat[] =  {0,0,0};
    mPostProcessingModel->setUniformFloat(emptyFloat,3,SHADER_SHADOW_MAP,"u_CameraRight");
    mPostProcessingModel->setUniformFloat(emptyFloat,3,SHADER_SHADOW_MAP,"u_CameraUp");
    mPostProcessingModel->setUniformFloat(emptyFloat,3,SHADER_SHADOW_MAP,"u_CameraLook");
    mPostProcessingModel->setUniformFloat(emptyFloat,3,SHADER_SHADOW_MAP,"u_CameraPosition");

 	/** Light Parameters **/
   // mPostProcessingModel->setUniformFloat(mNear,SHADER_SHADOW_MAP,"u_LightNear");
   // mPostProcessingModel->setUniformFloat(mFar,SHADER_SHADOW_MAP,"u_LightFar");


    /** Camera Parameters **/
    mPostProcessingModel->setUniformFloat(1,SHADER_SHADOW_MAP,"u_CameraNear");
    mPostProcessingModel->setUniformFloat(80,SHADER_SHADOW_MAP,"u_CameraFar");
#endif
}

void PostProcessing::setCameraInfo(vec3 * cameraEye, vec3* cameraCenter){
    mCameraCenter  = cameraCenter;
    mCameraEye     = cameraEye;
}

void PostProcessing::lightLookAt(vec3 lightPos, vec3 lightDir){

    mat4 trans = T(lightPos.x,lightPos.y,lightPos.z);

    /** Upload Texture matrix for light */
	vec3 lightUp  = vec3(0,1,0);
	/** View **/
	mLightViewMatrix 	   = lookAtv(lightPos,lightDir,lightUp);
    /** View projection **/
	mVPLightMatrix	= mLightProjectionMatrix* mLightViewMatrix;
	/** Model **/
	mat4 rot   = Ry((-mTime-3.14159265359));
	mModelLightMatrix = trans*rot;

	/** MVP **/
	mMVPLightMatrix = mVPLightMatrix * mModelLightMatrix;

	/** Texture Matrix **/
	mLightTextureMatrix = T(0.5, 0.5, 0.0)* S(0.5, 0.5, 1.0)*mMVPLightMatrix;
	mPostProcessingModel->replaceUniformMatrix(mLightTextureMatrix,SHADER_LIGHT_VOLUME,"LightTextureMatrix");

    /** Update model to world matrix for the light volume model **/
    mat4 * lightTransform = mPostProcessingModel->getTransform(SHADER_LIGHT_VOLUME);
    *lightTransform = mModelLightMatrix;

}

void PostProcessing::drawShadows(mat4 proj, mat4 view){
	/** Upload MVP-matrix for the light perspective **/
    /** Computing camera vectors **/
	vec3 camUp      = vec3(0,1,0);
	vec3 camLook    = Normalize(*mCameraCenter - *mCameraEye);
	vec3 camRight   = Normalize(CrossProduct(camLook,camUp));
	camUp           = Normalize(CrossProduct(camRight,camLook));

    float camPos[] = {mCameraEye->x,mCameraEye->y,mCameraEye->z};
    float right[] = {camRight.x,camRight.y,camRight.z};
    float up[] = {camUp.x,camUp.y,camUp.z};
    float look[] = {camLook.x,camLook.y,camLook.z};

    mPostProcessingModel->replaceUniformFloat(right,SHADER_SHADOW_MAP,"u_CameraRight");
    mPostProcessingModel->replaceUniformFloat(up,SHADER_SHADOW_MAP,"u_CameraUp");
    mPostProcessingModel->replaceUniformFloat(look,SHADER_SHADOW_MAP,"u_CameraLook");
    mPostProcessingModel->replaceUniformFloat(camPos,SHADER_SHADOW_MAP,"u_CameraPosition");

    mat4 shadowTextureMatrix = T(0.5, 0.5, 0.0)* S(0.5, 0.5, 1.0)*mVPLightMatrix;
    mPostProcessingModel->replaceUniformMatrix(shadowTextureMatrix,SHADER_SHADOW_MAP,"LightTextureMatrix");

    /** Enable blend **/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mPostProcessingModel->draw(SHADER_SHADOW_MAP,proj,view);
    glDisable(GL_BLEND);

}

void PostProcessing::draw(mat4 proj, mat4 view){

    mTime+=mLightSpeed;
    GLfloat x = mLightRadius*sin(mTime);
    GLfloat z = mLightRadius*cos(mTime);
    lightLookAt(vec3(x,mLightHeight,z),vec3(0,mLightHeight,0));

    drawLightDepth(proj,view);
#ifndef DEBUG
    drawLightVolume(proj,view);

 //  mPostProcessingModel->draw(SHADER_SPHERE,proj,view);

	/** Select screen as render target*/
	glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0, 0, *mScreenWidth, *mScreenHeight);

    /** Draw light onto screen **/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mPostProcessingModel->draw(SHADER_SCREEN_QUAD,proj,view);
    glDisable(GL_BLEND);
#else
    /** Select screen as render target*/
	glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0, 0, *mScreenWidth, *mScreenHeight);
    mPostProcessingModel->draw(SHADER_LIGHT_VOLUME,proj,view);
#ifdef SHADOW_MAP
    drawShadows(proj,view);
#endif
#endif

}


void PostProcessing::drawLightVolume(mat4 proj, mat4 view){

/*
    mPostProcessingModel->replaceUniform(mScreenWidth,"u_ScreenWidth");
    mPostProcessingModel->replaceUniform(mScreenHeight,"u_ScreenHeight");*/



    /** Set use light FBO */
    useFBO(mLightFBO,NULL,NULL);
    glViewport(0, 0, mFBOSize, mFBOSize);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    /** Disable culling and depth test **/
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    /** Enable blend function, with additive blending **/

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

	mPostProcessingModel->draw(SHADER_LIGHT_VOLUME,proj,view);

	/** Reset settings **/

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

}




void PostProcessing::drawLightDepth(mat4 proj, mat4 view){
	/** Swap to light depth buffer */
	glBindFramebuffer(GL_FRAMEBUFFER,mLightDepthFBO.fb);
	glViewport(0, 0, mFBOSize, mFBOSize);
    /** Clear previous frame values */
    glClear( GL_DEPTH_BUFFER_BIT);
    /** Disable color rendering, we only want to write to the Z-Buffer */
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	/** Render scene from light perspective */
    mTerrain->drawSimple(mLightProjectionMatrix,mLightViewMatrix);

    /** Swap to scene depth buffer */
	glBindFramebuffer(GL_FRAMEBUFFER,mSceneDepthFBO.fb);
	glViewport(0, 0, mFBOSize, mFBOSize);
    /** Clear previous frame values */
    glClear( GL_DEPTH_BUFFER_BIT);
    /** Disable color rendering, we only want to write to the Z-Buffer */
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	/** Render scene from camera perspective */
	mTerrain->drawSimple(proj,view);

	/** Enable again*/
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}


/** Following code comes from TSBK03 literature */
void PostProcessing::generateDepthFBO(FBOstruct * fbo, GLuint w ,GLuint h) {

    // Try to use a texture depth component
    glGenTextures(1, &fbo->depth);
    glBindTexture(GL_TEXTURE_2D, fbo->depth);
    // GL_LINEAR does not make sense for depth texture.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Clamp to avoid artefacts on the edges of the shadowmap

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create a framebuffer object
    glGenFramebuffers(1, &fbo->fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);

    // Instruct OpenGL that we won't bind a color texture to the FBO
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    // attach the texture to FBO depth attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER,
    GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, fbo->depth, 0);
    // check FBO status
    GLenum FBOstatus;
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE)
        printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");
    // switch back to window-system-provided framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void PostProcessing::uploadSquareModelData(ModelObject * modelObj,GLuint shaderId){
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

    GLuint squareIndices[] = {0, 2, 1, 0, 3, 2};
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
*
* Create a light frustum mesh.
*
**/
void PostProcessing::generateFrustumMesh(GLfloat farVal, GLfloat nearVal, GLuint dimension){


	// TODO SCALE FAR- AND FEAR FACES


	GLuint vertexCount = 2*dimension*dimension;
	int triangleCount = (dimension-1) * (dimension-1) * 2 * 2 + 4*2*(dimension-1);
	GLuint x, y;


  //  GLfloat *vertexArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	///GLuint *indexArray = (GLuint*)malloc(sizeof(GLuint) * triangleCount*3);
	GLfloat vertexArray [3 * vertexCount];
	GLuint indexArray[triangleCount*3];
	GLfloat res = dimension-1;

	GLuint N;
	GLuint SideOffset  =  6*(dimension-1);
	GLuint PlaneOffset = SideOffset*(dimension-1);
	GLuint SideIndexStep = dimension;
	GLuint PlaneIndexStep = dimension*dimension;
	GLuint TrianglesPerQuad = 6;

	/* Near face vertices */
	GLfloat xVal = 0.0f;
	GLfloat yVal = 0.0f;
	GLfloat zVal = 0.0f;
	for (x = 0; x < dimension; x++)
		for (y = 0; y < dimension; y++)
		{

            xVal = mScaleNear*(x/res - 0.5)*2.0;
            yVal = mScaleNear*(y/res - 0.5)*2.0;
            zVal = mOffsetNear;
            GLuint startIndex = (x + y * dimension)*3;
			vertexArray[startIndex + 0] = xVal;
			vertexArray[startIndex + 1] = yVal;
			vertexArray[startIndex + 2] = zVal;

		}


	/* Indices for the the near face */
	for (x = 0; x < dimension-1; x++)
		for (y = 0; y < dimension-1; y++)
		{

			GLuint startIndex = (x + y * (dimension-1))*6;
			indexArray[startIndex + 0] = x + y * dimension;
			indexArray[startIndex + 1] = x + (y+1) * dimension;
			indexArray[startIndex + 2] = x+1 + y * dimension;

			indexArray[startIndex + 3] = x+1 + y * dimension;
			indexArray[startIndex + 4] = x + (y+1) * dimension;
			indexArray[startIndex + 5] = x+1 + (y+1) * dimension;
		}

	/* Far face vertices */
	for (x = 0; x < dimension; x++)
		for (y = 0; y < dimension; y++)
		{

            xVal = mScaleFar*farVal*(x/res - 0.5f)*2.0f;
            yVal = mScaleFar*farVal*(y/res - 0.5f)*2.0f;
            zVal = mOffsetFar;

			GLuint startIndex = (x + y * dimension)*3 + 3*dimension*dimension;
			vertexArray[startIndex + 0] = xVal;
			vertexArray[startIndex + 1] = yVal;
			vertexArray[startIndex + 2] = zVal;

		}

	/* Indices for the the far face */
	for (x = 0; x < dimension-1; x++)
		for (y = 0; y < dimension-1; y++)
		{


			GLuint startIndex = (x + y * (dimension-1))*6 + PlaneOffset;
			indexArray[startIndex + 0] = x + y * dimension + PlaneIndexStep;
			indexArray[startIndex + 2] = x + (y+1) * dimension + PlaneIndexStep;
			indexArray[startIndex + 1] = x+1 + y * dimension + PlaneIndexStep;


			indexArray[startIndex + 3] = x+1 + y * dimension + PlaneIndexStep;
			indexArray[startIndex + 5] = x + (y+1) * dimension + PlaneIndexStep;
            indexArray[startIndex + 4] = x+1 + (y+1) * dimension + PlaneIndexStep;
		}
 	/* Indices for the faces connecting far and near plane */
	/* BOTTOM: */

	y = 0;

	GLuint BottomOffset = 2*PlaneOffset;
	for(x = 0; x < dimension-1; x++){
		N = BottomOffset + x*TrianglesPerQuad;
		//Triangle 1
		//  ____
		//  |  /
		//  | /
		//  |/

		indexArray[N + 0] = x;
		indexArray[N + 1] = x + PlaneIndexStep+1;
		indexArray[N + 2] = x + PlaneIndexStep;
		//    /|
		//   / |
		//  /__|

		indexArray[N + 3] = x;
		indexArray[N + 4] = x + 1;
		indexArray[N + 5] = x + PlaneIndexStep+1;

	}

	/*  RIGHT: */
	x = SideIndexStep-1;

	GLuint RightOffset = 2*PlaneOffset+SideOffset;
	for(y = 0; y < dimension-1; y++){
		N = RightOffset + y*TrianglesPerQuad;
		//Triangle 1
		//  ____
		//  |  /
		//  | /
		//  |/

		indexArray[N + 0] = x + (y + 0)*SideIndexStep;
		indexArray[N + 1] = x + (y + 1)*SideIndexStep+PlaneIndexStep;
		indexArray[N + 2] = x + (y + 0)*SideIndexStep+PlaneIndexStep;

		// Triangle 2
		//    /|
		//   / |
		//  /__|

		indexArray[N + 3] = x + (y + 0)*SideIndexStep;
		indexArray[N + 4] = x + (y + 1)*SideIndexStep;
		indexArray[N + 5] = x + (y + 1)*SideIndexStep+PlaneIndexStep;

	}

	/* Left: */
	x = 0;
	GLuint LeftOffset = 2*PlaneOffset+2*SideOffset;
	for(y = 0; y < dimension-1; y++){
		N = LeftOffset + y*TrianglesPerQuad;
		//Triangle 1
		//  ____
		//  |  /
		//  | /
		//  |/

		indexArray[N + 0] = x + (y + 0)*SideIndexStep;
		indexArray[N + 1] = x + (y + 0)*SideIndexStep+PlaneIndexStep;
		indexArray[N + 2] = x + (y + 1)*SideIndexStep+PlaneIndexStep;
		// Triangle 2
		//    /|
		//   / |
		//  /__|

		indexArray[N + 3] = x + (y + 0)*SideIndexStep;
		indexArray[N + 4] = x + (y + 1)*SideIndexStep+PlaneIndexStep;
		indexArray[N + 5] = x + (y + 1)*SideIndexStep;

	}
	/* TOP: */
	y = SideIndexStep -1;

	GLuint TopOffset = 2*PlaneOffset+3*SideOffset;
	for(x = 0; x < dimension-1; x++){
		N = TopOffset + x*TrianglesPerQuad;
		//Triangle 1
		//  ____
		//  |  /
		//  | /
		//  |/

		indexArray[N + 0] = x + (y + 0)*SideIndexStep;
		indexArray[N + 1] = x + (y + 0)*SideIndexStep + PlaneIndexStep;
		indexArray[N + 2] = x + (y + 0)*SideIndexStep + PlaneIndexStep+1;
		//    /|
		//   / |
		//  /__|

		indexArray[N + 3] = x + (y + 0)*SideIndexStep;
		indexArray[N + 4] = x + (y + 0)*SideIndexStep + PlaneIndexStep+1;
		indexArray[N + 5] = x + (y + 0)*SideIndexStep + 1;

	}

	// End of Light frustum generation
	// Create Model and upload to GPU:
	mPostProcessingModel->LoadDataToModel(
			vertexArray,
			NULL,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3,
			SHADER_LIGHT_VOLUME
			);

    /** Clean up **/
//    free(vertexArray);
//    free(indexArray);
	printError("PostProcessing Buffer");

}


PostProcessing::~PostProcessing()
{
    //dtor
}



