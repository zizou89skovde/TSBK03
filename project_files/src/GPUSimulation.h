#ifndef GPUSIMULATION_H
#define GPUSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "SimulationClass.h"

#include <math.h>
#include <vector>


/** GRAPHICS CONSTANTS **/
#define GPU_FLOATS_PER_VERTEX 3
#define GPU_FLOATS_PER_POSITION 4
#define INDICES_PER_QUAD 6
#define VERTICES_PER_TRIANGLE 3

/** MISC **/
#define DUMMY_TEXTURE -1

class GPUSimulation: public SimulationClass
{
    public:
        GPUSimulation(GLuint* w,GLuint* h, const char * vertexShader, const char * fragmentShader);
        virtual ~GPUSimulation();

        typedef struct{
            GLuint  GridDimension;
            GLfloat GridSize;
            GLfloat GridOffset[3];
            bool isUpward;
            const char * VertexShader;
            const char * FragmentShader;
        }SimulationData_Type;


    protected:
        /** Functions available for subclasses **/
        void intializeSimulation(SimulationData_Type * simulationData);
        void uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId);

        void setSimulationConstant(GLfloat constant, const char *uniformName);
        void setSimulationConstant(GLfloat* constant,GLuint sizeConstant, const char *uniformName);
        void setSimulationTexture(GLuint texid,const char* uniformName);

        void replaceSimulationConstant(GLfloat constant, const char *uniformName);
        void replaceSimulationConstant(GLfloat* constant, const char *uniformName);
        void replaceSimulationTexture(GLuint texid,const char* uniformName);

        FBOstruct* simulate(GLuint numIterations);

        GLuint* mScreenWidth;
        GLuint* mScreenHeight;
    private:

        /* Internal ID of the gpu computation program */
        static const GLuint GPU_SHADER_COMPUTE = 100;

        /** Buffers and Coordinates **/
        void uploadSquareModelData(ModelObject * modelObj,GLuint shaderId);
        void generatePositionBuffer(FBOstruct* fbo);

        /** Enum array holding the attachment codes **/
        GLenum FRAME_ATTACHMENT[2];

        /** For mass spring systems **/
        long unsigned int getSpringState(GLuint x,GLuint y);

        /** Model object **/
        ModelObject * mGPUSimulation;

        /** List of FBOs  **/
        std::vector<FBOstruct *> ListFBOPosition;

        /** FBO Stuff **/
        void shiftFBO();
        void enableFbo(FBOstruct * fbo);
        FBOstruct* getActiveFBO();

        /** Index where the most recent comptution is done **/
        GLuint mActiveFBOIndex;

        /** Num FBO used for computation **/
        static const GLuint NumberOfFBOs = 2;

        /** Simulation data **/
        SimulationData_Type mSimulationData;

};

#endif