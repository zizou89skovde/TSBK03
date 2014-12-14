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

class Environment
{
    public:
        Environment(GLuint * w, GLuint * h);

        void draw(mat4 proj, mat4 view);
        void drawDepthModels(mat4 proj, mat4 view);
        void drawReflectedModels(mat4 projectionMatrix,mat4 viewMatrix);
        void drawRefractedModels(mat4 projectionMatrix,mat4 viewMatrix);
        FBOstruct * getEnvironmentFBO();
        FBOstruct * getEnvironmentReflectedFBO();
        void setDepthModels(ModelObject * modelObject,GLuint shaderId);
        void setReflectedModels(ModelObject * modelObject,GLuint shaderId);
        virtual ~Environment();
    protected:
    private:

        void initializeSkyBox();

        void setClip(bool enabled);

        /** Environment model object for rendering **/
        ModelObject* mEnvironmentModel;

        /** Render targets. Used for reflecting and refracting the water **/
        const static GLuint mFBOWidth   =  SCREEN_WIDTH;
        const static GLuint mFBOHeight  =  SCREEN_WIDTH;
        FBOstruct* mEnvironmentFBO;
        FBOstruct* mEnvironmentReflectionFBO;

        /** Pointers to screen dimension known by the main **/
        GLuint *mScreenWitdh;
        GLuint *mScreenHeight;


        std::vector<ModelItem> mReflectionModelMap;
        std::vector<ModelItem> mDepthModelMap;

};

#endif // TERRAIN_H
