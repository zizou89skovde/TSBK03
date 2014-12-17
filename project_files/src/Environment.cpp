#include "Environment.h"

Environment::Environment(GLuint * w, GLuint * h)
{

    mScreenWitdh = w;
    mScreenHeight = h;
    mDefaultFBO = NULL;

    mEnvironmentModel = new ModelObject();
    /** Initialize FBO's  used for rendering the water reflection **/
    mReflectionFBO    = initFBO(mFBOWidth,mFBOHeight,0);
    mRefractedFBO     = initFBO(mFBOWidth,mFBOHeight, 0);
    initializeSkyBox();
    initializeEnvironment();


}

void Environment::initializeEnvironment(){

    /**
        These are common resource. Used by multiple classes. Environment-class
        holds this  terrain meta data so it can be distributes among other classes
    **/

    /** Load texture **/
    TextureData texData;
    LoadTGATexture((char *)"textures/fft-terrain4.tga",&texData);
    mEnvironmentMetaData.sHeightMapHandle = texData.texID;
    mEnvironmentMetaData.setTextureSize(texData.w,texData.h);
    LoadTGATextureSimple((char *)"textures/fft-terrain4_hd.tga",&mEnvironmentMetaData.sHeightMapHandleHighRes);
    LoadTGATextureSimple((char *)"textures/fft-terrain4_normal_hd.tga",&mEnvironmentMetaData.sNormalMapHandleHighRes);

    free(texData.imageData);

    /** Set size of the environment **/
    mEnvironmentMetaData.setSize(20.0f,5.0f,20.0f);

    /** Set offset of the environment **/
    mEnvironmentMetaData.setOffset(.0f,.0f,.0f);


}

EnvironmentMetaData Environment::getMetaData(){
    return mEnvironmentMetaData;
}

void Environment::setDefaultFBO(FBOstruct* fbo){
    mDefaultFBO = fbo;
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


    //setReflectedModels(mEnvironmentModel,SKYBOX_SHADER);

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


void Environment::setRefractedModels(ModelObject * modelObj,GLuint shaderId){
    mRefracedModelMap.push_back(std::make_pair(shaderId,modelObj));
}

FBOstruct * Environment::getReflectedFBO(){
    return mReflectionFBO;
}
FBOstruct * Environment::getRefractionFBO(){
    return mRefractedFBO;
}

void Environment::setClip(bool enabled){
    GLfloat flip = -1.0;
    if(enabled){
        mEnvironmentModel->replaceUniformFloat(&flip,SKYBOX_SHADER,"u_Flip");
    }else{
        flip = 1.0;
        mEnvironmentModel->replaceUniformFloat(&flip,SKYBOX_SHADER,"u_Flip");
    }
}


void Environment::drawReflectedModels(mat4 projectionMatrix,mat4 viewMatrix){
    ModelObject * modelObject;
    GLuint shaderId;
    /** Draw reflected ***/
    useFBO(mReflectionFBO,NULL,NULL);
    glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glDisable(GL_CULL_FACE);

    /** Draw skybox upside down. Clipped at the Y-axis **/
    glEnable(GL_CLIP_DISTANCE0);
    setClip(true);
    mEnvironmentModel->draw(SKYBOX_SHADER,projectionMatrix,viewMatrix);
    setClip(false);


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
    glDisable(GL_CLIP_DISTANCE0);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


    if(mDefaultFBO == NULL){
          glBindFramebuffer(GL_FRAMEBUFFER, 0);
          glViewport(0, 0, *mScreenWitdh, *mScreenHeight);
    } else{
          glBindFramebuffer(GL_FRAMEBUFFER, mDefaultFBO->fb);
          glViewport(0, 0, mDefaultFBO->width, mDefaultFBO->height);
    }



}



void Environment::drawRefractedModels(mat4 projectionMatrix,mat4 viewMatrix){
    /** Draw to Offscreen fbo */
    useFBO(mRefractedFBO,NULL,NULL);

	ModelObject * modelObject;
    GLuint shaderId;

    glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    for(ModelIterator it = mRefracedModelMap.begin(); it != mRefracedModelMap.end(); ++it) {
        ModelItem item = *it;
        /** Read pair **/
        shaderId    = item.first;
        modelObject = item.second;
        modelObject->draw(shaderId,projectionMatrix,viewMatrix);
    }
    if(mDefaultFBO == NULL){
          glBindFramebuffer(GL_FRAMEBUFFER, 0);
          glViewport(0, 0, *mScreenWitdh, *mScreenHeight);
    } else{
          glBindFramebuffer(GL_FRAMEBUFFER, mDefaultFBO->fb);
          glViewport(0, 0, mDefaultFBO->width, mDefaultFBO->height);
    }

}

void Environment::drawDepthModels(mat4 projectionMatrix,mat4 viewMatrix){

    ModelObject * modelObject;
    GLuint shaderId;
    glDisable(GL_CULL_FACE);
    for(ModelIterator it = mDepthModelMap.begin(); it != mDepthModelMap.end(); ++it) {

        ModelItem item = *it;
        /** Read pair **/
        shaderId    = item.first;
        modelObject = item.second;

        modelObject->draw(shaderId,projectionMatrix,viewMatrix);
    }
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

}

void Environment::draw(mat4 proj, mat4 view){
    mEnvironmentModel->draw(SKYBOX_SHADER,proj,view);
    drawRefractedModels(proj,view);
    drawReflectedModels(proj,view);

}




