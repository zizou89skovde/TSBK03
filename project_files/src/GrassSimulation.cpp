#include "GrassSimulation.h"

GrassSimulation::GrassSimulation()
{
}

void GrassSimulation::initialize(){

    /** Assign default values **/
    previousTime = -1;

	/** Initalize grass model object **/
    mGrassScene = new ModelObject();

	/** Load shader **/
	//GLuint grassShader = loadShadersG("shaders/grass.vert", "shaders/grass.frag", "shaders/grass.gs");
	GLuint grassShader = loadShadersGT("shaders/grass.vert", "shaders/grass.frag", "shaders/grass.gs", "shaders/grass.tcs", "shaders/grass.tes");
	//GLuint grassShader = loadShaders("shaders/grass.vert", "shaders/grass.frag");
	mGrassScene->setShader(grassShader, GRASS_SHADER_ID, MVP);

	/** Select draw method **/
	mGrassScene->setDrawMethod(PATCHES, GRASS_SHADER_ID);

	/** Set grass to be reflected in the water surface **/
	mEnvironment->setReflectedModels(mGrassScene,GRASS_SHADER_ID);

    /**  Upload buffer coordinates **/
    generateGrassMesh();

    /** Wind input **/
    mGrassScene->setUniformFloat(1.0f, GRASS_SHADER_ID, "u_Wind");
	angle = 0.0f;


	/** Load texture **/
    EnvironmentMetaData environmentMetaData = mEnvironment->getMetaData();
	mGrassScene->setTexture(environmentMetaData.sHeightMapHandleHighRes,GRASS_SHADER_ID,"u_HeightMap");
	mGrassScene->setTexture(environmentMetaData.sNormalMapHandleHighRes,GRASS_SHADER_ID,"u_NormalMap");

    /** Noise **/
    GLuint grassNoiseTexture;
    LoadTGATextureSimple((char*)"textures/noise.tga", &grassNoiseTexture);
    mGrassScene->setTexture(grassNoiseTexture,GRASS_SHADER_ID,"u_GrassNoise");

    /** Grass mask **/
    GLuint grassMaskTexture;
    LoadTGATextureSimple((char*)"textures/test3.tga", &grassMaskTexture);
    mGrassScene->setTexture(grassMaskTexture,GRASS_SHADER_ID,"u_GrassMask");

    /** Set model to world transform **/
	/** Scale and transform terrain to be centered around zero. And sceled to fit the size of the environment **/
    vec3 translation =vec3(-environmentMetaData.sSize[0],-environmentMetaData.sSize[1],-environmentMetaData.sSize[2])*0.5;
    vec3 scale = vec3(environmentMetaData.sSize[0],environmentMetaData.sSize[1],environmentMetaData.sSize[2]);
    mat4 transf = T(translation.x,translation.y,translation.z)*S(scale.x,scale.y,scale.z);
	mGrassScene->setTransform(transf,GRASS_SHADER_ID);
}

void GrassSimulation::generateGrassMesh() {

    GLuint  dimension  = GrassMeshBaseDimension;
    GLfloat resolution = 1.0f/((GLfloat)GrassMeshBaseDimension-1.0f);
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
    mGrassScene->setModel(model,GRASS_SHADER_ID);
    printError("generateGrassMesh");
}

void GrassSimulation::draw(mat4 projectionMatrix,mat4 viewMatrix) {

	glDisable(GL_CULL_FACE);
	mGrassScene->draw(GRASS_SHADER_ID,projectionMatrix,viewMatrix);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void GrassSimulation::update() {
	GLfloat dt = 0.0005f;
	GLfloat pi = 3.141592653589793;
	angle += dt;

	if (angle >= 2*pi) {
		angle = 0;
	}

	mGrassScene->replaceUniformFloat(&angle,GRASS_SHADER_ID,"u_Wind");
}

GrassSimulation::~GrassSimulation()
{
    //dtor
}
