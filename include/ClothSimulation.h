#ifndef CLOTHSIMULATION_H
#define CLOTHSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadTGA.h"

#include <math.h>
#include <vector>

#define CLOTH_RES 16
#define CLOTH_DIM (CLOTH_RES+1)
#define CLOTH_SIZE_X 4.0f
#define CLOTH_SIZE_Y 4.0f

#define FIX_POINT_1_X 0
#define FIX_POINT_1_Y CLOTH_RES

#define FIX_POINT_2_X CLOTH_RES
#define FIX_POINT_2_Y CLOTH_RES


#define FLOATS_PER_VERTEX 3
#define FLOATS_PER_TEXEL 4
#define VERTICES_PER_TRIANGLE 3
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


class ClothSimulation
{
    public:
        ClothSimulation();
        virtual ~ClothSimulation();

        void draw(mat4 projectionMatrix, mat4 viewMatrix);
            void update();
    protected:
    private:
        //Initialize Masses and Springs
        void createGridOfMasses();
        void connectMassToSpring();
        void checkSprings();

        GLfloat getDeltaLength(vec3 v1,vec3 v2 );
        Spring*createSpring(Mass* m1, Mass* m2, float springConstant,float dampConstant);

        //Physics - Data containers
        Mass mMasses[CLOTH_DIM][CLOTH_DIM];
        std::vector<Spring*> mSprings;

        //Physics - Constants
        static const double dt = 1.0f/60.0f;
        static const double SpringConstant =  50.75f;
        static const double SpringDamping = -0.25f;
        static const float VelocityDamping =  -0.0525f;
        vec3 * Gravity;

        //Physics - Methods
        void applyForces();
        void integrate();
        bool isFixPoint(int x, int y);
        vec3 getVerletVelocity(vec3 curVel, vec3 prevVel);
        GLfloat previousTime;

        // Graphics
        void uploadInitialModelData();
        void uploadNewData();
        void generateFrameBuffer();
        FBOstruct *fboCloth;
        ModelObject * mClothModel;




};

#endif // CLOTHSIMULATION_H
