#ifndef GPUCLOTHSIMULATION_H
#define GPUCLOTHSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"

#include <math.h>
#include <vector>


/** SIMULATION CONSTANTS **/
#define GPU_CLOTH_RES 64
#define GPU_CLOTH_DIM (GPU_CLOTH_RES+1)
#define GPU_CLOTH_SIZE 4


/** GRAPHICS CONSTANTS **/
#define GPU_FLOATS_PER_VERTEX 3
#define GPU_FLOATS_PER_POSITION 4
#define INDICES_PER_QUAD 6
#define VERTICES_PER_TRIANGLE 3

/** SHADER ID'S **/
#define GPU_SHADER_CLOTH 0
#define GPU_SHADER_COMPUTE_POSITION 1
#define GPU_SHADER_EVALUATE_SPRINGS  2

class GPUClothSimulation
{
    public:
        GPUClothSimulation(GLuint * w,GLuint *h);
        virtual ~GPUClothSimulation();

        void draw(mat4 projectionMatrix, mat4 viewMatrix);

    protected:

        /********** PHYSICS CONSTANTS *****************/
        static const GLfloat GpuSpringDamping        = -.65f;
        static const GLfloat GpuSpringConstantStruct = 50.75f;
        static const GLfloat GpuSpringConstantBend   = 50.25f;
        static const GLfloat GpuSpringBreakFactor    = 3.2f;

        static const GLfloat GpuSystemDamping        = -0.0125f;
        static const GLfloat GpuSystemDeltaTime      = 1.0f/60.0f;
        static const GLfloat GpuSystemGravity        = -0.00981f;

        static const GLfloat GpuRestLengthStruct     = 2.0*GPU_CLOTH_SIZE/GPU_CLOTH_RES;
        //static const GLfloat GpuRestLengthShear      = ;
        static const GLfloat GpuRestLengthBend        = 4.0*GPU_CLOTH_SIZE/GPU_CLOTH_RES;
        /********** GRAPHICS & GPU COMPUTATION ********/
        void shiftFBO();

        void uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId);
        void uploadSquareModelData(ModelObject * modelObj,GLuint shaderId);
        void generatePositionBuffer(FBOstruct* fbo);
        void generateSpringBuffer(FBOstruct* fbo);

        long unsigned int getSpringState(GLuint x,GLuint y);

        ModelObject * mGPUClothScene;
        ModelObject * mGPUComputation;

        static const GLuint NumberOfFBOs = 3;
        GLuint mActiveFBOIndex;

        std::vector<FBOstruct *> ListFBOPosition;
        std::vector<FBOstruct *> ListFBOSpringState;

        /********* FBO specific ************/
        void enableFbo(FBOstruct * fbo);
        FBOstruct* getActiveFBO();
        GLuint* mScreenWidth;
        GLuint* mScreenHeight;
    private:
};

#endif // CLOTHSIMULATIONGPU_H
