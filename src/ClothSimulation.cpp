#include "ClothSimulation.h"

ClothSimulation::ClothSimulation()
{
    //Initialize cloth model object
    mClothModel = new ModelObject();

    GLuint shader = loadShaders("phong.vert", "phong.frag");
	mClothModel->setShader(shader,0);

	GLuint texture = 0;
	LoadTGATextureSimple((char *)"test.tga",&texture);
	mClothModel->setTexture(texture,0);

    Model* myModel = LoadModelPlus((char *)"stanford-bunny.obj");
    mClothModel->setModel(myModel);

    mat4 transform1 = S(20,20,20);
    mClothModel->setTransform(transform1);

    createGridOfMasses();
   // connectMassToSpring();

}

ClothSimulation::~ClothSimulation()
{
   mClothModel->~ModelObject();
}

void printVec3(char* head,vec3 v){
    printf("%s x: %f  y: %f z: %f \n",head, v.x , v.y ,v.z);
}

GLfloat ClothSimulation::getDeltaLength(vec3 v1,vec3 v2 ){
    vec3 deltaPos = v1 - v2;
    GLfloat length = sqrt(DotProduct(deltaPos,deltaPos));
    return length;
}

void ClothSimulation::createGridOfMasses(){

    Mass * currentMass;
    GLfloat xInit,yInit,zInit;

    for(int y = 0; y < CLOTH_DIM; ++y)
        for(int x = 0; x < CLOTH_DIM; ++x){
        currentMass = &mMasses[y][x];

        //Initial position

        GLfloat len = CLOTH_RES/2.0;
        // X/Y positions are centered around zero, then normalized, then scaled with CLOTH_SIZE_<AXIS>.
        xInit =  CLOTH_SIZE_X*(GLfloat)(x - len)/len;
        yInit =  CLOTH_SIZE_Y*(GLfloat)(y - len)/len;
        zInit =  0;
        currentMass->currentPosition = new vec3(xInit,yInit,zInit);
        currentMass->x = x;
        currentMass->y = y;
    }


}

/** \brief
 *  Creates a spring instance that is  connected to masses m1 and m2. Also
 *  assigned with rest length of the spring.
 * \param m1 Mass* Pointer to a mass
 * \param m2 Mass* Pointer to a mass
 * \return Spring * Pointer to a spring now connected to masses m1 and m2
 *
 */
Spring * ClothSimulation::createSpring(Mass* m1, Mass* m2){
    Spring *spring = new Spring();

    //Connecting end points to masses
    spring->mass1 = m1;
    spring->mass2 = m2;

    //Computing the initial rest length of the spring
    GLfloat length = getDeltaLength(*(m1->currentPosition),*(m2->currentPosition));
    spring->currSpringLength = length;
    spring->restSpringLength = length;
    return spring;

}

void ClothSimulation::connectMassToSpring(){
    for(int y = 0; y < CLOTH_DIM; ++y)
        for(int x = 0; x < CLOTH_DIM; ++x){

        if(x < CLOTH_DIM - 1){
            //Right horizontal structural spring
            mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y][x+1]));
        }

        if(x < CLOTH_DIM - 2){
            //Right horizontal bend spring
            mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y][x+2]));
        }

        if(y < CLOTH_DIM - 1) {
            //upper vertical structural spring
            mSprings.push_back(createSpring(&mMasses[y+1][x],&mMasses[y][x]));
        }
        if(y < CLOTH_DIM - 2) {
            //upper vertical bend spring
            mSprings.push_back(createSpring(&mMasses[y+2][x],&mMasses[y][x]));
        }

        if( y < CLOTH_DIM - 1 && x < CLOTH_DIM - 1){
            //Right upper shear spring
            mSprings.push_back(createSpring(&mMasses[y+1][x],&mMasses[y][x+1]));
        }
        if( y < 1 && x < CLOTH_DIM - 1){
            //Right lower shear spring
            mSprings.push_back(createSpring(&mMasses[y-1][x],&mMasses[y][x+1]));
        }
    }
}



void ClothSimulation::draw(mat4 projectionMatrix, mat4 viewMatrix){
    mClothModel->draw(projectionMatrix, viewMatrix);
}
