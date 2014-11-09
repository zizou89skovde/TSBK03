#ifndef CLOTHSIMULATION_H
#define CLOTHSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadTGA.h"

#include <math.h>
#include <vector>

#define CLOTH_RES 4
#define CLOTH_DIM (CLOTH_RES+1)
#define CLOTH_SIZE_X 4
#define CLOTH_SIZE_Y 4

#define FIX_POINT_1_X 0
#define FIX_POINT_1_Y 0

#define FIX_POINT_2_X CLOTH_RES
#define FIX_POINT_2_Y 0


#define FLOATS_PER_VERTEX 3
#define FLOATS_PER_TEXEL 2
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
    protected:
    private:
        //Initialize Masses and Springs
        void createGridOfMasses();
        void connectMassToSpring();
        void checkSprings();

        GLfloat getDeltaLength(vec3 v1,vec3 v2 );
        Spring*createSpring(Mass* m1, Mass* m2, float springConstant,float dampConstant);

        //Physics - Constants
        static const double dt = 0.016;
        static const double SpringConstant =  50.75f;
        static const double SpringDamping = -0.25f;
        static const float VelocityDamping =  -0.0125f;
        vec3 * Gravity;

        //Physics - Methods
        void update();
        void applyForces();
        void integrate();
        bool isFixPoint(int x, int y);
        vec3 getVerletVelocity(vec3 curVel, vec3 prevVel);
        // Graphics
       void uploadInitialModelData();
       void uploadNewData();

        Mass mMasses[CLOTH_DIM][CLOTH_DIM];
        std::vector<Spring*> mSprings;


        ModelObject * mClothModel;
};

#endif // CLOTHSIMULATION_H
