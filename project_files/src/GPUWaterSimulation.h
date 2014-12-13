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
#define GPU_WATER_SIZE 12

/** SHADER ID'S **/
#define GPU_SHADER_WATER 0
#define GPU_SHADER_SIMPLE_WATER 1

/** Wind ON/OFF**/
//#define WIND


class GPUWaterSimulation:public GPUSimulation
{
    public:
        GPUWaterSimulation(GLuint * w,GLuint *h);
        virtual ~GPUWaterSimulation();
        void initialize();
        void draw(mat4 projectionMatrix, mat4 viewMatrix);

    protected:

        /********** PHYSICS CONSTANTS *****************/


        static const GLfloat GpuSystemDamping        = -0.0525f;
        static const GLfloat GpuSystemDeltaTime      = 1.0f/60.0f;

        static const GLfloat GpuSpringDamping        = -.805f;
        static const GLfloat GpuSpringConstant       = 20.75f;
        static const GLfloat GpuRestLength          = 2.0*GPU_WATER_SIZE/(GPU_WATER_DIM-1.0);
        /** Wind **/

        /** Rain drops **/
        GLfloat mPreviousTime;
        static const GLfloat RainFrequency = 20;
        static const GLfloat RainDropForce = 0.008f;
        static const GLfloat RainDropRadius = 0.010f;
        /********** GRAPHICS ********/
        ModelObject * mGPUWaterScene;

		GLuint mPreviousScreenWidth;
		GLuint mPreviousScreenHeight;

    private:
        void configureSimulation();
        void raindrops();
};

#endif // CLOTHSIMULATIONGPU_H
