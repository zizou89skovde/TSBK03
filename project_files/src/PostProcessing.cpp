#include "PostProcessing.h"

PostProcessing::PostProcessing(GLuint * w, GLuint * h)
{
    mScreenWidth = w;
    mScreenHeight = h;
  
    initializePostProcessing();


}

void PostProcessing::initializePostProcessing(){

	/** Create model object */
  	mPostProcessingModel = new ModelObject();	

	/** Load light volume shader */
	GLuint lightVolumeShader = loadShadersG("shaders/lightvolume.vert","shaders/lightvolume.frag","shaders/lightvolume.gs");
	mLightShaderHandle = lightVolumeShader;
//	GLuint lightVolumeShader = loadShaders("shaders/lightvolume.vert","shaders/lightvolume.frag");
    mPostProcessingModel->setShader(lightVolumeShader,SHADER_LIGHT_VOLUME,VP);
	
	/** Generate frustum mesh **/
	generateFrustumMesh(mFar,mNear,16);

	mLightDepthFBO = initFBO2(mFBOSize,mFBOSize,0,1);


	/* Set light depth buffer as texture for the light volume shader */
	mPostProcessingModel->setTexture(mLightDepthFBO->depth,SHADER_LIGHT_VOLUME,"u_DepthMap");

	/*Upload MVP-matrix for light */
	vec3 lightPos = vec3(0,0,0);
	vec3 lightDir = vec3(0,0,1);
	vec3 lightUp  = vec3(0,1,0);
	mLightViewMatrix 	   = lookAtv(lightPos,lightDir,lightUp);
	mLightProjectionMatrix = perspective(90, mRatio, mNear, mFar);
	mVPLightMatrix	= mLightProjectionMatrix* mLightViewMatrix;
 	glUniformMatrix4fv(glGetUniformLocation(lightVolumeShader, "VP_LightMatrix"), 1, GL_TRUE, mVPLightMatrix.m);


/////////////////////////////////////////////////////////////////

	/** Sphere Model **/
    Model* modelSphere = LoadModelPlus((char *)"models/skydome.obj");
	free(modelSphere->texCoordArray);
	modelSphere->texCoordArray = NULL;
    mPostProcessingModel->setModel(modelSphere,SHADER_SPHERE);

    /** Sphere Shader **/
    GLuint sphereShader = loadShaders("shaders/sphere.vert", "shaders/sphere.frag");
    mPostProcessingModel->setShader(sphereShader,SHADER_SPHERE,MVP);

    /** Sphere Transform **/
    mat4 transform2 = T(0,0,6)*S(4,4,4);
    mPostProcessingModel->setTransform(transform2,SHADER_SPHERE);

}


void PostProcessing::draw(mat4 proj, mat4 view){
	drawLightDepth();
	
	mPostProcessingModel->draw(proj,view);
}

void PostProcessing::drawLightDepth(){
	/* Swap to light depth buffer */
	useFBO(mLightDepthFBO,NULL,NULL);
	glViewport(0, 0, mFBOSize, mFBOSize);
	


	/* Render scene from light position */
	mPostProcessingModel->draw(SHADER_SPHERE,mLightProjectionMatrix,mLightViewMatrix);
	
	/* Set to offscreen mode */
	useFBO(0,NULL,NULL);
    glViewport(0, 0, *mScreenWidth, *mScreenHeight);
}

/**
*
* Create a light frustum mesh. It is simply a cone..-..
*
**/
void PostProcessing::generateFrustumMesh(GLfloat far, GLfloat near, GLuint dimension){

	
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
	for (x = 0; x < dimension; x++)
		for (y = 0; y < dimension; y++)
		{

            float xVal = (x / res - 0.5) * 2.0;
            float yVal = (y / res - 0.5) * 2.0;
            float zVal = near;

			vertexArray[(x + y * dimension)*3 + 0] = xVal;
			vertexArray[(x + y * dimension)*3 + 1] = yVal;
			vertexArray[(x + y * dimension)*3 + 2] = zVal;

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

            float xVal = far/2.0*(x / res - 0.5) * 2.0;
            float yVal = far/2.0*(y / res - 0.5) * 2.0;
            float zVal = far;

			GLuint startIndex = (x + y * dimension)*3 + 3*dimension*dimension;
			vertexArray[startIndex + 0] = xVal;
			vertexArray[startIndex + 1] = yVal;
			vertexArray[startIndex + 2] = zVal;

		}

	/* Indices for the the far face */
	GLuint indexOffset = PlaneIndexStep;
	for (x = 0; x < dimension-1; x++)
		for (y = 0; y < dimension-1; y++)
		{


			GLuint startIndex = (x + y * (dimension-1))*6 + PlaneOffset;
			indexArray[startIndex + 0] = x + y * dimension + PlaneIndexStep;
			indexArray[startIndex + 1] = x + (y+1) * dimension + PlaneIndexStep;
			indexArray[startIndex + 2] = x+1 + y * dimension + PlaneIndexStep;

			indexArray[startIndex + 3] = x+1 + y * dimension + PlaneIndexStep;
			indexArray[startIndex + 4] = x + (y+1) * dimension + PlaneIndexStep;
			indexArray[startIndex + 5] = x+1 + (y+1) * dimension + PlaneIndexStep;

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
		indexArray[N + 1] = x + (y + 0)*SideIndexStep + PlaneIndexStep+1;
		indexArray[N + 2] = x + (y + 0)*SideIndexStep + PlaneIndexStep;
		//    /|	
		//   / |
		//  /__|

		indexArray[N + 3] = x + (y + 0)*SideIndexStep;
		indexArray[N + 4] = x + (y + 0)*SideIndexStep + 1;
		indexArray[N + 5] = x + (y + 0)*SideIndexStep + PlaneIndexStep+1;

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



