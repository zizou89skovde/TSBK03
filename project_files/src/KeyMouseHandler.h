#ifndef KEYMOUSEHANDLER_H
#define KEYMOUSEHANDLER_H

#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "CPUClothSimulation.h"
#include "PostProcessing.h"
#include "CelShading.h"
class KeyMouseHandler
{
    public:

        SimulationClass* mClothSimulation;
        KeyMouseHandler();
        virtual ~KeyMouseHandler();

        void keyPress(unsigned char key, int x, int y);
        void mouseHandle(int x, int y);
        void setPostProcessing(PostProcessing* postProcessing){mPostProcessing = postProcessing;};
        void setCelShading(CelShading* celShading){mCelShading = celShading;};
        mat4 getViewMatrix();

        //void mouseDown(int x, int y);
        void mouseUp();

    protected:
    private:
        PostProcessing* mPostProcessing;
        CelShading* mCelShading;

        vec3 *cameraEye;
        vec3 *cameraCenter;
        vec3 *cameraUp;

        // Previous mouse position
        GLfloat xPrev, yPrev;

        vec3 lookAtVec();
        vec3 rightVec();
        vec3 upVec();

};

#endif // KEYMOUSEHANDLER_H
