#ifndef TERRAIN_H
#define TERRAIN_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "ScreenSize.h"
#include "LoadTGA.h"

//#include "LoadTGA.h"
#define SKYBOX_SHADER 0

typedef std::pair<GLuint,ModelObject*> ModelItem;
typedef std::vector<ModelItem>::iterator ModelIterator;

typedef struct{
        GLfloat sDimension; /*Number of elements per size*/
        GLfloat sResolution; /* size divided by dimension */
        GLfloat sSize[3];
        GLfloat sOffset[3];
        GLuint  sHeightMapHandle;
        GLuint  sHeightMapHandleHighRes;
        GLuint  sNormalMapHandleHighRes;
        GLfloat sTextureSize[2];
        void setSize(GLfloat sx,GLfloat sy,GLfloat sz){sSize[0] =sx;sSize[1] =sy;sSize[2] =sz;}
        void setTextureSize(GLfloat ss,GLfloat st){sTextureSize[0] =ss;sTextureSize[1]=st;}
        void setOffset(GLfloat ox,GLfloat oy,GLfloat oz){sOffset[0] =ox;sOffset[1] =oy;sOffset[2] =oz;}
}EnvironmentMetaData;

class Environment
{
    public:
        Environment(GLuint * w, GLuint * h);

        void draw(mat4 proj, mat4 view);
        void drawDepthModels(mat4 proj, mat4 view);

        void drawReflectedModels(mat4 projectionMatrix,mat4 viewMatrix);
        void drawRefractedModels(mat4 projectionMatrix,mat4 viewMatrix);

        void setDepthModels(ModelObject * modelObject,GLuint shaderId);
        void setReflectedModels(ModelObject * modelObject,GLuint shaderId);
        void setRefractedModels(ModelObject * modelObject,GLuint shaderId);


        void setDefaultFBO(FBOstruct * fbo);

        FBOstruct * getReflectedFBO();
        FBOstruct * getRefractionFBO();
        FBOstruct * getDepthFBO();

        EnvironmentMetaData getMetaData();

        virtual ~Environment();
    protected:
    private:

        void initializeSkyBox();
        void initializeEnvironment();

        void setClip(bool enabled);

        /** Environment model object for rendering **/
        ModelObject* mEnvironmentModel;

    

        /** Render targets. Used for reflecting and refracting the water **/
        const static GLuint mFBOWidth   =  SCREEN_WIDTH;
        const static GLuint mFBOHeight  =  SCREEN_HEIGHT;

        /** Pointer to the current screen size **/
        GLuint *mScreenWitdh;
        GLuint *mScreenHeight;

        /** Meta data **/
        EnvironmentMetaData mEnvironmentMetaData;

        /** FBOz **/
        FBOstruct* mEnvironmentDepthFBO;
        FBOstruct* mRefractedFBO;
        FBOstruct* mReflectionFBO;

        FBOstruct* mDefaultFBO;

        std::vector<ModelItem> mRefracedModelMap;
        std::vector<ModelItem> mReflectionModelMap;
        std::vector<ModelItem> mDepthModelMap;

};

#endif // TERRAIN_H
