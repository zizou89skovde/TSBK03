#include "ModelObject.h"

ModelObject::ModelObject()
{


}

ModelObject::~ModelObject()
{
    /** Clean up models **/
    Shader_Type* shader;
    for(ShaderIterator it = mShaderMap.begin(); it != mShaderMap.end(); ++it) {
        shader = it->second;

        /***** Delete Models ********/
        Model_Type * m = shader->sModelData;

        if(m->sModel->vertexArray!= NULL){
            free(m->sModel->vertexArray);
            glDeleteBuffers(1, &m->sModel->vb);
        }
        if(m->sModel->indexArray != NULL){
            free(m->sModel->indexArray);
            glDeleteBuffers(1,&m->sModel->ib);
        }
        if(m->sModel->texCoordArray != NULL){
            free(m->sModel->texCoordArray);
            glDeleteBuffers(1,&m->sModel->tb);
        }
        if(m->sModel->normalArray != NULL){
            free(m->sModel->normalArray);
            glDeleteBuffers(1,&m->sModel->nb);
        }
        /** Delete VAO **/
        glDeleteVertexArrays(1,&m->sModel->vao);

        /** Delete Model containers**/
        free(m->sModel);
        delete m;

        /***** Delete Textures ********/

        Texture_Type * texture;
        for(TextureIterator itTexture = shader->sTextureMap.begin(); itTexture != shader->sTextureMap.end(); ++itTexture) {
            texture = itTexture->second;
            glDeleteTextures(1,&texture->sTextureId);
            delete texture->sUniformName;
            delete texture;
        }
        shader->sTextureMap.clear();


        /** Delete Uniform**/
        UniformFloat_Type * uniformFloat;
        for(UniformFloatIterator itUniform = shader->sUniformFloatMap.begin(); itUniform != shader->sUniformFloatMap.end(); ++itUniform) {
            uniformFloat = itUniform->second;
            delete uniformFloat;

        }
        shader->sUniformFloatMap.clear();


        /** Delete Matrix Uniform**/
        UniformMatrix_Type * matrixUniform;
        for(UniformMatrixIterator itUniform = shader->sUniformMatrixMap.begin(); itUniform != shader->sUniformMatrixMap.end(); ++itUniform) {
            matrixUniform = itUniform->second;
            delete matrixUniform;
        }
        shader->sUniformFloatMap.clear();

    }

    #ifdef MODEL_OBJECT_VERBOSE
    printError("Model Object Clean up");
    #endif
}
void ModelObject::draw(mat4 projectionMatrix, mat4 viewMatrix){

    Shader_Type * shader;
    for(ShaderIterator it = mShaderMap.begin(); it != mShaderMap.end(); ++it) {
		shader = it->second;
   		selectDrawMethod(shader,projectionMatrix,viewMatrix);
    }
}

void ModelObject::draw(GLuint shaderId,mat4 projectionMatrix, mat4 viewMatrix){
	selectDrawMethod(mShaderMap[shaderId],projectionMatrix,viewMatrix);
    
    // Draw & update speedlines if point vector not empty.
    // Draw & update models/transforms if transform vector not empty.
    if (mShaderMap[shaderId]->sSpeedlines.sSpeedlineSwitch != 0) {
        updateSpeedlines(mShaderMap[shaderId]);

        if (mShaderMap[shaderId]->sSpeedlines.sSpeedlineSwitch == 2)
            updateSpeedModels(mShaderMap[shaderId]);

        if (mShaderMap[shaderId]->sSpeedlines.sSpeedlineSwitch == 1)
            drawSpeedlines(mShaderMap[shaderId], projectionMatrix, viewMatrix);

        if (mShaderMap[shaderId]->sSpeedlines.sSpeedlineSwitch == 2)
            drawSpeedModels(shaderId, projectionMatrix, viewMatrix);
    }
    else {
        mShaderMap[shaderId]->sSpeedlines.sMovementPoints.clear();
        mShaderMap[shaderId]->sSpeedlines.sMovementTransforms.clear();
        //mShaderMap[shaderId]->sSpeedlines.sTimestamps.clear();
    }
}

void ModelObject::selectDrawMethod(Shader_Type * shader, mat4 projectionMatrix, mat4 viewMatrix){

	switch(shader->sDrawMethod){

		case POINTSTEST:
			drawPoints(shader,projectionMatrix,viewMatrix);
			break;
		case ARRAYS:
			drawArrays(shader,projectionMatrix,viewMatrix);
			break;
		case PATCHES:
			drawPatches(shader,projectionMatrix,viewMatrix);
			break;
		case INSTANCED:
			drawInstanced(shader,projectionMatrix,viewMatrix);
			break;
		default:
			printf("ERROR invalid draw Method\n");
		break;
	}
}



void ModelObject::flipModels(GLuint shaderId){
    Shader_Type * shader = mShaderMap[shaderId];
    shader->sTransform = S(1,-1,1)*shader->sTransform;

    if(shader->sUniformFloatMap.find("u_Clip") != shader->sUniformFloatMap.end()){
        shader->sUniformFloatMap["u_Clip"]->sData[0] *= -1;
        uploadUniformFloat(shader->sUniformFloatMap["u_Clip"]);
    }

}


void ModelObject::uploadTransform(Shader_Type * shader,mat4 projectionMatrix,mat4 viewMatrix){
    mat4 mvMatrix;
    mat4 mvpMatrix;
    mat4 vpMatrix;
    mat3 normalMatrix;
    GLuint program = shader->sProgramHandle;
    switch(shader->sComposition){
    case MVP:
        mvMatrix = Mult(viewMatrix,shader->sTransform);
        mvpMatrix = Mult(projectionMatrix,mvMatrix);
        glUniformMatrix4fv(glGetUniformLocation(program, "MV_Matrix"), 1, GL_TRUE, mvMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(program, "MVP_Matrix"), 1, GL_TRUE, mvpMatrix.m);
        normalMatrix = InverseTranspose(mvMatrix);
        glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"), 1, GL_TRUE, normalMatrix.m);
        break;
    case VP:
        vpMatrix = Mult(projectionMatrix,viewMatrix);
        glUniformMatrix4fv(glGetUniformLocation(program, "V_Matrix"), 1, GL_TRUE, viewMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(program, "VP_Matrix"), 1, GL_TRUE, vpMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(program, "P_Matrix"), 1, GL_TRUE, projectionMatrix.m);
        normalMatrix = InverseTranspose(viewMatrix);
        glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"), 1, GL_TRUE, normalMatrix.m);
        break;
    case P:
        glUniformMatrix4fv(glGetUniformLocation(program, "P_Matrix"), 1, GL_TRUE, projectionMatrix.m);
        break;
    case NONE:
        break;
    default:
        mvMatrix = Mult(viewMatrix,shader->sTransform);
        mvpMatrix = Mult(projectionMatrix,mvMatrix);

        glUniformMatrix4fv(glGetUniformLocation(program, "MV_Matrix"), 1, GL_TRUE, mvMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(program, "MVP_Matrix"), 1, GL_TRUE, mvpMatrix.m);
        break;
    }
    #ifdef MODEL_OBJECT_VERBOSE
    printError("Shader Transform Matrices");
    #endif

}

void ModelObject::updateSpeedlines(Shader_Type* shader)
{
    // Get object current position and add to vector
    vec3 pos = shader->sTransform * vec3(0, 0, 0);
    std::vector<vec3>* points = &(shader->sSpeedlines.sMovementPoints);
    points->push_back(pos);
    //registerTimeSpeedlines(shader->sShaderId);

    if (points->size() > NUMBER_OF_SPEEDPOINTS) {
        points->erase(points->begin());
        //shader->sSpeedlines.sTimestamps.erase(shader->sSpeedlines.sTimestamps.begin());
    }

    // Go through all points, erase if point has lived too long
    // NOT USED ATM
    /*Timestamp_Type* time;
    int numberOfPoints = shader->sSpeedlines.sMovementPoints.size();
    for (int i = 0; i < numberOfPoints; ++i) {
        time = &(shader->sSpeedlines.sTimestamps)[i];
        
    }*/
}

void ModelObject::updateSpeedModels(Shader_Type * shader)
{
    // Get object current position and add to vector
    mat4 trans = shader->sTransform;
    std::vector<mat4>* transforms = &(shader->sSpeedlines.sMovementTransforms);
    transforms->push_back(trans);

    if (transforms->size() > NUMBER_OF_SPEEDPOINTS) {
        transforms->erase(transforms->begin());
    }
}

void ModelObject::selectTexture(Shader_Type * shader){
    Texture_Type * texture;
    GLuint numActiveTextures = 0;
    GLuint program = shader->sProgramHandle;
    for(TextureIterator it = shader->sTextureMap.begin(); it != shader->sTextureMap.end(); ++it) {
        texture = it->second;
        glActiveTexture(GL_TEXTURE0+numActiveTextures);
        glBindTexture(GL_TEXTURE_2D, texture->sTextureId);
        glUniform1i(glGetUniformLocation(program, texture->sUniformName), numActiveTextures);
        numActiveTextures++;
        #ifdef MODEL_OBJECT_VERBOSE
        printError(texture->sUniformName);
        #endif
    }
}
void ModelObject::uploadUniformFloat(UniformFloat_Type* uniform){
    Shader_Type * shader = mShaderMap[uniform->sShaderId];
    GLuint program = shader->sProgramHandle;
    glUseProgram(program);
    int loc = glGetUniformLocation(program, uniform->sUniformName);
    #ifdef MODEL_OBJECT_VERBOSE
    if(loc < 0 ){
        printf("Unused or invalid uniform: %s\n", uniform->sUniformName);
    }
    printError(uniform->sUniformName);
    #endif
    switch(uniform->sSize){
        case 1:
            glUniform1f(loc, uniform->sData[0]);
        break;
        case 2:
            glUniform2f(loc, uniform->sData[0],uniform->sData[1]);
        break;
        case 3:
            glUniform3f(loc, uniform->sData[0],uniform->sData[1],uniform->sData[2]);
        break;
        case 4:
            glUniform4f(loc, uniform->sData[0],uniform->sData[1],uniform->sData[2],uniform->sData[3]);
        break;
        default:
        break;
    }
    glUseProgram(0);
    #ifdef MODEL_OBJECT_VERBOSE
    printError("Upload Uniform");
    #endif
}

void ModelObject::uploadUniformMatrix(UniformMatrix_Type* uniformMatrix){
    Shader_Type * shader = mShaderMap[uniformMatrix->sShaderId];
    GLuint program = shader->sProgramHandle;
    glUseProgram(program);
    int loc = glGetUniformLocation(program, uniformMatrix->sUniformName);
    #ifdef MODEL_OBJECT_VERBOSE
    if(loc < 0 ){
        printf("Unused or invalid uniform: %s\n", uniformMatrix->sUniformName);
    }
    printError(uniformMatrix->sUniformName);
    #endif
    glUniformMatrix4fv(glGetUniformLocation(program, uniformMatrix->sUniformName), 1, GL_TRUE,  uniformMatrix->sMatrix.m);
    //glUniformMatrix4fv(loc, 1, GL_TRUE, uniformMatrix->sMatrix.m);
    glUseProgram(0);
    #ifdef MODEL_OBJECT_VERBOSE
    printError("Upload Uniform");
    #endif
}

void ModelObject::setUniformFloat(GLfloat * data, GLuint sizeData, GLuint shaderId, const char * uniformName){
    UniformFloat_Type * uniform = new UniformFloat_Type();
    for(GLuint i=0;i < sizeData; i++){
        uniform->sData[i] = data[i];
    }
    uniform->sSize = sizeData;
    uniform->sShaderId = shaderId;
    memset(uniform->sUniformName,0,uniform->CHAR_LEN*sizeof(char));
    strcpy(uniform->sUniformName,uniformName);
    mShaderMap[shaderId]->sUniformFloatMap[uniformName] = uniform;
    uploadUniformFloat(uniform);

}

void ModelObject::setUniformMatrix(mat4 matrix, GLuint shaderId, const char * uniformName){
    UniformMatrix_Type * uniform = new UniformMatrix_Type();
    uniform->sMatrix = matrix;
    uniform->sShaderId = shaderId;
    memset(uniform->sUniformName,0,uniform->CHAR_LEN*sizeof(char));
    strcpy(uniform->sUniformName,uniformName);
    mShaderMap[shaderId]->sUniformMatrixMap[uniformName] = uniform;
    uploadUniformMatrix(uniform);

}

void ModelObject::setUniformFloat(const GLfloat data,GLuint shaderId, const char * uniformName){
    UniformFloat_Type * uniform = new UniformFloat_Type();
    uniform->sData[0] = data;
    uniform->sSize = 1;
    uniform->sShaderId = shaderId;
    memset(uniform->sUniformName,0,uniform->CHAR_LEN*sizeof(char));
    strcpy(uniform->sUniformName,uniformName);
    mShaderMap[shaderId]->sUniformFloatMap[uniformName] = uniform;
    uploadUniformFloat(uniform);
}

void ModelObject::setShader(GLuint handleGPU, GLuint shaderId,Tranform_Composition_Type  composition){
    Shader_Type * shader = new Shader_Type();
    shader->sProgramHandle = handleGPU;
    shader->sShaderId        = shaderId;
    shader->sComposition     = composition;
    shader->sDepthTest       = DEPTH_TEST;
	shader->sDrawMethod 	 = ARRAYS;
    mShaderMap[shaderId] 	 = shader;
}

void ModelObject::setShader(GLuint handleGPU, GLuint shaderId,Tranform_Composition_Type  composition,DepthTest_Type depthTest){
    Shader_Type * shader     = new Shader_Type();
    shader->sProgramHandle   = handleGPU;
    shader->sShaderId        = shaderId;
    shader->sComposition     = composition;
    shader->sDepthTest       = depthTest;
	shader->sDrawMethod 	 = ARRAYS;
    mShaderMap[shaderId] 	 = shader;
}

void ModelObject::setShader(GLuint handleGPU, GLuint shaderId){
    Shader_Type * shader 	 = new Shader_Type();
    shader->sProgramHandle   = handleGPU;
    shader->sShaderId        = shaderId;
    shader->sComposition     = MVP;
    shader->sDepthTest       = DEPTH_TEST;
	shader->sDrawMethod 	 = ARRAYS;
    mShaderMap[shaderId] 	 = shader;
}

void ModelObject::setNumInstances(GLuint numInstances, GLuint shaderId){
	mShaderMap[shaderId]->sNumInstances = numInstances;
}

void ModelObject::setTexture(GLuint handle,GLuint shaderId,const char* uniformName){

    Texture_Type * newTexture = new Texture_Type();

    newTexture->sShaderId     = shaderId;
    newTexture->sTextureId    = handle;

    memset(newTexture->sUniformName,0,newTexture->CHAR_LEN*sizeof(char));
    strcpy(newTexture->sUniformName,uniformName);

    mShaderMap[shaderId]->sTextureMap[uniformName] = newTexture;


}
void ModelObject::setModel(Model * m, GLuint shaderId){
    Model_Type * model = new Model_Type();
    model->sModel = m;
    model->sShaderId = shaderId;
    mShaderMap[shaderId]->sModelData = model;

}


void ModelObject::setTransform(mat4 transf,GLuint shaderId){
    mShaderMap[shaderId]->sTransform = transf;

}

void ModelObject::setDrawMethod(DrawMethod_Type method,GLuint shaderId){
	mShaderMap[shaderId]->sDrawMethod = method;
}

// Initializes speed lines. Requires initial transform to be set.
void ModelObject::setSpeedlinesInit(GLuint shaderId)
{
    mShaderMap[shaderId]->sSpeedlines.sSpeedlineSwitch = 1;

    // Clear vectors to be sure it's empty
    mShaderMap[shaderId]->sSpeedlines.sMovementPoints.clear();
    //mShaderMap[shaderId]->sSpeedlines.sTimestamps.clear();

    // Get object start position as first point in vector
    vec3 startPos = mShaderMap[shaderId]->sTransform * vec3(0, 0, 0);
    mShaderMap[shaderId]->sSpeedlines.sMovementPoints.push_back(startPos);

    // Save timestamp
    //ResetMilli();
    //registerTimeSpeedlines(shaderId);

    // Compile and set the speed lines shader
   	GLuint speedlineShader = loadShaders("shaders/speedlineshader.vert", "shaders/speedlineshader.frag");
    mShaderMap[shaderId]->sSpeedlines.sSpeedlinesShader = speedlineShader;

    Model* m = new Model();

	glGenVertexArrays(1,&m->vao);
	glBindVertexArray(m->vao);

    glGenBuffers(1, &m->vb);
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);

    mShaderMap[shaderId]->sSpeedlines.m = m;
}

// Initializes speed models. Requires initial transform to be set and setSpeedlinesInit() to be run.
void ModelObject::setSpeedModelsInit(GLuint shaderId)
{
    // Clear vector to be sure it's empty
    mShaderMap[shaderId]->sSpeedlines.sMovementTransforms.clear();

    mShaderMap[shaderId]->sSpeedlines.sSpeedlineSwitch = 2;
    mat4 startTrans = mShaderMap[shaderId]->sTransform;
    mShaderMap[shaderId]->sSpeedlines.sMovementTransforms.push_back(startTrans);
}

void ModelObject::setNoSpeedlines(GLuint shaderId)
{
    mShaderMap[shaderId]->sSpeedlines.sSpeedlineSwitch = 0;
}

void ModelObject::registerTimeSpeedlines(GLuint shaderId)
{
    /*Timestamp_Type time;
    time.s = GetSeconds();
    time.ms = GetMilliseconds();
    mShaderMap[shaderId]->sSpeedlines.sTimestamps.push_back(time);*/
}

DrawMethod_Type ModelObject::getDrawMethod(GLuint shaderId){
	return mShaderMap[shaderId]->sDrawMethod;
}

int ModelObject::getSpeedlineMode(GLuint shaderId)
{
    return mShaderMap[shaderId]->sSpeedlines.sSpeedlineSwitch;
}

void ModelObject::replaceTexture(GLuint handle,GLuint shaderId,const char* uniformName){
    mShaderMap[shaderId]->sTextureMap[uniformName]->sTextureId = handle;
}

void ModelObject::replaceUniformFloat(GLfloat* data,GLuint shaderId,const char* uniformName){
    UniformFloat_Type * uniform = mShaderMap[shaderId]->sUniformFloatMap[uniformName];

    for(GLuint i = 0;i < uniform->sSize; i++)
        uniform->sData[i] = data[i];
    uploadUniformFloat(uniform);

    #ifdef MODEL_OBJECT_VERBOSE
    printError("Replace Uniform Float");
    #endif

}

void ModelObject::replaceUniformMatrix(mat4 matrix,GLuint shaderId,const char* uniformName){
    UniformMatrix_Type * uniform = mShaderMap[shaderId]->sUniformMatrixMap[uniformName];
    uniform->sMatrix = matrix;
    uploadUniformMatrix(uniform);
    #ifdef MODEL_OBJECT_VERBOSE
    printError("Replace Uniform Matrix");
    #endif


}
void ModelObject::clone(GLuint srcShaderId, GLuint dstShaderId, bool copyUniforms, bool copyTextures){

    Shader_Type* srcShader = mShaderMap[srcShaderId];

    /**Copy Uniforms **/
    if(copyUniforms){
        for(UniformFloatIterator it = srcShader->sUniformFloatMap.begin(); it != srcShader->sUniformFloatMap.end(); ++it) {
            mShaderMap[dstShaderId]->sUniformFloatMap[it->first] = it->second;
        }
        for(UniformMatrixIterator it = srcShader->sUniformMatrixMap.begin(); it != srcShader->sUniformMatrixMap.end(); ++it) {
            mShaderMap[dstShaderId]->sUniformMatrixMap[it->first] = it->second;
        }
    }
    /** Copy textures **/
    if(copyTextures){
        for(TextureIterator it = srcShader->sTextureMap.begin(); it != srcShader->sTextureMap.end(); ++it) {
                mShaderMap[dstShaderId]->sTextureMap[it->first] = it->second;
        }
    }

    /** Create a new model data object **/
    Model_Type* modelData = new Model_Type();
    modelData->sShaderId = dstShaderId;

    /** Copy handles to buffers **/
    modelData->sModel = srcShader->sModelData->sModel;
    mShaderMap[dstShaderId]->sModelData = modelData;

}


void ModelObject::drawPoints(Shader_Type * shader,mat4 projectionMatrix, mat4 viewMatrix){

	GLuint program  = shader->sProgramHandle;

    glUseProgram(program);

    uploadTransform(shader,projectionMatrix,viewMatrix);

	Model * m = shader->sModelData->sModel;

	glBindVertexArray(m->vao);	// Select VAO
    #ifdef MODEL_OBJECT_VERBOSE
    printError("VAO");
    #endif

	selectTexture(shader);

	glBindBuffer(GL_ARRAY_BUFFER, m->vb); // Select VBO

    #ifdef MODEL_OBJECT_VERBOSE
    printError("VBO");
    #endif

	GLuint loc = glGetAttribLocation(program, "in_Position");
	if (loc >= 0)
	{
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);
	}
	else
		fprintf(stderr, "DrawModel warning: '%s' not found in shader!\n", "in_Position");

    glDrawArrays(GL_POINTS, 0, m->numVertices);

    #ifdef MODEL_OBJECT_VERBOSE
    printError("Draw Points");
    #endif

}

void ModelObject::drawInstanced(Shader_Type * shader,mat4 projectionMatrix, mat4 viewMatrix){

	GLuint program  = shader->sProgramHandle;

    glUseProgram(program);

    uploadTransform(shader,projectionMatrix,viewMatrix);

	selectTexture(shader);

	//glPatchParameteri(GL_PATCH_VERTICES, 3);
   	glDrawArraysInstanced(GL_PATCHES, 0, 4, shader->sNumInstances);

    #ifdef MODEL_OBJECT_VERBOSE
    printError("Draw Instanced");
    #endif

}

void ModelObject::drawPatches(Shader_Type * shader,mat4 projectionMatrix, mat4 viewMatrix){

	GLuint program  = shader->sProgramHandle;

    glUseProgram(program);

    uploadTransform(shader,projectionMatrix,viewMatrix);

	Model * m = shader->sModelData->sModel;

	glBindVertexArray(m->vao);	// Select VAO
    #ifdef MODEL_OBJECT_VERBOSE
    printError("VAO");
    #endif

	selectTexture(shader);

	glBindBuffer(GL_ARRAY_BUFFER, m->vb);

    #ifdef MODEL_OBJECT_VERBOSE
    printError("Vertex buffer");
    #endif

	GLint loc = glGetAttribLocation(program, "in_Position");
	if (loc >= 0)
	{
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
		glEnableVertexAttribArray(loc);
	}
	else
		fprintf(stderr, "DrawModel warning: '%s' not found in shader!\n", "in_Position");
/*
	loc = glGetAttribLocation(program, "in_TextureCoord");
	if (loc >= 0)
	{
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);
	}
	else
		fprintf(stderr, "DrawModel warning: '%s' not found in shader!\n", "in_TextureCoord");
*/
	glPatchParameteri(GL_PATCH_VERTICES, 3);

    #ifdef MODEL_OBJECT_VERBOSE
    printError("Draw Patches 1");
    #endif
    glDrawElements(GL_PATCHES, m->numIndices, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_PATCHES, 0, m->numVertices);

    #ifdef MODEL_OBJECT_VERBOSE
    printError("Draw Patches 2");
    #endif

}

void ModelObject::drawArrays(Shader_Type * shader,mat4 projectionMatrix,mat4 viewMatrix){
        int program = shader->sProgramHandle;
        glUseProgram(program);

        uploadTransform(shader,projectionMatrix,viewMatrix);

        selectTexture(shader);

        SELECT_CONFIG
        drawModel(shader);
        DESELECT_CONFIG
}

void ModelObject::drawModel(Shader_Type* shader){

    Model_Type * m = shader->sModelData;
    GLuint program = shader->sProgramHandle;

    char* normalAttributeString= NULL;
    if(m->sModel->normalArray != NULL) normalAttributeString = (char *)"in_Normal";

    char* textureAttributeString = NULL;
    if(m->sModel->texCoordArray != NULL) textureAttributeString = (char *)"in_TextureCoord";

    DrawModel(m->sModel, program,(char *)"in_Position" ,normalAttributeString , textureAttributeString);


}

void ModelObject::drawSpeedlines(Shader_Type* shader, mat4 projectionMatrix, mat4 viewMatrix)
{
	GLuint program = shader->sSpeedlines.sSpeedlinesShader;
    glUseProgram(program);

    // Upload view and projection matrices
    mat4 vpMatrix;
    mat3 normalMatrix;
    vpMatrix = Mult(projectionMatrix,viewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "V_Matrix"), 1, GL_TRUE, viewMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "VP_Matrix"), 1, GL_TRUE, vpMatrix.m);
    normalMatrix = InverseTranspose(viewMatrix);
    glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"), 1, GL_TRUE, normalMatrix.m);

    Model* m = shader->sSpeedlines.m;

    // Convert from vector to array of coordinates
    vec3* point;
    int numberOfPoints = shader->sSpeedlines.sMovementPoints.size();
    GLfloat* pointArray = new GLfloat[numberOfPoints*3];
    for (int i = 0; i < numberOfPoints; ++i) {
        point = &(shader->sSpeedlines.sMovementPoints)[i];
        pointArray[i*3 + 0] = point->x;
        pointArray[i*3 + 1] = point->y;
        pointArray[i*3 + 2] = point->z;
        //printf("Lines Number of Points: %d, Point %d: x: %f y: %f z: %f\n", numberOfPoints, i, pointArray[i*3 + 0], pointArray[i*3 + 1], pointArray[i*3 + 2]);
    }

	glBindVertexArray(m->vao);	// Select VAO

    // Upload points data
	glBindBuffer(GL_ARRAY_BUFFER, m->vb); // Select VBO
    glBufferData(GL_ARRAY_BUFFER, numberOfPoints * 3 * sizeof(GLfloat), pointArray, GL_STATIC_DRAW);

    GLuint loc = glGetAttribLocation(program, "in_Position");
	if (loc >= 0)
	{
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);
	}
	else
		fprintf(stderr, "DrawModel warning: '%s' not found in shader!\n", "in_Position");

    glDrawArrays(GL_LINE_STRIP, 0, numberOfPoints);

    delete[] pointArray;
}

void ModelObject::drawSpeedModels(GLuint shaderId, mat4 projectionMatrix, mat4 viewMatrix)
{
    mat4* transformTempPtr = getTransform(shaderId);
    mat4 transformTemp = *transformTempPtr;

    // For all points, upload new transform and draw a model
    vec3* point;
    vec3 lastDrawPoint(100000, 100000, 100000);
    mat4* trans;
    int numberOfTransforms = mShaderMap[shaderId]->sSpeedlines.sMovementTransforms.size();
    GLfloat speedlinesAlpha[numberOfTransforms];
    GLfloat speedlinesAlphaCurrent = 1.0;

    for (int i = 0; i < numberOfTransforms; ++i) {

        point = &(mShaderMap[shaderId]->sSpeedlines.sMovementPoints)[i];
        float distanceToLastPoint = sqrt(pow(point->x - lastDrawPoint.x ,2) + pow(point->y - lastDrawPoint.y ,2) + pow(point->z - lastDrawPoint.z ,2));
        
        if (distanceToLastPoint > 0.1) {
            trans = &(mShaderMap[shaderId]->sSpeedlines.sMovementTransforms)[i];
            setTransform(*trans, shaderId);

            // Transparency stuff
            speedlinesAlphaCurrent -= 0.2;
            speedlinesAlpha[i] = speedlinesAlphaCurrent;
            setUniformFloat(&(speedlinesAlpha[i]), 1, shaderId, "u_SpeedlinesAlpha");
            glEnable(GL_BLEND); // Blending, for transparency
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            selectDrawMethod(mShaderMap[shaderId], projectionMatrix, viewMatrix);

            glDisable(GL_BLEND);
            lastDrawPoint = *point;
        }
    }

    // Reset transform for shader (doesn't seem to make a difference...?)
    setTransform(transformTemp, shaderId);
}


mat4 * ModelObject::getTransform(GLuint shaderId){
    return &(mShaderMap[shaderId]->sTransform);
}

void ModelObject::BuildModelVAO2(Model *m){

	glGenVertexArrays(1, &m->vao);
	glGenBuffers(1, &m->vb);
	glGenBuffers(1, &m->ib);
	if(m->normalArray != NULL)
        glGenBuffers(1, &m->nb);

	if (m->texCoordArray != NULL)
		glGenBuffers(1, &m->tb);

	glBindVertexArray(m->vao);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
	//glVertexAttribPointer(glGetAttribLocation(program, vertexVariableName), 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(glGetAttribLocation(program, vertexVariableName));
    if(m->normalArray != NULL ){
	// VBO for normal data
    glBindBuffer(GL_ARRAY_BUFFER, m->nb);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
        //glVertexAttribPointer(glGetAttribLocation(program, normalVariableName), 3, GL_FLOAT, GL_FALSE, 0, 0);
        //glEnableVertexAttribArray(glGetAttribLocation(program, normalVariableName));
    }
	// VBO for texture coordinate data NEW for 5b
	if (m->texCoordArray != NULL)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m->tb);
		glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
		//glVertexAttribPointer(glGetAttribLocation(program, texCoordVariableName), 2, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(glGetAttribLocation(program, texCoordVariableName));
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices*sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);
}
void ModelObject::LoadDataToModel(
			GLfloat *vertices,
			GLfloat *normals,
			GLfloat *texCoords,
			GLfloat *colors,
			GLuint *indices,
			int numVert,
			int numInd,
			GLuint shaderId)
{

	Model* m = new Model();
	memset(m, 0, sizeof(Model));

	m->vertexArray = vertices;
	m->texCoordArray = texCoords;
	m->normalArray = normals;
	m->indexArray = indices;
	m->numVertices = numVert;
	m->numIndices = numInd;

	BuildModelVAO2(m);
    Model_Type * model = new Model_Type();
    model->sModel      = m;
    model->sShaderId   = shaderId;

	mShaderMap[shaderId]->sModelData = model;
}

void ModelObject::freeModelData(Model * m){
        if(m->vertexArray!= NULL){
            free(m->vertexArray);

        }
        if(m->indexArray != NULL){
            free(m->indexArray);

        }
        if(m->texCoordArray != NULL){
            free(m->texCoordArray);

        }
        if(m->normalArray != NULL){
            free(m->normalArray);

        }
}


