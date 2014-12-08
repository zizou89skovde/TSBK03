#ifndef MODELOBJECT_H
#define MODELOBJECT_H
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include<stdio.h>

/**
 Following structures are used
 are to combine each item with the
 corresponding shader. Serving the same purpose
 as "map", TODO: change to map.
**/

typedef enum{
    //MPV = 0, DEFAUULT
    MVP = 1,
    VP,
    P,
    NONE
}Tranform_Composition_Type;

typedef enum{
    DEPTH_TEST,
    NO_DEPTH_TEST
}DepthTest_Type;

typedef struct{
    char sUniformName[40];
    static const GLuint CHAR_LEN = 40;
    GLuint sTextureId;
    GLuint sShaderId;
}Texture_Type;

typedef struct{
    char sUniformName[40];
    static const GLuint CHAR_LEN = 40;
    GLuint sSize;
    GLfloat sData[4];
    GLuint sShaderId;
}Uniform_Type;

typedef struct{
    GLuint sShaderId;
    GLuint sShaderHandleGPU;
    Tranform_Composition_Type sComposition;
    DepthTest_Type sDepthTest;
}Shader_Type;

typedef struct{
    GLuint sShaderId;
    Model* sModel;
}Model_Type;



typedef struct{
    GLuint sShaderId;
    mat4 sTransform;
}Transform_Type;



class ModelObject
{
    public:

        ModelObject();
        virtual ~ModelObject();

        /** Misc functions, used by Cloth-classes **/
        void LoadDataToModel(
			GLfloat *vertices,
			GLfloat *normals,
			GLfloat *texCoords,
			GLfloat *colors,
			GLuint *indices,
			int numVert,
			int numInd,
			GLuint shaderId);
        void BuildModelVAO2(Model *m);
        void uploadNewVertexData(GLfloat* dataBuffer,size_t bufferSize,GLuint shaderId);

        /** Draw Function **/
        void draw(mat4 projectionMatrix, mat4 viewMatrix);
        void drawBuffers(GLuint shaderId,GLuint numBuffers,GLuint * attachment);
        void draw(GLuint shaderId,mat4 projectionMatrix, mat4 viewMatrix);
        /** Set functions **/
        void setModel(Model * m,GLuint shaderId);
        void setShader(GLuint handle,GLuint id,Tranform_Composition_Type  composition);
        void setShader(GLuint handle,GLuint id,Tranform_Composition_Type  composition,DepthTest_Type depthTest);
        void setShader(GLuint handle,GLuint id);
        void setTexture(GLuint handle,GLuint shaderId,const char* uniformName);
        void setUniform(GLfloat* data, GLuint sizeData, GLuint shaderId, const char* uniformName);
        void setUniform(const GLfloat data, GLuint shaderId, const char* uniformName);
        void setTransform(mat4 transf,GLuint id);

        void replaceTexture(GLuint handle,const char* uniformName);
        void replaceUniform(GLfloat* handle,const char* uniformName);

        /** Get functions **/
        mat4 * getTransform(GLuint ShaderId);
        void flipModels();

    protected:
    private:

        /** Private help functions **/
        void   uploadUniformFloat(Uniform_Type* uniform);

        void   uploadTexture(GLuint activeShaderIndex,GLuint activeShaderHandle);
        void   uploadTransform(Shader_Type * shader,mat4 projectionMatrix,mat4 viewMatrix);

        /** Private data containers **/
        std::vector<Texture_Type*> mTextureList;
        std::vector<Uniform_Type*> mUniformList;
        std::vector<Shader_Type * > mShaderList;
        std::vector<Model_Type*>  mModelList;
        std::vector<Transform_Type *> mTransformList;

        /** Private GetFunctions **/
        void drawModel(GLuint shaderId,GLuint activeShaderHandle);
        void freeModelData(Model * m);


};

#endif // MODELOBJECT_H
