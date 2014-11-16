#ifndef GPUCLOTHSIMULATION_H
#define GPUCLOTHSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadTGA.h"

#include <math.h>
#include <vector>


/** SIMULATION CONSTANTS **/
#define GPU_CLOTH_RES 64
#define GPU_CLOTH_DIM (GPU_CLOTH_RES+1)
#define GPU_CLOTH_SIZE 4

/** SPRING STATE CODES **/
#define SPRING_STRUCT_RIGHT         1
#define SPRING_STRUCT_UP            2
#define SPRING_SHEAR_UP_RIGHT       4
#define SPRING_SHEAR_DOWN_RIGHT     8
#define SPRING_BEND_RIGHT           16
#define SPRING_BEND_UP              32



/** GRAPHICS CONSTANTS **/
#define GPU_FLOATS_PER_TEXEL 2
#define INDICES_PER_QUAD 6
#define VERTICES_PER_TRIANGLE 3

/** SHADER ID'S **/
#define GPU_CLOTH_SHADER 0
#define GPU_COMPUTE_SHADER 1

class GPUClothSimulation
{
    public:
        GPUClothSimulation();
        virtual ~GPUClothSimulation();

        void draw(mat4 projectionMatrix, mat4 viewMatrix);
    protected:

        /********** GRAPHICS & GPU COMPUTATION ********/
        void uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId);

        void generatePositionBuffer(FBOstruct* fbo);
        void generateSpringBuffer(FBOstruct* fbo);

        GLubyte getSpringState(GLuint x,GLuint y);

        ModelObject * mGPUClothScene;
        ModelObject * mGPUPositionComputation;
        ModelObject * mGPUSpringStateUpdating;

        std::vector<FBOstruct *> ListFBOPosition;
        std::vector<FBOstruct *> ListFBOSpringState;
    private:
};

#endif // CLOTHSIMULATIONGPU_H
