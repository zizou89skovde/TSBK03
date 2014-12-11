#include "Terrain.h"

Terrain::Terrain(GLuint * w, GLuint * h)
{
    mScreenWitdh = w;
    mScreenHeight = h;
    mTerrainModel = new ModelObject();
    mTerrainDepthModel = new ModelObject();

    initializeSkyBox();
    initializeTerrain();

}

TerrainMetaData * Terrain::getTerrainMetaData(){
    return mTerrainMetaData;
}

TextureData * Terrain::getTextureData()
{
    return mTerrainTextureData;
}

FBOstruct * Terrain::getTerrainReflectedFBO(){
    return mTerrainReflectionFBO;
}

FBOstruct * Terrain::getTerrainFBO(){
    return mTerrainFBO;
}

void Terrain::initializeSkyBox(){
    /** Setting Sky dome shader **/
    GLuint skyDomeShader = loadShaders("shaders/skydome.vert","shaders/skydome.frag");
    mTerrainModel->setShader(skyDomeShader,SKYBOX_SHADER,VP,NO_DEPTH_TEST);

    /** Set sky dome texture **/
    GLuint skyDomeTexture;
    LoadTGATextureSimple((char *)"textures/skydome.tga",&skyDomeTexture);
    mTerrainModel->setTexture(skyDomeTexture,SKYBOX_SHADER,"u_Texture");

    /** Upload sky dome model **/
    Model* modelSkyDome = LoadModelPlus((char *)"models/skydome.obj");
	free(modelSkyDome->normalArray);
	modelSkyDome->normalArray = NULL;
    mTerrainModel->setModel(modelSkyDome,SKYBOX_SHADER);
    mTerrainModel->freeModelData(modelSkyDome);
    /** Set flip **/
    mTerrainModel->setUniformFloat(1.0f,SKYBOX_SHADER,"u_Flip");


}

void Terrain::initializeTerrain(){


    /** Load shader, one Phong shader, and shader without any shading ( for depth rendering ) */
    GLuint shader = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
	mTerrainModel->setShader(shader,TERRAIN_SHADER);

    GLuint shaderSimple = loadShaders("shaders/simple.vert", "shaders/simple.frag");
	mTerrainDepthModel->setShader(shaderSimple,TERRAIN_SIMPLE_SHADER);

    /** Load Texture **/
    mTerrainTextureData = new TextureData;
    LoadTGATextureData((char*)"textures/fft-terrain2.tga", mTerrainTextureData);

    /** Generate terrain from height map **/
    GenerateTerrain(mTerrainTextureData);

    /** Set transform **/
    mat4 IdMatrix = IdentityMatrix();
    mTerrainDepthModel->setTransform(IdMatrix,TERRAIN_SIMPLE_SHADER);

    /** Upload u_Clip  parameter **/
    mTerrainModel->setUniformFloat(0.0,TERRAIN_SHADER,"u_Clip");

    /** Set transform **/
    mat4 transformMatrix = IdentityMatrix();
    mTerrainModel->setTransform(transformMatrix,TERRAIN_SHADER);

    /** Initialize FBO's  used for rendering the water reflection **/
    mTerrainReflectionFBO = initFBO(FBOSize,FBOSize,0);
    mTerrainFBO = initFBO(FBOSize, FBOSize, 0);


}

Terrain::~Terrain()
{
    //dtor
}

void Terrain::setClip(bool enabled){
    GLfloat flip = -1.0;
    if(enabled){
        GLfloat clip = 1.0;
        mTerrainModel->replaceUniformFloat(&clip,TERRAIN_SHADER,"u_Clip");
        mTerrainModel->replaceUniformFloat(&flip,SKYBOX_SHADER,"u_Flip");
    }else{
        flip = 1.0;
        mTerrainModel->replaceUniformFloat(&flip,SKYBOX_SHADER,"u_Flip");
    }
}
void Terrain::drawSimple(mat4 proj, mat4 view){
    mTerrainDepthModel->draw(TERRAIN_SIMPLE_SHADER,proj,view);
}

void Terrain::drawDepth(mat4 proj, mat4 view){
    mTerrainDepthModel->draw(TERRAIN_SIMPLE_SHADER,proj,view);
}

void Terrain::setExternalModels(ModelObject * modelObj){
    mExternalModel.push_back(modelObj);
}

void Terrain::renderFlippedExternalModel(mat4 projectionMatrix,mat4 viewMatrix){
    ModelObject * modelObject;
    for(std::vector<ModelObject*>::iterator it = mExternalModel.begin(); it != mExternalModel.end(); ++it) {
        modelObject = *it;
        /** Flip model in regard to the Y-axis **/
        modelObject->flipModels();

        modelObject->draw(projectionMatrix,viewMatrix);
        /** Reset model to its original direction **/
        modelObject->flipModels();
    }
}

void Terrain::draw(mat4 proj, mat4 view){

    /** Draw reflected ***/
    useFBO(mTerrainReflectionFBO,NULL,NULL);
    glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4* transformTerrain = mTerrainModel->getTransform(TERRAIN_SHADER);


    /** Draw scene upside down **/
    *transformTerrain = S(1,-1,1);
    glEnable(GL_CLIP_DISTANCE0);
    setClip(true);
    glDisable(GL_CULL_FACE);
    mTerrainModel->draw(SKYBOX_SHADER,proj,view);
    renderFlippedExternalModel(proj,view);
    mTerrainModel->draw(TERRAIN_SHADER,proj,view);
    glDisable(GL_CLIP_DISTANCE0);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
    /** Draw to Offscreen fbo */
    setClip(false);
    useFBO(mTerrainFBO,NULL,NULL);
    glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat sizeTerrain = 1.00;
    *transformTerrain = S(sizeTerrain,sizeTerrain,sizeTerrain);
    mTerrainModel->draw(TERRAIN_SHADER,proj,view);

    /** Draw to screen **/
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, *mScreenWitdh, *mScreenHeight);
    mTerrainModel->draw(SKYBOX_SHADER,proj,view);
    mTerrainModel->draw(TERRAIN_SHADER,proj,view);




}

void Terrain::GenerateTerrain(TextureData *tex){
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	GLuint x, z;
    GLfloat *vertexArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLuint *indexArray = (GLuint*)malloc(sizeof(GLuint) * triangleCount*3);

    float heightScale = TerrainHeightScale;
    float heightConversion = heightScale/255.0;
    float heightOffset = heightScale/2.0 + TerrainHeightOffset;

    float gridSize    = TerrainPlaneScale;
    float gridRes     = gridSize/(tex->width-1);
    float gridOffset  = gridSize/2.0;


    mTerrainMetaData = new TerrainMetaData;
    mTerrainMetaData->HeightScale       = heightScale;
    mTerrainMetaData->TerrainDimension  = tex->width-1;
    mTerrainMetaData->TerrainSize       = gridSize;
    mTerrainMetaData->TerrainResolution = gridRes;
    mTerrainMetaData->TerrainHeightOffset = TerrainHeightOffset;

	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{


// Vertex array. You need to scale this properly
            float xVal = x * gridRes - gridOffset;
            float yVal = tex->imageData[(x + z * tex->width) * (tex->bpp/8)]*heightConversion - heightOffset;
            float zVal = z * gridRes - gridOffset;

			vertexArray[(x + z * tex->width)*3 + 0] = xVal;
			vertexArray[(x + z * tex->width)*3 + 1] = yVal;
			vertexArray[(x + z * tex->width)*3 + 2] = zVal;
// Normal vectors. You need to calculate these.
            vec3 normal = calcNormal(x,z,gridRes,heightConversion,tex);
			normalArray[(x + z * tex->width)*3 + 0] = normal.x;
			normalArray[(x + z * tex->width)*3 + 1] = normal.y;
			normalArray[(x + z * tex->width)*3 + 2] = normal.z;
// Texture coordinates. You may want to scale them.
		/*	texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;*/
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}

	// End of terrain generation
	// Create Model and upload to GPU:
	mTerrainModel->LoadDataToModel(
			vertexArray,
			normalArray,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3,
			TERRAIN_SHADER
			);

    mTerrainDepthModel->LoadDataToModel(
			vertexArray,
			NULL,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3,
			TERRAIN_SIMPLE_SHADER
			);


    /** Clean up **/
    free(vertexArray);
    free(normalArray);
    free(indexArray);

    GLuint type = GL_RGBA;
    // Build A Texture From The Data
	glGenTextures(1, &mTerrainTextureData->texID);			// Generate OpenGL texture IDs
	glBindTexture(GL_TEXTURE_2D, mTerrainTextureData->texID);		// Bind Our Texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
	if (mTerrainTextureData->bpp == 8)						// Was The TGA 8 Bits? Should be grayscale then.
	{
		type=GL_RED;			// If So Set The 'type' To GL_RED
	}
	if (mTerrainTextureData->bpp == 24)						// Was The TGA 24 Bits?
	{
		type=GL_RGB;			// If So Set The 'type' To GL_RGB
	}
	glTexImage2D(GL_TEXTURE_2D, 0, type, mTerrainTextureData->w, mTerrainTextureData->h, 0, type, GL_UNSIGNED_BYTE, mTerrainTextureData[0].imageData);
	/********* MIPMAP ******************/
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtered
    /***************************/
    free(mTerrainTextureData->imageData);
    mTerrainTextureData->imageData = NULL;

  printError("Terrain Buffer");

}

vec3 Terrain::calcNormal(GLuint x,GLuint z,GLfloat planeRes,GLfloat heightRes, TextureData * tex) {
    GLfloat steps[] =
    {
        1,0,
        1,1,
        0,1,
        -1,0,
        -1,-1,
        0,-1
    };

    GLfloat y = tex->imageData[(x + z * tex->width) * (tex->bpp/8)]*heightRes;
    vec3 center = vec3(x*planeRes,y*heightRes,z*planeRes);


    vec3 * triVectors = (vec3 * )malloc(sizeof(GLfloat)*3*6);

    for(GLuint i=0; i < 6; i ++){
        GLuint xIndex = x+steps[2*i];
        GLuint zIndex = z+steps[2*i+1];

        GLfloat yVal = y;
        if(xIndex >= 0 && xIndex < tex->width - 1  && zIndex >= 0 && zIndex < tex->height - 1){
            yVal = tex->imageData[(xIndex + zIndex * tex->width) * (tex->bpp/8)]*heightRes;
        }

        vec3 endPoint = vec3(xIndex*planeRes,yVal,zIndex*planeRes);
        triVectors[i] = endPoint - center;

    }
    vec3 normal = vec3(0,0,0);
    for(GLuint i=0; i < 6; i ++){
        GLuint i1 = i;
        GLuint i2 = (i+1) % 6;
        normal += Normalize(CrossProduct(triVectors[i1],triVectors[i2]));

    }
    free(triVectors);
    normal /= 6.0;

	return normal;
}




