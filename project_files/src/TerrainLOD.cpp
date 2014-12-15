#include "TerrainLOD.h"

void TerrainLOD::initialize(){

	mTerrainLODScene = new ModelObject();

	/** Load shader **/
	GLuint shaderTerrainLOD = loadShadersGT("shaders/terrainLOD2.vert", "shaders/terrainLOD.frag", "shaders/terrainLOD.gs", "shaders/terrainLOD.tcs", "shaders/terrainLOD.tes");
	//GLuint shaderTerrainLOD = loadShadersGT("shaders/terrainLOD.vert", "shaders/terrainLOD.frag", NULL , "shaders/terrainLOD.tcs", "shaders/terrainLOD.tes");
	//GLuint shaderTerrainLOD = loadShadersG("shaders/terrainLOD.vert", "shaders/terrainLOD.frag", "shaders/terrainLOD.gs");

	//GLuint shaderTerrainLOD = loadShadersG("shaders/terrainLOD2.vert", "shaders/terrainLOD.frag", "shaders/terrainLOD.gs");
	mTerrainLODScene->setShader(shaderTerrainLOD,SHADER_TERRAIN_LOD);




	/** Load texture **/

	/** Load texture from disk **/

	TextureData heightMapTextureData;
	LoadTGATextureData((char*)"textures/fft-terrain4.tga",&heightMapTextureData);
	free(heightMapTextureData.imageData);
	mTerrainLODScene->setTexture(heightMapTextureData.texID,SHADER_TERRAIN_LOD,"u_HeightMap");

    /** Upload model data **/
    //uploadSquareModelData(mTerrainLODScene,SHADER_TERRAIN_LOD);
    generateLODBuffers(mTerrainLODScene,SHADER_TERRAIN_LOD);

	/** Upload size of the texture **/
	mTextureSize[0] = heightMapTextureData.w;
	mTextureSize[1] = heightMapTextureData.h;
	mTerrainLODScene->setUniformFloat(mTextureSize,2,SHADER_TERRAIN_LOD,"u_TextureSize");

	/** Select drawInstanced-method and set number of instances **/
	//mTerrainLODScene->setDrawMethod(INSTANCED,SHADER_TERRAIN_LOD);
	mTerrainLODScene->setDrawMethod(PATCHES,SHADER_TERRAIN_LOD);
	//mTerrainLODScene->setNumInstances(mTextureSize[0]*mTextureSize[1],SHADER_TERRAIN_LOD);

	/** Set model to world transform **/
	mTerrainLODScene->setTransform(IdentityMatrix(),SHADER_TERRAIN_LOD);


}

TerrainMetaData * TerrainLOD::getTerrainMetaData(){
    return mTerrainMetaData;
}

TextureData * TerrainLOD::getTextureData()
{
    return mTerrainTextureData;
}


void TerrainLOD::draw(mat4 projectionMatrix, mat4 viewMatrix){

    glDisable(GL_CULL_FACE);
	mTerrainLODScene->draw(projectionMatrix, viewMatrix);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}

void TerrainLOD::generateLODBuffers(ModelObject * modelObj, GLuint shaderId){

    GLuint DIM_X = 64;
    GLuint DIM_Z = 64;

    GLfloat RES_X = 1.0f/(DIM_X-1);
    GLfloat RES_Z = 1.0f/(DIM_Z-1);

    GLfloat sizeZ  = 100;
    GLfloat sizeX = 100;

    GLfloat STEP_X =  sizeX*RES_X;
    GLfloat STEP_Z =  sizeX*RES_Z;

    GLuint x,z;
    GLfloat xPos,yPos,zPos;


    GLuint VERTICES_PER_QUAD = 6;
    GLuint FLOATS_PER_POSITION = 3; // X,Y,Z
    GLuint NUM_VERTICES = VERTICES_PER_QUAD*(DIM_X-1)*(DIM_Z-1);
    GLfloat * vertexBuffer = (GLfloat *) malloc(sizeof(GLfloat)*FLOATS_PER_POSITION*NUM_VERTICES);

    GLuint FLOATS_PER_TEXEL = 2;
    GLfloat * textureCoordBuffer = (GLfloat *) malloc(sizeof(GLfloat)*FLOATS_PER_TEXEL*NUM_VERTICES);
    GLuint index = 0;
    GLuint textureIndex = 0;
    for(z = 0; z < DIM_Z-1; z++)
    for(x = 0; x < DIM_X-1; x++){

        xPos = (x*RES_X - 0.5)*sizeX;
        yPos = 0.0;
        zPos = (z*RES_Z - 0.5)*sizeZ;

        /**  0  **/
        vertexBuffer[index++] = xPos;
        vertexBuffer[index++] = yPos;
        vertexBuffer[index++] = zPos;

        textureCoordBuffer[textureIndex++] = x*RES_X;
        textureCoordBuffer[textureIndex++] = z*RES_Z;

        /**  1 **/
        vertexBuffer[index++] = xPos;
        vertexBuffer[index++] = yPos;
        vertexBuffer[index++] = zPos+STEP_Z;

        textureCoordBuffer[textureIndex++] = x*RES_X;
        textureCoordBuffer[textureIndex++] = (z+1)*RES_Z;

        /**  2 **/
        vertexBuffer[index++] = xPos+STEP_X;
        vertexBuffer[index++] = yPos;
        vertexBuffer[index++] = zPos+STEP_Z;


        textureCoordBuffer[textureIndex++] = (x+1)*RES_X;
        textureCoordBuffer[textureIndex++] = (z+1)*RES_Z;

        /**  3 **/
        vertexBuffer[index++] = xPos+STEP_X;
        vertexBuffer[index++] = yPos;
        vertexBuffer[index++] = zPos+STEP_Z;

        textureCoordBuffer[textureIndex++] = (x+1)*RES_X;
        textureCoordBuffer[textureIndex++] = (z+1)*RES_Z;


        /**  4 **/
        vertexBuffer[index++] = xPos;
        vertexBuffer[index++] = yPos;
        vertexBuffer[index++] = zPos;

        textureCoordBuffer[textureIndex++] = x*RES_X;
        textureCoordBuffer[textureIndex++] = z*RES_Z;


        /**  5 **/
        vertexBuffer[index++] = xPos + STEP_X;
        vertexBuffer[index++] = yPos;
        vertexBuffer[index++] = zPos;

        textureCoordBuffer[textureIndex++] = (x+1)*RES_X;
        textureCoordBuffer[textureIndex++] = z*RES_Z;


    }

    GLuint squareIndices[] = {0, 2, 1, 0};
    size_t numBytes = sizeof(GLuint)* 4;
    GLuint * squareIndicesData = (GLuint * )malloc(numBytes);
    memcpy(squareIndicesData,squareIndices,numBytes);

    modelObj->LoadDataToModel(
        vertexBuffer,
        NULL,
        textureCoordBuffer,
        NULL,
        squareIndicesData,
        NUM_VERTICES,
        2*3*2,
        shaderId);

}


void TerrainLOD::uploadSquareModelData(ModelObject * modelObj,GLuint shaderId){
    GLfloat square [] = {
                    -1,-1,0,
                    -1,1, 0,
                    1,1, 0,
                    1,1, 0,
                    -1,-1, 0,
                    1,-1, 0};

    size_t numBytes = sizeof(GLfloat)* 18;
    GLfloat * squareData = (GLfloat * ) malloc(numBytes);
    memcpy(squareData,square,numBytes);

    GLfloat squareTexCoord[] = {
                     0, 0,
                     0, 1,
                     1, 1,
                     1, 0,
                     0, 0,
                     0, 0
    };

    numBytes = sizeof(GLfloat)* 12;
    GLfloat * squareTexCoordData = (GLfloat * ) malloc(numBytes);
    memcpy(squareTexCoordData,squareTexCoord,numBytes);

    GLuint squareIndices[] = {0, 2, 1, 0, 3, 2,4, 6, 5, 4, 7, 6};
    numBytes = sizeof(GLuint)* 12;
    GLuint * squareIndicesData = (GLuint * )malloc(numBytes);
    memcpy(squareIndicesData,squareIndices,numBytes);

    modelObj->LoadDataToModel(
        squareData,
        NULL,
        squareTexCoordData,
        NULL,
        squareIndicesData,
        18,
        2*3*2,
        shaderId);

    free(squareData);
    free(squareTexCoordData);
    free(squareIndicesData);

    printError("Cloth Gen Position Buffer Coordinates ");
}
/*
void TerrainLOD::setCameraInfo(vec3 * cameraEye, vec3* cameraCenter){
    mCameraCenter  = cameraCenter;
    mCameraEye     = cameraEye;
}
*/
