#include "GPUClothSimulation.h"

GPUClothSimulation::GPUClothSimulation(GLuint * w,GLuint *h): GPUSimulation(w,h)
{

}

void GPUClothSimulation::initialize(){
    /** Initializing GPU Simulation **/
    SimulationData_Type * simulationData = new SimulationData_Type();
    //simulationData->VertexShader   = "shaders/mass_spring_verlet.vert";
    //simulationData->FragmentShader = "shaders/mass_spring_verlet.frag";
    const char * vert = "shaders/mass_spring_verlet.vert";
    const char * frag = "shaders/mass_spring_verlet.frag";
    memset(simulationData->VertexShader,0,simulationData->MAX_LEN_STRING);
    memset(simulationData->FragmentShader,0,simulationData->MAX_LEN_STRING);
    memset(simulationData->FragmentShader,0,simulationData->MAX_LEN_STRING);
    strcpy(simulationData->VertexShader,vert);
    strcpy(simulationData->FragmentShader,frag);
    simulationData->GridDimension  = GPU_CLOTH_DIM;
    simulationData->GridSize       = GPU_CLOTH_SIZE;
    simulationData->GridOffset[0] = 0;
    simulationData->GridOffset[1] = 2;
    simulationData->GridOffset[2] = 0;
    simulationData->isUpward       = true;
    intializeSimulation(simulationData);
    configureSimulation();

    mGPUClothScene = new ModelObject();
    mTerrain->setExternalModels(mGPUClothScene);
    /** Setting Cloth model shader **/
    //GLuint clothModelShader = loadShadersG("shaders/cloth.vert","shaders/cloth.frag","shaders/cloth.gs");
    GLuint clothModelShader = loadShaders("shaders/cloth_phong.vert","shaders/cloth_phong.frag");
    mGPUClothScene->setShader(clothModelShader,GPU_SHADER_CLOTH,MVP);

    /** Sphere Shader **/
    GLuint sphereShader = loadShaders("shaders/sphere.vert", "shaders/sphere.frag");
    mGPUClothScene->setShader(sphereShader,GPU_SHADER_SPHERE);

    /** Set initial transform **/
    mGPUClothScene->setTransform(IdentityMatrix(),GPU_SHADER_CLOTH);

    /** Uploading Texture coordinates **/
    uploadBufferCoordinates(mGPUClothScene,GPU_SHADER_CLOTH);

    /** Set initial textures **/
    mGPUClothScene->setTexture(getActiveFBO()->texids[0],GPU_SHADER_CLOTH,"u_MassPos_Tex");

    /** Upload grid resolution **/
    GLfloat meta[2];
	meta[0] = GPU_CLOTH_DIM;
	meta[1] = GPU_CLOTH_SIZE;
    mGPUClothScene->setUniformFloat(meta,2,GPU_SHADER_CLOTH,"u_Resolution");
    setSimulationConstant(meta,2,(const char*) "u_Resolution");

    /** Sphere - For collision **/
    Sphere * sphere = new Sphere();
    sphere->position = new vec3(mSphereOffsetX,mSphereOffsetY,mSphereOffsetZ);
    sphere->radius = mSphereRadius;
    mSpheres.push_back(sphere);

    /** Sphere Model **/
    Model* modelSphere = LoadModelPlus((char *)"models/sphere.obj");
    mGPUClothScene->setModel(modelSphere,GPU_SHADER_SPHERE);
    mGPUClothScene->freeModelData(modelSphere);



    /** Sphere Transform **/
    mObjectDirection = 1.0;
    GLfloat r = sphere->radius;
    vec3  pos = *sphere->position;
    mat4 transform2 = T(pos.x,pos.y,pos.z)*S(r,r,r);
    mGPUClothScene->setTransform(transform2,GPU_SHADER_SPHERE);

}

GPUClothSimulation::~GPUClothSimulation(){
//    ~mGPUClothScene;
}

void GPUClothSimulation::configureSimulation(){
    /** Upload system props **/
    setSimulationConstant(GpuSystemDeltaTime,"u_DeltaTime");
    setSimulationConstant(GpuSystemDamping,"u_SystemDamping");
    setSimulationConstant(GpuSystemGravity,"u_Gravity");

    /** Upload spring props **/
    setSimulationConstant(GpuSpringDamping,"u_SpringDamping");
    setSimulationConstant(GpuSpringConstantStruct,"u_SpringConstant");
    setSimulationConstant(GpuSpringConstantBend,"u_SpringConstantBend");
    setSimulationConstant(GpuRestLengthStruct,"u_RestLengthStruct");
    setSimulationConstant((const float)sqrt(2.0*GpuRestLengthStruct*GpuRestLengthStruct),"u_RestLengthShear");
    setSimulationConstant(GpuRestLengthBend,"u_RestLengthBend");

#ifdef WIND
    /** Upload wind **/
    mTime = 0;
    mWindVector[0] = 0;
    mWindVector[1] = 0;
    mWindVector[2] = 0;
    setSimulationConstant(mWindVector,3,"u_Wind");
#endif
    /** Sphere initial position */
    GLfloat dummyPos[3] = {0,0,-28};
    setSimulationConstant(dummyPos,3,"u_SpherePosition");
    setSimulationConstant(mSphereRadius,"u_SphereRadius");

}

void GPUClothSimulation::updateWind(){

    /** Update wind randomly**/
    mWindVector[0] = 0.004*sin(mTime);
    mWindVector[2] = 0.004*cos(0.5*mTime);
    mTime += 0.01;
    replaceSimulationConstant(mWindVector,"u_Wind");

}

void GPUClothSimulation::updateCollisionObject(){

    Sphere* s = mSpheres.at(0);
    if(abs(s->position->z) >mSphereRouteLength){
       mObjectDirection*= -1.0;
    }
    vec3 deltaPos = vec3(0.0,0.0,mObjectDirection*mSphereSpeed);
    updateSpherePosition(deltaPos);
}

void GPUClothSimulation::draw(mat4 projectionMatrix, mat4 viewMatrix){

#ifdef WIND
    updateWind();
#endif
    /** Computing shaders **/
    FBOstruct * resultFbo = simulate(5);

    /** Update update sphere */
    updateCollisionObject();

    /** Render Cloth **/
    mGPUClothScene->replaceTexture(resultFbo->texids[0],GPU_SHADER_CLOTH,(const char *)"u_MassPos_Tex");

    /** No culling. In order to se the back face of the cloth **/
    glDisable(GL_CULL_FACE);
    mGPUClothScene->draw(projectionMatrix,viewMatrix);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
    printError("GPU Cloth draw");

}

void GPUClothSimulation::updateSpherePosition(vec3 deltaPos){
    /** Update position of sphere **/
    Sphere * s = mSpheres.at(0);
    *s->position += deltaPos;
    /** Update computation uniform **/
    replaceSimulationConstant((GLfloat*)s->position,(const char*)"u_SpherePosition");
    /** Update model transform **/
    mat4 * transf = mGPUClothScene->getTransform(GPU_SHADER_SPHERE);
    GLfloat r = s->radius;
    vec3  pos = *s->position;

    *transf = T(pos.x,pos.y,pos.z)*S(r,r,r);

}




