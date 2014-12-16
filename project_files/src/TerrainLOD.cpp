#include "TerrainLOD.h"

void TerrainLOD::initialize(){

	mTerrainLODScene = new ModelObject();

	/** Load shader **/

    /** LINE_STRIP **/
	//GLuint shaderTerrainLOD = loadShadersGT("shaders/terrainLOD2.vert", "shaders/terrainLOD.frag", "shaders/terrainLOD.gs", "shaders/terrainLOD.tcs", "shaders/terrainLOD.tes");

    /** TRIANGLAR **/
	GLuint shaderTerrainLOD = loadShadersGT("shaders/terrainLOD2.vert", "shaders/terrainLOD_diffuse.frag", NULL , "shaders/terrainLOD.tcs", "shaders/terrainLOD.tes");

	mTerrainLODScene->setShader(shaderTerrainLOD,SHADER_TERRAIN_LOD);

    /** Upload model data **/
    generateTerrain();

	/** Load texture **/
    EnvironmentMetaData environmentMetaData = mEnvironment->getMetaData();
	mTerrainLODScene->setTexture(environmentMetaData.sHeightMapHandleHighRes,SHADER_TERRAIN_LOD,"u_HeightMap");
	mTerrainLODScene->setTexture(environmentMetaData.sNormalMapHandleHighRes,SHADER_TERRAIN_LOD,"u_NormalMap");


	/** Select drawInstanced-method and set number of instances **/
	mTerrainLODScene->setDrawMethod(PATCHES,SHADER_TERRAIN_LOD);


	/** Set model to world transform **/
	/** Scale and transform terrain to be centered around zero. And sceled to fit the size of the environment **/
    vec3 translation =vec3(-environmentMetaData.sSize[0],-environmentMetaData.sSize[1],-environmentMetaData.sSize[2])*0.5;
    vec3 scale = vec3(environmentMetaData.sSize[0],environmentMetaData.sSize[1],environmentMetaData.sSize[2]);
    mat4 transf = T(translation.x,translation.y,translation.z)*S(scale.x,scale.y,scale.z);
	mTerrainLODScene->setTransform(transf,SHADER_TERRAIN_LOD);


	mEnvironment->setReflectedModels(mTerrainLODScene,SHADER_TERRAIN_LOD);
	mEnvironment->setRefractedModels(mTerrainLODScene,SHADER_TERRAIN_LOD);

}

void TerrainLOD::draw(mat4 projectionMatrix, mat4 viewMatrix){

    glDisable(GL_CULL_FACE);
	mTerrainLODScene->draw(projectionMatrix, viewMatrix);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}

void TerrainLOD::generateTerrain(){

    GLuint  dimension  = TerrainBaseDimension;
    GLfloat resolution = 1.0f/((GLfloat)TerrainBaseDimension-1.0f);

    GLuint vertexCount = dimension*dimension;
    GLuint triangleCount = (dimension-1) * (dimension-1)* 2;
    GLuint indexCount    = triangleCount*VERTICES_PER_TRIANGLE;

    GLfloat vertexArray[FLOATS_PER_POSITION * vertexCount];

    for(GLuint z = 0; z < dimension; ++z)
        for(GLuint x = 0; x < dimension; ++x){

            GLfloat xPos = (GLfloat)x*resolution;
            GLfloat zPos = (GLfloat)z*resolution;
            vertexArray[(x + z * dimension)*3 + 0] = xPos;
            vertexArray[(x + z * dimension)*3 + 1] = 0;
            vertexArray[(x + z * dimension)*3 + 2] = zPos;

    }

    GLuint indexArray[indexCount];
    for (GLuint y = 0; y < dimension-1; y++)
        for (GLuint x = 0; x < dimension-1; x++)
        {
        // Triangle 1
            indexArray[(x + y * (dimension-1))*INDICES_PER_QUAD + 0] = x + y * dimension;
            indexArray[(x + y * (dimension-1))*INDICES_PER_QUAD + 1] = x + (y+1) * dimension;
            indexArray[(x + y * (dimension-1))*INDICES_PER_QUAD + 2] = x+1 + y * dimension;
        // Triangle 2
            indexArray[(x + y *  (dimension-1))*INDICES_PER_QUAD + 3] = x+1 + y * dimension;
            indexArray[(x + y *  (dimension-1))*INDICES_PER_QUAD + 4] = x + (y+1) * dimension;
            indexArray[(x + y *  (dimension-1))*INDICES_PER_QUAD + 5] = x+1 + (y+1) * dimension;
        }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint positions;
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);

    Model * model = new Model();
    model->numIndices = indexCount;
    model->ib = indices;
    model->vao = vao;
    model->vb = positions;
    mTerrainLODScene->setModel(model,SHADER_TERRAIN_LOD);
    printError("Terrain Buffers");

}


