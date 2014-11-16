#ifndef CPUCLOTHSIMULATION_H
#define CPUCLOTHSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadTGA.h"

#include <math.h>
#include <vector>

#define CLOTH_RES 64
#define CLOTH_DIM (CLOTH_RES+1)
#define CLOTH_SIZE_X 4.0f
#define CLOTH_SIZE_Y 4.0f

#define FIX_POINT_1_X_START  0
#define FIX_POINT_1_X_END    4
#define FIX_POINT_1_Y_START  CLOTH_RES-4
#define FIX_POINT_1_Y_END    CLOTH_RES

#define FIX_POINT_2_X_START CLOTH_RES-4
#define FIX_POINT_2_X_END   CLOTH_RES
#define FIX_POINT_2_Y_START CLOTH_RES-4
#define FIX_POINT_2_Y_END   CLOTH_RES

#define SPRING_DAMPING -.65f
#define SPRING_CONSTANT_STRUCTURAL   50.75f
#define SPRING_CONSTANT_BEND         50.25f
#define SPRING_BREAK_FACTOR           3.2f

#define SYSTEM_DAMPING               -0.0125f
#define SYSTEN_DELTA_TIME            4.0f/60.0f

#define FLOATS_PER_VERTEX 3
#define FLOATS_PER_TEXEL 4
#define VERTICES_PER_TRIANGLE 3

/** Model/Shader Id's **/
#define CLOTH_SHADER_ID  0
#define SPHERE_SHADER_ID 1

typedef struct{
    vec3 *position;
    float radius;
}Sphere;

typedef struct{

    vec3  *previousPosition;
    vec3  *currentPosition;
    vec3  *accelearation;
    //Index
    int x,y;
}Mass;

typedef struct Spring{
    Mass* mass1;
    Mass* mass2;
    float restLength;
    float length;
    float springConstant;
    float dampConstant;
}Spring;


class CPUClothSimulation
{
    public:
        CPUClothSimulation();
        virtual ~CPUClothSimulation();

        void draw(mat4 projectionMatrix, mat4 viewMatrix);
        void update();
        void updateSpherePosition(vec3 deltaPos);
    protected:
    private:

        void createGridOfMasses();
        void connectMassToSpring();

        Spring* createSpring(Mass* m1, Mass* m2);
        GLfloat getDeltaLength(vec3 v1,vec3 v2 );

        Spring*createSpring(Mass* m1, Mass* m2, float springConstant,float dampConstant);

        //Physics - Data containers
        Mass mMasses[CLOTH_DIM][CLOTH_DIM];
        std::vector<Spring*> mSprings;
        std::vector<Sphere*> mSpheres;

        GLubyte mSpringMap[CLOTH_DIM][CLOTH_DIM];
        #define SPRING_STRUCT_RIGHT         1
        #define SPRING_STRUCT_UP            2
        #define SPRING_SHEAR_UP_RIGHT       4
        #define SPRING_SHEAR_DOWN_RIGHT     8
        #define SPRING_BEND_RIGHT           16
        #define SPRING_BEND_UP              32



        //Physics - Constants
        static const double dt = SYSTEN_DELTA_TIME;
        static const double SpringConstant     =  SPRING_CONSTANT_STRUCTURAL;
        static const double SpringConstantBend =  SPRING_CONSTANT_BEND;
        static const double SpringDamping =       SPRING_DAMPING;
        static const float VelocityDamping =      SYSTEM_DAMPING;
        vec3 * Gravity;

        //Physics - Methods
        void applyForces();
        void integrate();
        void checkCollision();
        bool isFixPoint(int x, int y);
        vec3 getVerletVelocity(vec3 curVel, vec3 prevVel);
        GLfloat previousTime;


        // Graphics
        void uploadBufferCoordinates();
        void uploadNewPositionBuffer();
        void uploadNewSpringBuffer();
        void generateFrameBuffers();
        bool isSpringChanged;
        FBOstruct *mFboMassPosition;
        FBOstruct *mFboActiveSprings;
        ModelObject * mCPUClothScene;
};

#endif // CLOTHSIMULATION_H

