#ifndef CELSHADING_H
#define CELSHADING_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"
#include "ScreenSize.h"
#include <vector>

#define CEL_OBJECT_SHADER_ID 0
#define CEL_SCREEN_QUAD_ID 1
#define CEL_DERIVE_SHADER_ID 2

class CelShading
{
    public:
        CelShading(GLuint* w, GLuint* h);
        void initialize(FBOstruct* fbo_in);
        void draw(mat4 projectionMatrix, mat4 viewMatrix);
        virtual ~CelShading();

        void switchCelShading();
        void increaseSteps();
        void decreaseSteps();
        void increaseContourThresh();
        void decreaseContourThresh();

        // Fire away object
        void fireObject();
        void changeSpeedlines();
        void changeGravity();

    private:
        // Generate a depth FBO to derive
        void generateDepthFBO(FBOstruct* fbo, GLuint w, GLuint h);
        // Get the depth image
        void drawDepth(mat4 proj, mat4 view);
        // Move object according to speed
        void updateObjectPosition(vec3 deltaPos);
        void updateObjectTransform();
        // Load sketch image to celshader
        void loadSketch();
        // Square to draw from FBO
        void uploadSquareModelData(ModelObject* modelObj, GLuint shaderId);

        GLfloat randFloat(GLfloat min, GLfloat max);

        // FBO:s to perform operations on
        FBOstruct* mFBO;
        FBOstruct* mContourFBO;
        
        ModelObject* mCelObject;

        // Cel shading uniforms
        GLfloat mCelShadingSwitch;
        GLfloat mCelShadingSteps;
        GLfloat mCelShadingContourThresh;

        // Screen boundaries not to be broken in shaders
		GLuint* mScreenHeight;
		GLuint* mScreenWidth;

        // Object inits
        typedef struct{
            vec3* position;
            vec3* velocity;
            vec3* rotation;
            vec3* angular; // Angular velocity
            mat4* trans;
            float radius;
        } Object;

        Object* mObject;
        static const GLfloat mObjectRadius = 0.5;
        static const GLfloat mObjectSpeed = 0.03;
        static const GLfloat mObjectOffsetX = 0.0;
        static const GLfloat mObjectOffsetY = 0.0;
        static const GLfloat mObjectOffsetZ = 3.0;
        GLfloat mGravity;

};


#endif
