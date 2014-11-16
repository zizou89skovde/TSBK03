#include "ModelObject.h"

ModelObject::ModelObject()
{
}

ModelObject::~ModelObject()
{
    /** Clean up models **/
    Model_Type * m;
    for(std::vector<Model_Type*>::iterator it = mModelList.begin(); it != mModelList.end(); ++it) {
        m = *it;
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
    }



    /**Clear the list **/
    mModelList.clear();

    /** Delete Textures **/
    Texture_Type * texture;
    for(std::vector<Texture_Type*>::iterator it = mTextureList.begin(); it != mTextureList.end(); ++it) {
        texture = *it;
        glDeleteTextures(1,&texture->sTextureId);
        delete texture->sUniformName;
        delete texture;
    }
    mTextureList.clear();

    /** Delete Uniform**/
    Uniform_Type * uniform;
    for(std::vector<Uniform_Type*>::iterator it = mUniformList.begin(); it != mUniformList.end(); ++it) {
        uniform = *it;
        delete uniform->sData;
        delete uniform->sUniformName;
        delete uniform;
    }
    mUniformList.clear();

    Shader_Type * shader;
    for(std::vector<Shader_Type*>::iterator it = mShaderList.begin(); it != mShaderList.end(); ++it) {
        shader = *it;
        glDeleteProgram(shader->sShaderHandleGPU);
        delete shader;
    }
    mShaderList.clear();

     Transform_Type * t;
    for(std::vector<Transform_Type*>::iterator it = mTransformList.begin(); it != mTransformList.end(); ++it) {
        t = *it;
        delete t;
    }
    mTransformList.clear();
    printError("Model Object Clean up");
}
void ModelObject::drawVP(mat4 projectionMatrix, mat4 viewMatrix){
    Shader_Type * shader;
    for(std::vector<Shader_Type*>::iterator it = mShaderList.begin(); it != mShaderList.end(); ++it) {

        shader = *it;

        int program = shader->sShaderHandleGPU;
        glUseProgram(program);

        //Compute model2view , and model2view/projection matrix.
        mat4 mvMatrix = Mult(viewMatrix,*getTransform(shader->sShaderId));
        mat4 mvpMatrix = Mult(projectionMatrix,mvMatrix);

        glUniformMatrix4fv(glGetUniformLocation(program, "MV_Matrix"), 1, GL_TRUE, mvMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(program, "MVP_Matrix"), 1, GL_TRUE, mvpMatrix.m);

        uploadTexture(shader->sShaderId,program);
        uploadUniformFloat(shader->sShaderId,program);


    }

}
void ModelObject::draw(mat4 projectionMatrix, mat4 viewMatrix){
        Shader_Type * shader;
        for(std::vector<Shader_Type*>::iterator it = mShaderList.begin(); it != mShaderList.end(); ++it) {

        shader = *it;

        int program = shader->sShaderHandleGPU;
        glUseProgram(program);

        //Compute model2view , and model2view/projection matrix.
        mat4 mvMatrix = Mult(viewMatrix,*getTransform(shader->sShaderId));
        mat4 mvpMatrix = Mult(projectionMatrix,mvMatrix);

        glUniformMatrix4fv(glGetUniformLocation(program, "MV_Matrix"), 1, GL_TRUE, mvMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(program, "MVP_Matrix"), 1, GL_TRUE, mvpMatrix.m);

        uploadTexture(shader->sShaderId,program);
        uploadUniformFloat(shader->sShaderId,program);

        drawModel(shader->sShaderId,program);


    }

}

void ModelObject::uploadTexture(GLuint activeShaderId,GLuint activeShaderHandle){
    Texture_Type * texture;
    GLuint numActiveTextures = 0;
    for(std::vector<Texture_Type*>::iterator it = mTextureList.begin(); it != mTextureList.end(); ++it) {
        texture = *it;
        if(texture->sShaderId == activeShaderId ){
            glActiveTexture(GL_TEXTURE0+numActiveTextures);
            glBindTexture(GL_TEXTURE_2D, texture->sTextureId);
            glUniform1i(glGetUniformLocation(activeShaderHandle, texture->sUniformName), numActiveTextures);
            numActiveTextures++;
        }
    }

}
void ModelObject::uploadUniformFloat(GLuint activeShaderId,GLuint activeShaderHandle){
    Uniform_Type * uniform;
    for(std::vector<Uniform_Type*>::iterator it = mUniformList.begin(); it != mUniformList.end(); ++it) {
        uniform = *it;
        if(uniform->sShaderId == activeShaderId){
            switch(uniform->sSize){
                case 1:
                    glUniform1f(glGetUniformLocation(activeShaderHandle,uniform->sUniformName), uniform->sData[0]);
                break;
                case 2:
                    glUniform2f(glGetUniformLocation(activeShaderHandle, uniform->sUniformName), uniform->sData[0],uniform->sData[1]);
                break;
                case 3:
                    glUniform3f(glGetUniformLocation(activeShaderHandle, uniform->sUniformName), uniform->sData[0],uniform->sData[1],uniform->sData[2]);
                break;
                case 4:
                    glUniform4f(glGetUniformLocation(activeShaderHandle, uniform->sUniformName), uniform->sData[0],uniform->sData[1],uniform->sData[2],uniform->sData[3]);
                break;
                default:
                    break;
            }
        }
    }
}

void ModelObject::setUniform(GLfloat * data, GLuint sizeData, GLuint shaderId, const char * uniformName){
    Uniform_Type * uniform = new Uniform_Type();
    uniform->sData = data;
    uniform->sSize = sizeData;
    uniform->sShaderId = shaderId;
    memset(uniform->sUniformName,0,uniform->CHAR_LEN*sizeof(char));
    strcpy(uniform->sUniformName,uniformName);
    mUniformList.push_back(uniform);
}
void ModelObject::setShader(GLuint handleGPU, GLuint shaderId){
    Shader_Type * shader = new Shader_Type();
    shader->sShaderHandleGPU = handleGPU;
    shader->sShaderId        = shaderId;
    mShaderList.push_back(shader);
}
void ModelObject::setTexture(GLuint handle,GLuint shaderId,const char* uniformName){

    Texture_Type * newTexture = new Texture_Type();
    newTexture->sShaderId  = shaderId;
    newTexture->sTextureId    = handle;
    memset(newTexture->sUniformName,0,newTexture->CHAR_LEN*sizeof(char));
    strcpy(newTexture->sUniformName,uniformName);
    mTextureList.push_back(newTexture);


}
void ModelObject::setModel(Model * m, GLuint shaderId){
    Model_Type * model = new Model_Type();
    model->sModel = m;
    model->sShaderId = shaderId;
    mModelList.push_back(model);
}
void ModelObject::setTransform(mat4 transf,GLuint shaderId){
    Transform_Type * transformT = new Transform_Type();
    transformT->sTransform = transf;
    transformT->sShaderId  = shaderId;
    mTransformList.push_back(transformT);
}

void ModelObject::drawModel(GLuint shaderId,GLuint activeShaderHandle){
    Model_Type * m;
    for(std::vector<Model_Type*>::iterator it = mModelList.begin(); it != mModelList.end(); ++it) {
        m = *it;
        if(m->sShaderId == shaderId){

            char* normalAttributeString= NULL;
            if(m->sModel->normalArray != NULL) normalAttributeString = (char *)"in_Normal";

            char* textureAttributeString = NULL;
            if(m->sModel->texCoordArray != NULL) textureAttributeString = (char *)"in_TextureCoord";
            DrawModel(m->sModel, activeShaderHandle,(char *)"in_Position" ,normalAttributeString , textureAttributeString);

            return;
        }
    }
    printf("ERROR: No model was selected for current shader. Shader id: %d",shaderId);
}
mat4 * ModelObject::getTransform(GLuint shaderId){
    Transform_Type * t;
    for(std::vector<Transform_Type*>::iterator it = mTransformList.begin(); it != mTransformList.end(); ++it) {
        t = *it;
        if(t->sShaderId == shaderId)
            return &t->sTransform;
    }
    return NULL;
}

void ModelObject::uploadNewVertexData(GLfloat* dataBuffer,size_t bufferSize,GLuint shaderId){
    Model* model = NULL;
    Model_Type * m;
    for(std::vector<Model_Type*>::iterator it = mModelList.begin(); it != mModelList.end(); ++it) {
        m = *it;
        if(m->sShaderId == shaderId)
            model = m->sModel;
    }
    glBindVertexArray(model->vao);
    glBindBuffer(GL_ARRAY_BUFFER, model->vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, dataBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void ModelObject::BuildModelVAO2(Model *m){
	glGenVertexArrays(1, &m->vao);
	glGenBuffers(1, &m->vb);
	glGenBuffers(1, &m->ib);
	glGenBuffers(1, &m->nb);
	if (m->texCoordArray != NULL)
		glGenBuffers(1, &m->tb);

	glBindVertexArray(m->vao);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STREAM_DRAW);
	//glVertexAttribPointer(glGetAttribLocation(program, vertexVariableName), 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(glGetAttribLocation(program, vertexVariableName));

	// VBO for normal data
	glBindBuffer(GL_ARRAY_BUFFER, m->nb);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
	//glVertexAttribPointer(glGetAttribLocation(program, normalVariableName), 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(glGetAttribLocation(program, normalVariableName));

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

	Model* m = new Model(); //(Model *)malloc(sizeof(Model));
	memset(m, 0, sizeof(Model));

	m->vertexArray = vertices;
	m->texCoordArray = texCoords;
	m->normalArray = normals;
	m->indexArray = indices;
	m->numVertices = numVert;
	m->numIndices = numInd;

	BuildModelVAO2(m);
    Model_Type * model = new Model_Type();
    model->sModel = m;
    model->sShaderId = shaderId;
	this->mModelList.push_back(model);
}

