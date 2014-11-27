#include "GPUWaterSimulation.h"

GPUWaterSimulation::GPUWaterSimulation(GLuint * w,GLuint *h, const char * vertexShader, const char * fragmentShader): GPUSimulation(w,h,vertexShader,fragmentShader)
{

    /** Initializing GPU Simulation **/
    SimulationData_Type * simulationData = new SimulationData_Type();
    simulationData->VertexShader   = vertexShader;
    simulationData->FragmentShader = fragmentShader;
    simulationData->GridDimension  = GPU_WATER_DIM;
    simulationData->GridSize       = GPU_WATER_SIZE;
    simulationData->GridOffset[0] = 0;
    simulationData->GridOffset[1] = 0;
    simulationData->GridOffset[2] = 0;
    simulationData->isUpward       = false;

    intializeSimulation(simulationData);

    configureSimulation();

    mGPUWaterScene = new ModelObject();

    /** Setting Cloth model shader **/
    GLuint clothModelShader = loadShaders("shaders/water_phong.vert","shaders/water_phong.frag");
    mGPUWaterScene->setShader(clothModelShader,GPU_SHADER_WATER,VP);

    /** Uploading Texture coordinates **/
    uploadBufferCoordinates(mGPUWaterScene,GPU_SHADER_WATER);

    /** Set initial textures **/
    mGPUWaterScene->setTexture(DUMMY_TEXTURE,GPU_SHADER_WATER,"u_Position_Texture");
    mGPUWaterScene->setTexture(DUMMY_TEXTURE,GPU_SHADER_WATER,"u_Normal_Texture");



    /** Upload grid resolution **/
    GLfloat * meta = (GLfloat*)malloc(sizeof(GLfloat)*2);
	meta[0] = GPU_WATER_DIM;
	meta[1] = GPU_WATER_SIZE;
    mGPUWaterScene->setUniform(meta,2,GPU_SHADER_WATER,"u_Resolution");
    setSimulationConstant(meta,2,(const char*) "u_Meta");

}

GPUWaterSimulation::~GPUWaterSimulation(){
//    ~mGPUClothScene;
}

void GPUWaterSimulation::configureSimulation(){

    /** Upload ground heightmap **/
    GLuint texId;
    LoadTGATextureSimple((char*)"textures/terrain2.tga",&texId);
    setSimulationTexture(texId,"u_HeightMap");

    /** Upload system props **/
    setSimulationConstant(GpuSystemDeltaTime,"u_DeltaTime");
    setSimulationConstant(GpuSystemDamping,"u_SystemDamping");

    /** Upload wind **/
    mTime = 0;
    setSimulationConstant(mTime,"u_Time");

}


void GPUWaterSimulation::draw(mat4 projectionMatrix, mat4 viewMatrix){

    mTime += 0.001;
    replaceSimulationConstant(mTime,"u_Time");

    /** Computing shaders **/
    FBOstruct * resultFbo = simulate(1);

    /** Render Cloth **/
    mGPUWaterScene->replaceTexture(resultFbo->texids[0],"u_Position_Texture");
    mGPUWaterScene->replaceTexture(resultFbo->texids[1],"u_Normal_Texture");
    mGPUWaterScene->draw(projectionMatrix,viewMatrix);

    printError("GPU Cloth draw");

}






