#ifndef GPUCLOTHSIMULATION_H
#define GPUCLOTHSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "GPUSimulation.h"


#include <math.h>
#include <vector>


/** SIMULATION CONSTANTS **/
#define GPU_CLOTH_DIM 322
#define GPU_CLOTH_SIZE 4

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

        /********** PHYSICS CONSTANTS *****************/
        static const GLfloat GpuSpringDamping        = -.25f;
        static const GLfloat GpuSpringConstantStruct = 80.75f;
        static const GLfloat GpuSpringConstantBend   = 60.25f;
        static const GLfloat GpuSpringBreakFactor    = 3.2f;

        static const GLfloat GpuSystemDamping        = -0.0225f;
        static const GLfloat GpuSystemDeltaTime      = 2.0f/60.0f;
        static const GLfloat GpuSystemGravity        = -0.00981f;

        static const GLfloat GpuRestLengthStruct     = 2.0*GPU_CLOTH_SIZE/(GPU_CLOTH_DIM-1.0);
        static const GLfloat GpuRestLengthBend       = 4.0*GPU_CLOTH_SIZE/(GPU_CLOTH_DIM-1.0);

        /** Wind **/
        void updateWind();
        GLfloat mWindVector[3];
        GLfloat mTime;

        /********** GRAPHICS ********/
        ModelObject * mGPUClothScene;

    private:
        void configureSimulation();
};

#endif // CLOTHSIMULATIONGPU_H
