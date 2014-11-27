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
#define GPU_WATER_DIM 64
#define GPU_WATER_SIZE 20

/** SHADER ID'S **/
#define GPU_SHADER_WATER 0



class GPUWaterSimulation:public GPUSimulation
{
    public:
        GPUWaterSimulation(GLuint * w,GLuint *h,const char * vertexShader, const char * fragmentShader);
        virtual ~GPUWaterSimulation();
        void draw(mat4 projectionMatrix, mat4 viewMatrix);

    protected:

        /********** PHYSICS CONSTANTS *****************/
        static const GLfloat GpuSpringDamping        = -.25f;

        static const GLfloat GpuSystemDamping        = -0.00125f;
        static const GLfloat GpuSystemDeltaTime      = 2.0f/60.0f;

        /** Wind **/
        GLfloat mTime;

        /********** GRAPHICS ********/
        ModelObject * mGPUWaterScene;

    private:
        void configureSimulation();
};

#endif // CLOTHSIMULATIONGPU_H
