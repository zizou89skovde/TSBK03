#ifndef MODELOBJECT_H
#define MODELOBJECT_H
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "milli.h"
#include<stdio.h>

/**
 Following structures are used
 are to combine each item with the
 corresponding shader. Serving the same purpose
 as "map", TODO: change to map.
**/

#define SELECT_CONFIG  if(shader->sDepthTest == NO_DEPTH_TEST){glDisable(GL_DEPTH_TEST);}
#define DESELECT_CONFIG if(shader->sDepthTest == NO_DEPTH_TEST){glEnable(GL_DEPTH_TEST);}

#define MODEL_OBJECT_VERBOSE

#define NUMBER_OF_SPEEDPOINTS 20 // temp placement

typedef enum{
	ARRAYS,
	POINTSTEST,
	PATCHES,
	INSTANCED
}DrawMethod_Type;

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
}UniformFloat_Type;

typedef struct{
    mat4 sMatrix;
    GLuint sShaderId;
    char sUniformName[40];
    static const GLuint CHAR_LEN = 40;
}UniformMatrix_Type;

typedef struct{
    double s;
    int ms;
}Timestamp_Type;

typedef struct{
    GLuint sShaderId;
    Model* sModel;
}Model_Type;

typedef struct{
    GLuint sSpeedlinesShader;
    /** Speed line timestamp vector **/
    std::vector<Timestamp_Type> sTimestamps;
    /** Speed line point vector (world coordinates). Empty if not used. **/
    std::vector<vec3> sMovementPoints;
    /** Speed line transform vector. Empty if not used **/
    std::vector<mat4> sMovementTransforms;
    /** Model to hold VAO & VBO **/
    Model* m;
    /** Switch for line (1), model (2) or no (0) speedline drawing **/
    int sSpeedlineSwitch;
}Speed_Lines;

typedef struct{

    /** Key value for the shader map **/
    GLuint sShaderId;

    /** Shader program **/
    GLuint sProgramHandle;

    /** Textures **/
    std::map<std::string,Texture_Type*> sTextureMap;

    /** Uniform Lists*/
    std::map<std::string,UniformFloat_Type *> sUniformFloatMap;
    std::map<std::string,UniformMatrix_Type *> sUniformMatrixMap;

    /** GL flags **/
    DepthTest_Type sDepthTest;

    /** Model to world transform **/
    mat4 sTransform;
    Tranform_Composition_Type sComposition;

	/** Draw method type **/
	DrawMethod_Type sDrawMethod;

    /** Taking care of speed lines **/
    Speed_Lines sSpeedlines; 

	/** Number of instances (uses if INSTANCE-draw method is selected **/
	GLuint sNumInstances;

    Model_Type * sModelData;

}Shader_Type;

typedef std::map<GLuint,Shader_Type*>::iterator ShaderIterator;
typedef std::map<std::string,Texture_Type*>::iterator TextureIterator;
typedef std::map<std::string,UniformFloat_Type*>::iterator UniformFloatIterator;
typedef std::map<std::string,UniformMatrix_Type*>::iterator UniformMatrixIterator;

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
        void clone(GLuint srcShaderId, GLuint dstShaderId, bool copyUniforms, bool copyTextures);
        /** Set functions **/
        void setModel(Model * m,GLuint shaderId);
        void setShader(GLuint handle,GLuint id,Tranform_Composition_Type  composition);
        void setShader(GLuint handle,GLuint id,Tranform_Composition_Type  composition,DepthTest_Type depthTest);
        void setShader(GLuint handle,GLuint id);
        void setTexture(GLuint handle,GLuint shaderId,const char* uniformName);
        void setUniformFloat(GLfloat* data, GLuint sizeData, GLuint shaderId, const char* uniformName);
        void setUniformMatrix(mat4 data, GLuint shaderId, const char* uniformName);
        void setUniformFloat(const GLfloat data, GLuint shaderId, const char* uniformName);
        void setTransform(mat4 transf,GLuint id);
        void setSpeedlinesInit(GLuint shaderId);
        void setSpeedModelsInit(GLuint shaderId);
        void setNoSpeedlines(GLuint shaderId);
        void registerTimeSpeedlines(GLuint shaderId);
		void setDrawMethod(DrawMethod_Type method, GLuint shaderId);
		void setNumInstances(GLuint numInstances, GLuint shaderId);



		/* Get functions */
		DrawMethod_Type getDrawMethod(GLuint shaderId);
        int getSpeedlineMode(GLuint shaderId);


        void replaceTexture(GLuint handle,GLuint shaderId,const char* uniformName);
        void replaceUniformFloat(GLfloat* handle,GLuint shaderId,const char* uniformName);
        void replaceUniformMatrix(mat4 matrix,GLuint shaderId,const char* uniformName);
        /** Misc functions **/
         void freeModelData(Model * m);
        mat4 * getTransform(GLuint ShaderId);
        void flipModels(GLuint shaderId);

    protected:
    private:

        /** Private help functions **/
        void   uploadUniformFloat(UniformFloat_Type* uniform);
        void   uploadUniformMatrix(UniformMatrix_Type* uniform);
        void   selectTexture(Shader_Type * shader);
        void   uploadTransform(Shader_Type * shader,mat4 projectionMatrix,mat4 viewMatrix);
        void   updateSpeedlines(Shader_Type * shader);
        void   updateSpeedModels(Shader_Type * shader);

        /** Private data containers **/
        std::map<GLuint,Shader_Type*> mShaderMap;

        /** Private draw functions **/
		void selectDrawMethod(Shader_Type * shader, mat4 projectionMatrix, mat4 viewMatrix);
        void drawModel(Shader_Type* shader);
		void drawPoints(Shader_Type *  shader,mat4 projectionMatrix, mat4 viewMatrix);
		void drawArrays(Shader_Type *  shader,mat4 projectionMatrix, mat4 viewMatrix);
        void drawSpeedlines(Shader_Type* shader, mat4 projectionMatrix, mat4 viewMatrix);
        void drawSpeedModels(GLuint shaderId, mat4 projectionMatrix, mat4 viewMatrix);
		void drawPatches(Shader_Type * shader,mat4 projectionMatrix, mat4 viewMatrix);
		void drawInstanced(Shader_Type * shader,mat4 projectionMatrix, mat4 viewMatrix);


};

#endif // MODELOBJECT_H
