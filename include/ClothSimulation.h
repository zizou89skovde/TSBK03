#ifndef CLOTHSIMULATION_H
#define CLOTHSIMULATION_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadTGA.h"

#include <math.h>
#include <vector>

#define CLOTH_RES 4
#define CLOTH_DIM CLOTH_RES+1
#define CLOTH_SIZE_X 4
#define CLOTH_SIZE_Y 4

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
    float restSpringLength;
    float currSpringLength;
}Spring;


class ClothSimulation
{
    public:
        ClothSimulation();
        virtual ~ClothSimulation();

        void draw(mat4 projectionMatrix, mat4 viewMatrix);
    protected:
    private:

        void createGridOfMasses();
        void connectMassToSpring();

        Spring* createSpring(Mass* m1, Mass* m2);
        GLfloat getDeltaLength(vec3 v1,vec3 v2 );

        Mass mMasses[CLOTH_DIM][CLOTH_DIM];
        std::vector<Spring*> mSprings;


        ModelObject * mClothModel;
};

#endif // CLOTHSIMULATION_H
