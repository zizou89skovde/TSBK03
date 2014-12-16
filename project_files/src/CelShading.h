#ifndef CELSHADING_H
#define CELSHADING_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"
#include "ScreenSize.h"

#define CEL_SPHERE_SHADER_ID 0
#define CEL_SCREEN_QUAD_ID 1
#define CEL_DERIVE_SHADER_ID 2

class CelShading
{
    public:
        CelShading(GLuint* w, GLuint* h);
        void initialize(FBOstruct* fbo_in);
        void draw(mat4 projectionMatrix, mat4 viewMatrix);
        virtual ~CelShading();

    private:
        // Generate a depth FBO to derive
        void generateDepthFBO(FBOstruct* fbo, GLuint w, GLuint h);
        // Get the depth image
        void drawDepth(mat4 proj, mat4 view);
        // Move sphere according to speed
        void updateSpherePosition(vec3 deltaPos);
        // Load sketch image to celshader
        void loadSketch();
        // Square to draw from FBO
        void uploadSquareModelData(ModelObject* modelObj, GLuint shaderId);

        // Depth FBO stuff
		FBOstruct mDepthFBO;
		//static const GLuint mFBOWidth  = SCREEN_WIDTH;
		//static const GLuint mFBOHeight = SCREEN_HEIGHT;

        FBOstruct* mFBO;
        FBOstruct* mContourFBO;
        
        ModelObject* mCelObject;

        // Screen boundaries not to be broken in shaders
		GLuint* mScreenHeight;
		GLuint* mScreenWidth;

        // Sphere inits
        typedef struct{
            vec3 *position;
            float radius;
        } Sphere;

        Sphere* mSphere;
        static const GLfloat mSphereRadius = 0.5;
        static const GLfloat mSphereSpeed = 0.03;
        static const GLfloat mSphereOffsetX = 0.0;
        static const GLfloat mSphereOffsetY = 0.0;
        static const GLfloat mSphereOffsetZ = 3.0;
};


#endif
