#include "ModelObject.h"

ModelObject::ModelObject()
{
     //   modelVec          = new std::vector<Model*>();
      //  modelTransformVec = new std::vector<mat4>();

        numberOfShaders  = 0;
        numberOfTextures = 0;
}

ModelObject::~ModelObject()
{
       // delete modelVec;
       // delete modelTransformVec;
}

void ModelObject::draw(mat4 projectionMatrix, mat4 viewMatrix){

  // for(int i = 0; i < numberOfShaders; i++){
        int program = shaderHandle[0][0];
        glUseProgram(program);

        //Compute model2view , and model2view/projection matrix.
        mat4 mvMatrix = Mult(viewMatrix,modelTransformVec.at(0));
        mat4 mvpMatrix = Mult(projectionMatrix,mvMatrix);

        glUniformMatrix4fv(glGetUniformLocation(program, "MV_Matrix"), 1, GL_TRUE, mvMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(program, "MVP_Matrix"), 1, GL_TRUE, mvpMatrix.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureHandle[0][0]);
        glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
        DrawModel(modelVec.at(0), program, (char *)"in_Position", (char *)"in_Normal", NULL);
 //   }

}
void ModelObject::setShader(GLuint handle, GLuint id){
    shaderHandle[numberOfShaders][0] = handle;
    shaderHandle[numberOfShaders][1] = id;
    numberOfShaders++;
}
void ModelObject::setTexture(GLuint handle, GLuint id){
    textureHandle[numberOfTextures][0] = handle;
    textureHandle[numberOfTextures][1] = id;
    numberOfTextures++;
}

// Loader for inline data to Model (almost same as LoadModelPlus)
void ModelObject::LoadDataToModel(
			GLfloat *vertices,
			GLfloat *normals,
			GLfloat *texCoords,
			GLfloat *colors,
			GLuint *indices,
			int numVert,
			int numInd)
{

	Model* m = (Model *)malloc(sizeof(Model));
	memset(m, 0, sizeof(Model));

	m->vertexArray = vertices;
	m->texCoordArray = texCoords;
	m->normalArray = normals;
	m->indexArray = indices;
	m->numVertices = numVert;
	m->numIndices = numInd;

	BuildModelVAO2(m);

	this->modelVec.push_back(m);
}

void ModelObject::uploadNewVertexData(GLfloat* dataBuffer,size_t bufferSize){
    Model* m = modelVec.at(0);
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vb);
     glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, &dataBuffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModelObject::BuildModelVAO2(Model *m/*,
			GLuint program,
			char* vertexVariableName,
			char* normalVariableName,
			char* texCoordVariableName*/)
{
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


void ModelObject::setModel(Model * m){
    modelVec.push_back(m);
}

GLuint ModelObject::getTexture(GLuint id){
    for(GLuint i = 0; i < numberOfTextures; i++){
        if(textureHandle[i][1] == id)
            return textureHandle[i][0];
    }
    return 0;
}

GLuint ModelObject::getShader(GLuint id){
    for(GLuint i = 0; i < numberOfShaders; i++){
        if(shaderHandle[i][1] == id)
            return shaderHandle[i][0];
    }
    return 0;
}
void ModelObject::setTransform(mat4 transf){
 modelTransformVec.push_back(transf);
}

