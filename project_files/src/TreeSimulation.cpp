#include "TreeSimulation.h"

TreeSimulation::TreeSimulation()
{
}

void TreeSimulation::initialize(){

    /** Assign default values **/
    previousTime = -1;

	/** Initalize Tree model object **/
    mTreeScene = new ModelObject(); //Notera: ModelObject är lite feldöpt. Den borde heta Scene object. Eftersom den kan hålla flera olika modeller/shaders osv.

	/** Load shader **/
	GLuint TreeShader = loadShadersG("shaders/tree.vert", "shaders/tree.frag", "shaders/tree.gs");
	mTreeScene->setShader(TreeShader, TREE_SHADER_ID,VP);

    /**  Upload buffer coordinates **/
    uploadBufferCoordinates(mTreeScene,TREE_SHADER_ID);

    /** Upload terrain parameters **/
    TerrainMetaData* terrainData = mTerrain->getTerrainMetaData();
    //mTreeScene->setUniform(1.0f, TREE_SHADER_ID, "u_Wind");

	angle = 0.0f;

	/** Translate and scale the tree **/
/*
    GLfloat scaleFactor = 10.0;
    vec3  pos = vec3(0.0, 0.0,0.0);
    mat4 transform2 = T(pos.x,pos.y,pos.z)*S(scaleFactor, scaleFactor, scaleFactor);
    mTreeScene->setTransform(transform2, TREE_SHADER_ID);
*/
	/***** START WATCH OUT **********************************************************************************/

    GLfloat * gridOffset = (GLfloat*)malloc(sizeof(GLfloat)*3);
    gridOffset[0] = -terrainData->TerrainSize/2.0;
    gridOffset[1] = 0;
    gridOffset[2] = -terrainData->TerrainSize/2.0;
    mTreeScene->setUniform(gridOffset,3,TREE_SHADER_ID,"u_GridOffset");
    //mTreeScene->setUniform(terrainData->TerrainResolution,"u_GridResolution");
    mTreeScene->setUniform(terrainData->TerrainSize,TREE_SHADER_ID,"u_GridSize");
    mTreeScene->setUniform(terrainData->HeightScale,TREE_SHADER_ID,"u_GridHeightScale");

    /** Set hight map texture **/
    mTreeScene->setTexture(mTerrain->getTextureData()->texID,TREE_SHADER_ID,"u_HeightMap");

	/***** END WATCH OUT **********************************************************************************/

    /** Noise **/
    GLuint TreeNoiseTexture;
    LoadTGATextureSimple((char*)"textures/noise.tga", &TreeNoiseTexture);
    mTreeScene->setTexture(TreeNoiseTexture,TREE_SHADER_ID,"u_TreeNoise");
}

void TreeSimulation::setTerrain(Terrain * terrain){
    mTerrain = terrain;
}

void TreeSimulation::uploadBufferCoordinates(ModelObject* modelobject,GLuint shaderId) {

	

    GLfloat triangle[] = {
						-10, 0, -10,
    	                -10, 0, -10,
    	                0, 0, -10};

	

    GLuint triangleIndices[] = {0, 1, 2};
    modelobject->LoadDataToModel(
        triangle,
        NULL,
        NULL,
        NULL,
        triangleIndices,
        2,
        3,
        shaderId);

    printError("Tree Gen Position Buffer Coordinates ");

}

void TreeSimulation::draw(mat4 projectionMatrix,mat4 viewMatrix){
    mTreeScene->draw(projectionMatrix,viewMatrix);
}

void TreeSimulation::update(){
	
	GLfloat dt = 0.005f;
	GLfloat pi = 3.141592653589793;
	
	angle += dt;
	if (angle >= 2*pi) {
		angle = 0;
	}
	printf("Vind: %f \n", angle);
	mTreeScene->replaceUniform(&angle,"u_Wind");


}

TreeSimulation::~TreeSimulation()
{
    //dtor
}
