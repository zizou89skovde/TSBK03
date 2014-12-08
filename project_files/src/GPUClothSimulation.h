#ifndef GPUCLOTHSIMULATION_H
#define GPUCLOTHSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "GPUSimulation.h"


#include <math.h>
#include <vector>


/** SIMULATION CONSTANTS **/
#define GPU_CLOTH_DIM 256
#define GPU_CLOTH_SIZE 2

/** SHADER ID'S **/
#define GPU_SHADER_CLOTH 0
#define GPU_SHADER_SPHERE 1



class GPUClothSimulation:public GPUSimulation
{
    public:
        GPUClothSimulation(GLuint * w,GLuint *h);
        virtual ~GPUClothSimulation();
        void initialize();
        void draw(mat4 projectionMatrix, mat4 viewMatrix);
        void updateSpherePosition(vec3 deltaPos);

    protected:


    private:
             /********** PHYSICS CONSTANTS *****************/
        static const GLfloat GpuSpringDamping        = -.55f;
        static const GLfloat GpuSpringConstantStruct = 70.75f;
        static const GLfloat GpuSpringConstantBend   = 70.25f;
        static const GLfloat GpuSpringBreakFactor    = 3.2f;

        static const GLfloat GpuSystemDamping        = -0.0325f;
        static const GLfloat GpuSystemDeltaTime      = 1.0f/60.0f;
        static const GLfloat GpuSystemGravity        = -0.00981f;

        static const GLfloat GpuRestLengthStruct     = 1.0*GPU_CLOTH_SIZE/(GPU_CLOTH_DIM-1.0);
        static const GLfloat GpuRestLengthBend       = 2.0*GPU_CLOTH_SIZE/(GPU_CLOTH_DIM-1.0);

        /** Wind **/
        void updateWind();
        GLfloat mWindVector[3];
        GLfloat mTime;

        /********** GRAPHICS ********/
        ModelObject * mGPUClothScene;

        static const GLfloat mSphereRadius = 0.5;
        static const GLfloat mSphereSpeed = 0.001;
        static const GLfloat mSphereRouteLength = 1.0;
        static const GLfloat mSphereOffsetX = 0.0;
        static const GLfloat mSphereOffsetY = 1.5;
        static const GLfloat mSphereOffsetZ = -1.0;

        GLfloat mObjectDirection;
        void configureSimulation();
        void updateCollisionObject();
};

#endif // CLOTHSIMULATIONGPU_H
