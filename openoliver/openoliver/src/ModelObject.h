#ifndef MODELOBJECT_H
#define MODELOBJECT_H

#include <vector>

#include "loadobj.h"
#include "VectorUtils3.h"
#include <stdlib.h>
#include <string.h>
#include<stdio.h>
class ModelObject
{
    public:

        ModelObject();
        virtual ~ModelObject();

        void draw(mat4 projectionMatrix, mat4 viewMatrix);
        void setModel(Model * m);
        void BuildModelVAO2(Model *m);
        void LoadDataToModel(
			GLfloat *vertices,
			GLfloat *normals,
			GLfloat *texCoords,
			GLfloat *colors,
			GLuint *indices,
			int numVert,
			int numInd);
        void uploadNewVertexData(GLfloat* dataBuffer,size_t bufferSize);
        void setShader(GLuint handle, GLuint id);
        void setTexture(GLuint handle, GLuint id);
        void setTransform(mat4 transf);
    protected:
    private:

        GLuint getTexture(GLuint id);
        GLuint getShader(GLuint id);

        GLuint  shaderHandle[10][2];
        GLuint   numberOfShaders;
        GLuint  textureHandle[20][2];
        GLuint   numberOfTextures;
        std::vector<Model*> modelVec;
        std::vector<mat4> modelTransformVec;

};

#endif // MODELOBJECT_H
