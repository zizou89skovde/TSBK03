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

//LBM2887M
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureHandle[0][0]);
        glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
        int size = modelVec.size();
        printf("Model vector size: %d \n",size);
        DrawModel(modelVec.at(0), program, "in_Position", "in_Normal", NULL);
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

void ModelObject::setModel(Model * m){
    modelVec.push_back(m);
}

GLuint ModelObject::getTexture(GLuint id){
    for(int i = 0; i < numberOfTextures; i++){
        if(textureHandle[i][1] == id)
            return textureHandle[i][0];
    }
    return 0;
}

GLuint ModelObject::getShader(GLuint id){
    for(int i = 0; i < numberOfShaders; i++){
        if(shaderHandle[i][1] == id)
            return shaderHandle[i][0];
    }
    return 0;
}
void ModelObject::setTransform(mat4 transf){

}

