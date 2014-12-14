#include "Environment.h"

Environment::Environment(GLuint * w, GLuint * h)
{
    mScreenWitdh = w;
    mScreenHeight = h;
    mEnvironmentModel = new ModelObject();


    /** Initialize FBO's  used for rendering the water reflection **/
    mEnvironmentReflectionFBO   = initFBO(mFBOWidth,mFBOHeight,0);
    mEnvironmentFBO             = initFBO(mFBOWidth,mFBOHeight, 0);

    initializeSkyBox();


}



void Environment::initializeSkyBox(){
    /** Setting Sky dome shader **/
    GLuint skyDomeShader = loadShaders("shaders/skydome.vert","shaders/skydome.frag");
    mEnvironmentModel->setShader(skyDomeShader,SKYBOX_SHADER,VP,NO_DEPTH_TEST);

    /** Set sky dome texture **/
    GLuint skyDomeTexture;
    LoadTGATextureSimple((char *)"textures/skydome.tga",&skyDomeTexture);
    mEnvironmentModel->setTexture(skyDomeTexture,SKYBOX_SHADER,"u_Texture");

    /** Upload sky dome model **/
    Model* modelSkyDome = LoadModelPlus((char *)"models/skydome.obj");
	free(modelSkyDome->normalArray);
	modelSkyDome->normalArray = NULL;
    mEnvironmentModel->setModel(modelSkyDome,SKYBOX_SHADER);
    mEnvironmentModel->freeModelData(modelSkyDome);
    /** Set flip **/
    mEnvironmentModel->setUniformFloat(1.0f,SKYBOX_SHADER,"u_Flip");


}

Environment::~Environment()
{
    //dtor
}

void Environment::setReflectedModels(ModelObject * modelObj,GLuint shaderId){
    mReflectionModelMap.push_back(std::make_pair(shaderId,modelObj));
}

void Environment::setDepthModels(ModelObject * modelObj,GLuint shaderId){
    mDepthModelMap.push_back(std::make_pair(shaderId,modelObj));
}

void Environment::drawReflectedModels(mat4 projectionMatrix,mat4 viewMatrix){
    ModelObject * modelObject;
    GLuint shaderId;
        /** Draw reflected ***/
/*    useFBO(mEnvirioReflectionFBO,NULL,NULL);
    glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4* transformTerrain = mTerrainModel->getTransform(TERRAIN_SHADER);
  // Draw scene upside down //
    *transformTerrain = S(1,-1,1);
    glEnable(GL_CLIP_DISTANCE0);
    setClip(true);
    glDisable(GL_CULL_FACE);
    mTerrainModel->draw(SKYBOX_SHADER,proj,view);

    drawReflectedModels(proj,view);

    glDisable(GL_CLIP_DISTANCE0);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
*/
    for(ModelIterator it = mReflectionModelMap.begin(); it != mReflectionModelMap.end(); ++it) {

        ModelItem item = *it;

        /** Read pair **/
        shaderId    = item.first;
        modelObject = item.second;

        /** Flip model in regard to the Y-axis **/
        modelObject->flipModels(shaderId);

        modelObject->draw(shaderId,projectionMatrix,viewMatrix);

        /** Reset model to its original direction **/
        modelObject->flipModels(shaderId);
    }
}

FBOstruct * Environment::getEnvironmentReflectedFBO(){
    return mEnvironmentReflectionFBO;
}

FBOstruct * Environment::getEnvironmentFBO(){
    return mEnvironmentFBO;
}


void Environment::drawRefractedModels(mat4 projectionMatrix,mat4 viewMatrix){
    /** Draw to Offscreen fbo */
    /*setClip(false);
    useFBO(mTerrainFBO,NULL,NULL);
    glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat sizeTerrain = 1.00;
    *transformTerrain = S(sizeTerrain,sizeTerrain,sizeTerrain);
    mTerrainModel->draw(TERRAIN_SHADER,proj,view);
    */
}

void Environment::drawDepthModels(mat4 projectionMatrix,mat4 viewMatrix){

  //  mTerrainDepthModel->draw(TERRAIN_SIMPLE_SHADER,projectionMatrix,viewMatrix);

    ModelObject * modelObject;
    GLuint shaderId;
    glDisable(GL_CULL_FACE);
    for(ModelIterator it = mDepthModelMap.begin(); it != mDepthModelMap.end(); ++it) {

        ModelItem item = *it;
        // Read pair //
        shaderId    = item.first;
        modelObject = item.second;

        modelObject->draw(shaderId,projectionMatrix,viewMatrix);
    }
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

}

void Environment::draw(mat4 proj, mat4 view){
    /** Draw to screen **/
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, *mScreenWitdh, *mScreenHeight);
    mEnvironmentModel->draw(SKYBOX_SHADER,proj,view);
}



