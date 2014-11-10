#include "ClothSimulation.h"

ClothSimulation::ClothSimulation()
{

    //Physics create grid
    createGridOfMasses();
    connectMassToSpring();


    //Initialize cloth model object
    mClothModel = new ModelObject();


    // Load model data
    uploadInitialModelData();
    //Generate Cloth FBO, used in vertex shader for vertex displacement and also calculating normals.
    generateFrameBuffer();
    mClothModel->setTexture(fboCloth->texid,0);
  //  uploadNewData();

    // Load shader
    GLuint shader = loadShaders("cloth.vert", "cloth.frag");
	mClothModel->setShader(shader,0);

    // Set initial transform
    mat4 transform1 = S(1,1,1);
    mClothModel->setTransform(transform1);

    //Assign default values to some variables
    previousTime = -1;
    Gravity = new vec3(0,-0.982,0.0);


}

ClothSimulation::~ClothSimulation()
{
   mClothModel->~ModelObject();
}

void printVec3(char* head,vec3 v){
    printf("%s x: %f  y: %f z: %f \n",head, v.x , v.y ,v.z);
}
/** \brief
 * Computes distance between two 3d points
 * \param v1 vec3 3d point
 * \param v2 vec3 3d point
 * \return GLfloat distance between two 3d points
 *
 */

GLfloat ClothSimulation::getDeltaLength(vec3 v1,vec3 v2 ){
    vec3 deltaPos = v1 - v2;
    GLfloat length = sqrt(DotProduct(deltaPos,deltaPos));
    return length;
}
/** \brief Creates a grid of masses. The masses reside in mMasses, which is a 2D array
 * of Mass data type. The size of the grid is define in ClothSimulation.h. See CLOTH_DIM.
 *
 * \return void
 *
 */
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
        yInit =  8;//CLOTH_SIZE_Y*(GLfloat)(y - len)/len;
        zInit =  CLOTH_SIZE_Y*(GLfloat)(y - len)/len;//8;
        currentMass->currentPosition =  new vec3(xInit,yInit,zInit);
        currentMass->previousPosition = new vec3(xInit,yInit,zInit);
        currentMass->accelearation = new vec3(0,0,0);
        currentMass->x = x;
        currentMass->y = y;

    }


}
/****************************************************************************/
/****************************************************************************/
/* Initializing  Masses and Springs */
/****************************************************************************/
/****************************************************************************/
/** \brief
 *  Creates a spring instance that is  connected to masses m1 and m2. Also
 *  assigned with rest length of the spring.
 * \param m1 Mass* Pointer to a mass
 * \param m2 Mass* Pointer to a mass
 * \return Spring * Pointer to a spring now connected to masses m1 and m2
 *
 */
Spring * ClothSimulation::createSpring(Mass* m1, Mass* m2, float springConstant,float dampConstant){
    Spring *spring = new Spring();

    //Connecting end points to masses
    spring->mass1 = m1;
    spring->mass2 = m2;

    //Computing the initial rest length of the spring
    GLfloat length      = getDeltaLength(*m1->currentPosition,*m2->currentPosition);
    spring->length      = length;
    spring->restLength  = length;

    spring->springConstant    = springConstant;
    spring->dampConstant    = dampConstant;
    return spring;

}
/** \brief Creates spring instances which is connecting masses.
 * There are 3 type of springs - structural, bend and shear springs.
 * \return void
 */
void ClothSimulation::connectMassToSpring(){
    for(int y = 0; y < CLOTH_DIM; ++y)
        for(int x = 0; x < CLOTH_DIM; ++x){

        if(x < CLOTH_DIM - 1){
            //Right horizontal structural spring
              mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y][x+1],SpringConstant,SpringDamping));
        }

        if(x < CLOTH_DIM - 2){
            //Right horizontal bend spring
             mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y][x+2],SpringConstant,SpringDamping));
        }

        if(y < CLOTH_DIM - 1) {
            //upper vertical structural spring
            mSprings.push_back(createSpring(&mMasses[y+1][x],&mMasses[y][x],SpringConstant,SpringDamping));
        }
        if(y < CLOTH_DIM - 2) {
            //upper vertical bend spring
            mSprings.push_back(createSpring(&mMasses[y+2][x],&mMasses[y][x],SpringConstant,SpringDamping));
        }

        if( y < CLOTH_DIM - 1 && x < CLOTH_DIM - 1){
            //Right upper shear spring
            mSprings.push_back(createSpring(&mMasses[y+1][x],&mMasses[y][x+1],SpringConstant,SpringDamping));
        }
        if( y > 0 && x < CLOTH_DIM - 1){
            //Right lower shear spring
            mSprings.push_back(createSpring(&mMasses[y-1][x],&mMasses[y][x+1],SpringConstant,SpringDamping));
        }
    }
}


/****************************************************************************/
/****************************************************************************/
/*Cloth Graphics*/
/****************************************************************************/
/****************************************************************************/

void ClothSimulation::uploadInitialModelData(){
    int vertexCount = CLOTH_DIM*CLOTH_DIM;
	int triangleCount = (CLOTH_DIM-1) * (CLOTH_DIM-1)* 2;

    GLfloat vertexArray[FLOATS_PER_VERTEX * vertexCount];
	GLfloat *normalArray = (GLfloat *) malloc(sizeof(GLfloat) * FLOATS_PER_VERTEX* vertexCount);
	GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount*VERTICES_PER_TRIANGLE);

     for(GLuint y = 0; y < CLOTH_DIM; ++y)
        for(GLuint x = 0; x < CLOTH_DIM; ++x){

            float xPos = x/(float)CLOTH_DIM;
            float yPos = y/(float)CLOTH_DIM;
            vertexArray[(x + y * CLOTH_DIM)*3 + 0] = xPos;
            vertexArray[(x + y * CLOTH_DIM)*3 + 1] = yPos;
            vertexArray[(x + y * CLOTH_DIM)*3 + 2] = 0;
            //printf("X: %f Y: %f \n",xPos,yPos);
            normalArray[(x + y * CLOTH_DIM)*3 + 0] = 0;
            normalArray[(x + y * CLOTH_DIM)*3 + 1] = 0;
            normalArray[(x + y * CLOTH_DIM)*3 + 2] = 1;

        }

	 for (GLuint y = 0; y < CLOTH_DIM-1; y++)
        for (GLuint x = 0; x < CLOTH_DIM-1; x++)
		{
		// Triangle 1
			indexArray[(x + y * (CLOTH_DIM-1))*6 + 0] = x + y * CLOTH_DIM;
			indexArray[(x + y * (CLOTH_DIM-1))*6 + 1] = x + (y+1) * CLOTH_DIM;
			indexArray[(x + y * (CLOTH_DIM-1))*6 + 2] = x+1 + y * CLOTH_DIM;
		// Triangle 2
			indexArray[(x + y * (CLOTH_DIM-1))*6 + 3] = x+1 + y * CLOTH_DIM;
			indexArray[(x + y * (CLOTH_DIM-1))*6 + 4] = x + (y+1) * CLOTH_DIM;
			indexArray[(x + y * (CLOTH_DIM-1))*6 + 5] = x+1 + (y+1) * CLOTH_DIM;
		}
        mClothModel->LoadDataToModel(
			vertexArray,
			normalArray,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);
}
void ClothSimulation::draw(mat4 projectionMatrix, mat4 viewMatrix){
    mClothModel->draw(projectionMatrix, viewMatrix);
}
void ClothSimulation::uploadNewData(){
  /*  int vertexCount = CLOTH_DIM*CLOTH_DIM;
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * FLOATS_PER_VERTEX * vertexCount);
   // GLfloat *normalArray = (GLfloat *) malloc(sizeof(GLfloat) * FLOATS_PER_VERTEX* vertexCount);
    for(GLuint y = 0; y < CLOTH_DIM; ++y)
    for(GLuint x = 0; x < CLOTH_DIM; ++x){
        Mass mass = mMasses[y][x];
        vertexArray[(x + y * CLOTH_DIM)*3 + 0] = mass.currentPosition->x;
        vertexArray[(x + y * CLOTH_DIM)*3 + 1] = mass.currentPosition->y;
        vertexArray[(x + y * CLOTH_DIM)*3 + 2] = mass.currentPosition->z;
    }
    size_t buffSize = 4*3*vertexCount;
    mClothModel->uploadNewVertexData(vertexArray,buffSize);*/
    int vertexCount = CLOTH_DIM*CLOTH_DIM;
    printError(" pre Cloth FBO");

    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (FLOATS_PER_TEXEL) * vertexCount);
    for(GLuint y = 0; y < CLOTH_DIM; ++y)
    for(GLuint x = 0; x < CLOTH_DIM; ++x){
        Mass mass = mMasses[y][x];
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 0] = mass.currentPosition->x;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 1] = mass.currentPosition->y;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL+ 2]  = mass.currentPosition->z;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 3] = 1;
    }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, CLOTH_DIM,CLOTH_DIM, 0, GL_RGBA, GL_FLOAT, vertexArray);
	free(vertexArray);

    glBindTexture(GL_TEXTURE_2D, fboCloth->texid);

}
 void ClothSimulation::generateFrameBuffer(){
    //Create fbo struct
    fboCloth = (FBOstruct*)malloc(sizeof(FBOstruct));

    //Set set dimensions
	fboCloth->width = CLOTH_DIM;
	fboCloth->height = CLOTH_DIM;

	//Generate frame buffer and texture
    glGenFramebuffers(1, &fboCloth->fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCloth->fb);
	glGenTextures(1, &fboCloth->texid);
	glBindTexture(GL_TEXTURE_2D, fboCloth->texid);

	//Select clamp and nearest configuration
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    int vertexCount = CLOTH_DIM*CLOTH_DIM;
    printError(" pre Cloth FBO");

    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (FLOATS_PER_TEXEL) * vertexCount);
    for(GLuint y = 0; y < CLOTH_DIM; ++y)
    for(GLuint x = 0; x < CLOTH_DIM; ++x){
        Mass mass = mMasses[y][x];
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 0] = mass.currentPosition->x;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 1] = mass.currentPosition->y;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL+ 2] = mass.currentPosition->z;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 3] = 1;
    }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, CLOTH_DIM,CLOTH_DIM, 0, GL_RGBA, GL_FLOAT, vertexArray);
	free(vertexArray);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    printError("Cloth FBO");
}

/****************************************************************************/
/****************************************************************************/
/*Cloth physics */
/****************************************************************************/
/****************************************************************************/
/**
* Check if current mass is a fix point. That not should be affected by any force.
* There are two fix point which up hold the rest of the cloth.
**/
void ClothSimulation::update(){


    if(previousTime < 0){
        previousTime = glutGet(GLUT_ELAPSED_TIME);
        return;
    }

    GLfloat newTime = (GLfloat) glutGet(GLUT_ELAPSED_TIME);
    GLfloat elapsedTime = newTime-previousTime;


    GLuint numIterations = (GLuint)(elapsedTime/(dt*500.0f));
    if(numIterations > 0){
        previousTime = newTime;
        applyForces();
        integrate();
        uploadNewData();
    }
}
/**
* Integrate position of each mass using verlet integration method:
* X(n+1) = 2*X(n) - X(n-1) * A(x)*dt^2
**/
void ClothSimulation::integrate(){
    for(int y = 0; y < CLOTH_DIM; ++y)
        for(int x = 0; x < CLOTH_DIM; ++x){

            vec3 pos = *mMasses[y][x].currentPosition;
            vec3 prevPos = *mMasses[y][x].previousPosition;
            vec3 acceleration = *mMasses[y][x].accelearation;

            //Store previous position
            *mMasses[y][x].previousPosition = pos;

            //Update to new position if current mass are not fix
            if(!isFixPoint(x,y)){
                *mMasses[y][x].currentPosition = 2*pos -prevPos + acceleration*dt*dt;
            }
    }
}
bool ClothSimulation::isFixPoint(int x, int y){
    return  ( x == FIX_POINT_1_X && y == FIX_POINT_1_Y ) ||
            ( x == FIX_POINT_2_X && y == FIX_POINT_2_Y );

}
/**
* Computing Velocity using verlet method:
* v(n) = (p(n)-p(n-1))/dt
*/
vec3 ClothSimulation::getVerletVelocity(vec3 curVel, vec3 prevVel){
    return (curVel-prevVel)/dt;
}
/**
* Updating acceleration of each mass by computing force from gravity,springs and some nameless damping.
*
*/
void ClothSimulation::applyForces(){
     //Iterate over all masses in order to apply gravity and a damp force on each and every mass.
     for(int y = 0; y < CLOTH_DIM; ++y)
        for(int x = 0; x < CLOTH_DIM; ++x){
            // Reset acceleration
            mMasses[y][x].accelearation = new vec3(0,0,0);

            // Apply gravity force
            *mMasses[y][x].accelearation += *Gravity;

            // Apply damping force in regard to the current  velocity.
            vec3 velocity = getVerletVelocity(*mMasses[y][x].currentPosition,*mMasses[y][x].previousPosition);
            *mMasses[y][x].accelearation += velocity*VelocityDamping;
        }

    Spring * spring;
    for(std::vector<Spring*>::iterator it = mSprings.begin(); it != mSprings.end(); ++it) {

        spring = *it;
        Mass * mass1 = spring->mass1;
        Mass * mass2 = spring->mass2;

        // Reading current and previous position
        vec3 p1     = *mass1->currentPosition;
        vec3 p1Prev = *mass1->previousPosition;
        vec3 p2     = *mass2->currentPosition;
        vec3 p2Prev = *mass2->previousPosition;

        // Computing velocity, using Verlet method, for the two masses.
        vec3 v1 = getVerletVelocity(p1,p1Prev);
        vec3 v2 = getVerletVelocity(p2,p2Prev);

        //
        vec3 deltaPos = p1-p2;
        vec3 deltaVel = v1-v2;

        double springLength = getDeltaLength(p1,p2);

        // Determine the spring and damp force for each spring
        float springForce = -spring->springConstant *(springLength - spring->restLength);
        float dampForce   = spring->dampConstant *(DotProduct(deltaPos,deltaVel)/springLength);

        vec3 force = Normalize(deltaPos)*(springForce+dampForce);

        // Add force from spring
        *mass1->accelearation += force;
        *mass2->accelearation -= force;

    }
}

