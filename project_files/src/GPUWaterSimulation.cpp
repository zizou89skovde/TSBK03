#include "GPUWaterSimulation.h"

GPUWaterSimulation::GPUWaterSimulation(GLuint * w,GLuint *h): GPUSimulation(w,h)
{



}

void GPUWaterSimulation::initialize(){
    /** Initializing GPU Simulation **/
    SimulationData_Type * simulationData = new SimulationData_Type();


    simulationData->GridDimension  = GPU_WATER_DIM;
    simulationData->GridSize       = GPU_WATER_SIZE;
    simulationData->GridOffset[0] = -1;
    simulationData->GridOffset[1] = -2;
    simulationData->GridOffset[2] = 0;
    simulationData->isUpward       = false;

    const char * vert = "shaders/water_verlet.vert";
    const char * frag = "shaders/water_verlet.frag";
    memset(simulationData->VertexShader,0,simulationData->MAX_LEN_STRING);
    memset(simulationData->FragmentShader,0,simulationData->MAX_LEN_STRING);
    strcpy(simulationData->VertexShader,vert);
    strcpy(simulationData->FragmentShader,frag);

    intializeSimulation(simulationData);
    setSimulationConstant( simulationData->GridOffset[1],"u_SeaLevel");
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

    /** Upload Offscreen rendering of the terrain */
	FBOstruct * terrainFBO = mTerrain->getTerrainFBO();
    mGPUWaterScene->setTexture(terrainFBO->texid,GPU_SHADER_WATER,"u_TerrainColor");
    mGPUWaterScene->setTexture(terrainFBO->depth,GPU_SHADER_WATER,"u_TerrainDepth");
    /** Upload upside down rendering of the terrain  **/
    FBOstruct * terrainReflectionFBO = mTerrain->getTerrainReflectedFBO();
    mGPUWaterScene->setTexture(terrainReflectionFBO->texid,GPU_SHADER_WATER,"u_TerrainReflection");

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
    TextureData* textureData = mTerrain->getTextureData();
    setSimulationTexture(textureData->texID,"u_HeightMap");

    TerrainMetaData* terrainMeta = mTerrain->getTerrainMetaData();
    setSimulationConstant(terrainMeta->HeightScale, "u_TerrainHeight");
    setSimulationConstant(terrainMeta->TerrainSize, "u_TerrainSize");

    /** Upload system props **/
    setSimulationConstant(GpuSystemDeltaTime,"u_DeltaTime");
    setSimulationConstant(GpuSystemDamping,"u_SystemDamping");

    setSimulationConstant(GpuSpringDamping,"u_SpringDamping");
    setSimulationConstant(GpuSpringConstant,"u_SpringConstant");
    setSimulationConstant(GpuRestLength,"u_RestLength");

    /** Upload wind **/
	uploadTime(0.0079);



}


void GPUWaterSimulation::draw(mat4 projectionMatrix, mat4 viewMatrix){



    /** Computing shaders **/
    FBOstruct * resultFbo = simulate(1);

    /** Render Cloth **/
    mGPUWaterScene->replaceTexture(resultFbo->texids[0],"u_Position_Texture");
    mGPUWaterScene->replaceTexture(resultFbo->texids[1],"u_Normal_Texture");
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mGPUWaterScene->draw(projectionMatrix,viewMatrix);
    glBlendFunc (GL_ONE, GL_ZERO);
    glDisable (GL_BLEND);
    printError("GPU Cloth draw");

}






