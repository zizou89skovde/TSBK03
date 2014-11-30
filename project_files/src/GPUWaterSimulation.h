#ifndef GPUWATERSIMULATION_H
#define GPUWATERSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "GPUSimulation.h"
#include "LoadTGA.h"

#include <math.h>
#include <vector>


/** SIMULATION CONSTANTS **/
#define GPU_WATER_DIM 128
#define GPU_WATER_SIZE 16

/** SHADER ID'S **/
#define GPU_SHADER_WATER 0



class GPUWaterSimulation:public GPUSimulation
{
    public:
        GPUWaterSimulation(GLuint * w,GLuint *h);
        virtual ~GPUWaterSimulation();
        void initialize();
        void draw(mat4 projectionMatrix, mat4 viewMatrix);

    protected:

        /********** PHYSICS CONSTANTS *****************/


        static const GLfloat GpuSystemDamping        = -0.00525f;
        static const GLfloat GpuSystemDeltaTime      = 1.0f/60.0f;

        static const GLfloat GpuSpringDamping        = -.805f;
        static const GLfloat GpuSpringConstant       = 20.75f;
        static const GLfloat GpuRestLength          = 2.0*GPU_WATER_SIZE/(GPU_WATER_DIM-1.0);
        /** Wind **/


        /********** GRAPHICS ********/
        ModelObject * mGPUWaterScene;

    private:
        void configureSimulation();
};

#endif // CLOTHSIMULATIONGPU_H
