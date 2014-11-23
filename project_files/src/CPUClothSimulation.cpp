#include "CPUClothSimulation.h"

CPUClothSimulation::CPUClothSimulation(): SimulationClass()
{


    /** Set all elements in spring map to zero. Important(!) **/
    memset(mSpringMap,0,CLOTH_DIM*CLOTH_DIM*sizeof(GLubyte));

    /** Physics create grid **/
    createGridOfMasses();
    connectMassToSpring();


    /** Initialize cloth model object **/
    mCPUClothScene = new ModelObject();

    /**  Load buffer (texture) coordinates **/
    uploadBufferCoordinates();

    /** Generate Cloth FBO, used in vertex shader for vertex displacement and also calculating normals. **/
    generateFrameBuffers();

    /** Load shader **/
    GLuint clothShader = loadShadersG("shaders/cloth.vert","shaders/cloth.frag","shaders/cloth.gs");
	mCPUClothScene->setShader(clothShader,CLOTH_SHADER_ID,VP);

    /**  Assign texture handles to the model object **/
    mCPUClothScene->setTexture(mFboMassPosition->texid,CLOTH_SHADER_ID,(const char *)"u_MassPos_Tex");
    mCPUClothScene->setTexture(mFboActiveSprings->texid,CLOTH_SHADER_ID,(const char *)"u_SpringMap_Tex");

	GLfloat * resolution = (GLfloat*)malloc(sizeof(GLfloat)*2);
	resolution[0] = CLOTH_RES;
	resolution[1] = CLOTH_SIZE_X;
    mCPUClothScene->setUniform(resolution,2,0,(const char*) "u_Resolution");

    /** Set initial transform **/
    mat4 transform1 = IdentityMatrix();
    mCPUClothScene->setTransform(transform1,CLOTH_SHADER_ID);

    /** Assign default values to some variables **/
    previousTime = -1;
    Gravity = new vec3(0,-0.00981,0.0);
    isSpringChanged = false;

    /** Sphere - For collision **/
    Sphere * sphere = new Sphere();
    sphere->position = new vec3(0,0,-4);
    sphere->radius = 1.1;
    mSpheres.push_back(sphere);

    /** Sphere Model **/
    Model* modelSphere = LoadModelPlus((char *)"models/sphere.obj");
    mCPUClothScene->setModel(modelSphere,SPHERE_SHADER_ID);

    /** Sphere Shader **/
    GLuint sphereShader = loadShaders("shaders/sphere.vert", "shaders/sphere.frag");
    mCPUClothScene->setShader(sphereShader,SPHERE_SHADER_ID);

    /** Sphere Transform **/
    GLfloat r = sphere->radius;
    vec3  pos = *sphere->position;
    mat4 transform2 = T(pos.x,pos.y,pos.z)*S(r,r,r);

    mCPUClothScene->setTransform(transform2,SPHERE_SHADER_ID);

}

CPUClothSimulation::~CPUClothSimulation()
{
    /** Delete model objects **/
    mCPUClothScene->~ModelObject();

    /** Delete mass grid **/
    Mass * currentMass;
    for(int y = 0; y < CLOTH_DIM; ++y)
        for(int x = 0; x < CLOTH_DIM; ++x){
        currentMass = &mMasses[y][x];
        free(currentMass->accelearation);
        free(currentMass->currentPosition);
        free(currentMass->previousPosition);
    }
    free(mMasses);
    free(mSpringMap);

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

GLfloat CPUClothSimulation::getDeltaLength(vec3 v1,vec3 v2 ){
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
void CPUClothSimulation::createGridOfMasses(){

    Mass * currentMass;
    GLfloat xInit,yInit,zInit;

    for(int y = 0; y < CLOTH_DIM; ++y)
        for(int x = 0; x < CLOTH_DIM; ++x){
        currentMass = &mMasses[y][x];

        //Initial position

        GLfloat len = CLOTH_RES/2.0;
        // X/Y positions are centered around zero, then normalized, then scaled with CLOTH_SIZE_<AXIS>.
        xInit =  CLOTH_SIZE_X*(GLfloat)(x - len)/len;
        yInit =  CLOTH_SIZE_Y*(GLfloat)(y - len)/len; //4;//CLOTH_SIZE_X/2.0f;//CLOTH_SIZE_Y*(GLfloat)(y - len)/len;
        zInit =  0; //CLOTH_SIZE_Y*(GLfloat)(y - len)/len;//8;
        currentMass->currentPosition =  new vec3(xInit,yInit,zInit);
        currentMass->previousPosition = new vec3(xInit,yInit,zInit);
        currentMass->accelearation = new vec3(0,0,0);
        currentMass->x = x;
        currentMass->y = y;

    }


}
/****************************************************************************/
/****************************************************************************/
/***************** Initializing  Masses and Springs **********************/
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
Spring * CPUClothSimulation::createSpring(Mass* m1, Mass* m2, float springConstant,float dampConstant){
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
void CPUClothSimulation::connectMassToSpring(){
    //int tearIndex_X = CLOTH_RES/2;
   // int tearIndex_Y = CLOTH_RES/2;
    for(int y = 0; y < CLOTH_DIM; ++y)
        for(int x = 0; x < CLOTH_DIM; ++x){


        if(y < CLOTH_DIM - 1) {
            //upper vertical structural spring
            mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y+1][x],SpringConstant,SpringDamping));

            //
            mSpringMap[y][x] |= SPRING_STRUCT_UP;

        }
        if(y < CLOTH_DIM - 2) {
            //upper vertical bend spring
            mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y+2][x],SpringConstantBend/*SpringConstant*/,SpringDamping));
            mSpringMap[y][x] |= SPRING_BEND_UP;
        }

        //if(x == tearIndex_X && y < tearIndex_Y)
        //    continue;

        if(x < CLOTH_DIM - 1){
            //Right horizontal structural spring
           // if(x+1 != tearIndex )
              mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y][x+1],SpringConstant,SpringDamping));
              mSpringMap[y][x] |= SPRING_STRUCT_RIGHT;
        }

        if(x < CLOTH_DIM - 2){
            //Right horizontal bend spring
           // if(x+1 != tearIndex_X && y < tearIndex_Y)
             mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y][x+2],SpringConstantBend/*SpringConstant*/,SpringDamping));
             mSpringMap[y][x] |= SPRING_BEND_RIGHT;
        }

        if( y < CLOTH_DIM - 1 && x < CLOTH_DIM - 1){
            //Right upper shear spring
           // if(x+1 != tearIndex )
            mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y+1][x+1],SpringConstant,SpringDamping));
              mSpringMap[y][x] |= SPRING_SHEAR_UP_RIGHT;
        }
        if( y > 0 && x < CLOTH_DIM - 1){
            //Right lower shear spring
           // if(x+1 != tearIndex )
                mSprings.push_back(createSpring(&mMasses[y][x],&mMasses[y-1][x+1],SpringConstant,SpringDamping));
                mSpringMap[y][x] |= SPRING_SHEAR_DOWN_RIGHT;
        }
    }
}


/****************************************************************************/
/****************************************************************************/
/************************* Cloth Graphics ***********************************/
/****************************************************************************/
/****************************************************************************/

void CPUClothSimulation::uploadBufferCoordinates(){
    int vertexCount = CLOTH_DIM*CLOTH_DIM;
	int triangleCount = (CLOTH_DIM-1) * (CLOTH_DIM-1)* 2;
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (FLOATS_PER_TEXEL) * vertexCount);
 //   GLfloat vertexArray[FLOATS_PER_VERTEX * vertexCount];
	GLfloat *normalArray = (GLfloat *) malloc(sizeof(GLfloat) * FLOATS_PER_VERTEX* vertexCount);
	GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount*VERTICES_PER_TRIANGLE);

     for(GLuint y = 0; y < CLOTH_DIM; ++y)
        for(GLuint x = 0; x < CLOTH_DIM; ++x){

            float xPos = x/(float)CLOTH_RES;
            float yPos = y/(float)CLOTH_RES;
            vertexArray[(x + y * CLOTH_DIM)*3 + 0] = xPos;
            vertexArray[(x + y * CLOTH_DIM)*3 + 1] = yPos;
            vertexArray[(x + y * CLOTH_DIM)*3 + 2] = 0;

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
        mCPUClothScene->LoadDataToModel(
			vertexArray,
			normalArray,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3
			,CLOTH_SHADER_ID);

}
void CPUClothSimulation::draw(mat4 projectionMatrix, mat4 viewMatrix){

    if(isSpringChanged){
        uploadNewSpringBuffer();
        isSpringChanged = false;
    }
    uploadNewPositionBuffer();
    mCPUClothScene->draw(projectionMatrix, viewMatrix);
}
void CPUClothSimulation::uploadNewPositionBuffer(){
    int vertexCount = CLOTH_DIM*CLOTH_DIM;
  //  printError(" pre Cloth FBO");
   // printf("\nNEW GRID: \n");
    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (FLOATS_PER_TEXEL) * vertexCount);
    for(GLuint y = 0; y < CLOTH_DIM; ++y)
        for(GLuint x = 0; x < CLOTH_DIM; ++x){
        Mass mass = mMasses[y][x];
      //  printVec3("MASS POSS", *mass.currentPosition);
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 0] = mass.currentPosition->x;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 1] = mass.currentPosition->y;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL+ 2]  = mass.currentPosition->z;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 3] = 1;
    }
    glBindTexture(GL_TEXTURE_2D, mFboMassPosition->texid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, CLOTH_DIM,CLOTH_DIM, 0, GL_RGBA, GL_FLOAT, vertexArray);
	free(vertexArray);



}
void CPUClothSimulation::uploadNewSpringBuffer(){
    int vertexCount = CLOTH_DIM*CLOTH_DIM;
    glBindTexture(GL_TEXTURE_2D, mFboActiveSprings->texid);
    GLfloat *springArray = (GLfloat *) malloc(sizeof(GLfloat) * vertexCount);
    for(GLuint y = 0; y < CLOTH_DIM; ++y)
        for(GLuint x = 0; x < CLOTH_DIM; ++x){
        GLfloat spring = mSpringMap[y][x];
        springArray[(x + y * CLOTH_DIM) + 0] = spring;

    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CLOTH_DIM,CLOTH_DIM, 0,GL_RED, GL_FLOAT, springArray); /*_UNSIGNED*/
	glBindTexture(GL_TEXTURE_2D, 0);

}
 void CPUClothSimulation::generateFrameBuffers(){

  /********************* Mass positions *******************/
    //Create fbo struct
    mFboMassPosition = (FBOstruct*)malloc(sizeof(FBOstruct));

    //Set set dimensions
	mFboMassPosition->width = CLOTH_DIM;
	mFboMassPosition->height = CLOTH_DIM;

	//Generate frame buffer and texture
    glGenFramebuffers(1, &mFboMassPosition->fb);
	glBindFramebuffer(GL_FRAMEBUFFER, mFboMassPosition->fb);
	glGenTextures(1, &mFboMassPosition->texid);
	glBindTexture(GL_TEXTURE_2D, mFboMassPosition->texid);

	//Select clamp and nearest configuration
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    int vertexCount = CLOTH_DIM*CLOTH_DIM;


    GLfloat *vertexArray = (GLfloat *) malloc(sizeof(GLfloat) * (FLOATS_PER_TEXEL) * vertexCount);
    for(GLuint y = 0; y < CLOTH_DIM; ++y)
        for(GLuint x = 0; x < CLOTH_DIM; ++x){
        Mass mass = mMasses[y][x];
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 0] = mass.currentPosition->x;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 1] = mass.currentPosition->y;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 2] = mass.currentPosition->z;
        vertexArray[(x + y * CLOTH_DIM)*FLOATS_PER_TEXEL + 3] = 1;
    }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, CLOTH_DIM,CLOTH_DIM, 0, GL_RGBA, GL_FLOAT, vertexArray);
	free(vertexArray);
printError("Cloth pre FBO SPRING");
  /********************* Spring Map *******************/
    //Create fbo struct
    mFboActiveSprings = (FBOstruct*)malloc(sizeof(FBOstruct));

    //Set set dimensions
	mFboActiveSprings->width = CLOTH_DIM;
	mFboActiveSprings->height = CLOTH_DIM;

	//Generate frame buffer and texture
    glGenFramebuffers(1, &mFboActiveSprings->fb);
	glBindFramebuffer(GL_FRAMEBUFFER, mFboActiveSprings->fb);
	glGenTextures(1, &mFboActiveSprings->texid);
	glBindTexture(GL_TEXTURE_2D, mFboActiveSprings->texid);
    printError("Cloth 0 FBO SPRING");
	//Select clamp and nearest configuration
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	printError("Cloth 0.5 FBO SPRING");

    GLfloat *springArray = (GLfloat *) malloc(sizeof(GLfloat) * vertexCount);
    for(GLuint y = 0; y < CLOTH_DIM; ++y)
        for(GLuint x = 0; x < CLOTH_DIM; ++x){
        GLfloat spring = mSpringMap[y][x];
        springArray[(x + y * CLOTH_DIM) + 0] = spring;

    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CLOTH_DIM,CLOTH_DIM, 0,GL_RED, GL_FLOAT, springArray); /*_UNSIGNED*/
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, CLOTH_DIM,CLOTH_DIM, 0,GL_RED_INTEGER, GL_BYTE, springArray); /*_UNSIGNED*/
    free(springArray);
    //Bind default/screen frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    printError("Cloth 1  FBO SPRING");
}

/****************************************************************************/
/****************************************************************************/
/**************************** Cloth physics *********************************/
/****************************************************************************/
/****************************************************************************/
/**
* Check if current mass is a fix point. That not should be affected by any force.
* There are two fix point which up hold the rest of the cloth.
**/
void CPUClothSimulation::update(){


    if(previousTime < 0){
        previousTime = glutGet(GLUT_ELAPSED_TIME);
        return;
    }

    GLfloat newTime = (GLfloat) glutGet(GLUT_ELAPSED_TIME);
    GLfloat elapsedTime = newTime-previousTime;


    GLuint numIterations = (GLuint)(elapsedTime/(dt*1.0f));
    if(numIterations > 0){
        previousTime = newTime;
        applyForces();
        integrate();
        checkCollision();

    }
}


/**
* Integrate position of each mass using Verlet integration method:
* X(n+1) = 2*X(n) - X(n-1) * A(x)*dt^2
**/
void CPUClothSimulation::integrate(){
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
bool CPUClothSimulation::isFixPoint(int x, int y){
    return  (( x > FIX_POINT_1_X_START && x < FIX_POINT_1_X_END) &&
            ( y > FIX_POINT_1_Y_START && y < FIX_POINT_1_Y_END)) ||
            (( x > FIX_POINT_2_X_START && x < FIX_POINT_2_X_END) &&
            ( y > FIX_POINT_2_Y_START && y < FIX_POINT_2_Y_END));

}
/**
* Computing Velocity using Verlet method:
* v(n) = (p(n)-p(n-1))/dt
*/
vec3 CPUClothSimulation::getVerletVelocity(vec3 curVel, vec3 prevVel){
    return (curVel-prevVel)/dt;
}
/**
* Updating acceleration of each mass by computing force from gravity,springs and some nameless damping.
*/
void CPUClothSimulation::applyForces(){
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
       // printf("MASS1 INDEX: x:%d y:%d +\n",mass1->x,mass1->y);
       // printf("MASS2 INDEX: x:%d y:%d +\n",mass2->x,mass2->y);
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

        if(springLength > SPRING_BREAK_FACTOR*spring->restLength ){
            int x = mass1->x;
            int y = mass1->y;
            mSpringMap[y][x] = 34;
            mSprings.erase(it);
            isSpringChanged = true;
            continue;


        }
        // Determine the spring and damp force for each spring
        float springForce = -spring->springConstant *(springLength - spring->restLength);
        float dampForce   = spring->dampConstant *(DotProduct(deltaPos,deltaVel)/springLength);

        vec3 force = Normalize(deltaPos)*(springForce+dampForce);

        // Add force from spring
        *mass1->accelearation += force;
        *mass2->accelearation -= force;

    }
}
/**
*
*Check collision with sphere
*/
void CPUClothSimulation::checkCollision(){
    GLfloat stepSize = 0.2;
    Sphere* sphere;
     for(std::vector<Sphere*>::iterator it = mSpheres.begin(); it != mSpheres.end(); ++it) {
        //printf("\nNEW GRID: \n");
        sphere = *it;
        vec3 sphereCenter = *sphere->position;
        GLfloat sphereRadius = sphere->radius;
        for(int y = 0; y < CLOTH_DIM; ++y)
            for(int x = 0; x < CLOTH_DIM; ++x){

                //Determine distance from mass to the center of the sphere
                vec3 massPos = *mMasses[y][x].currentPosition;
                GLfloat distance = getDeltaLength(massPos,sphereCenter);
            //    printVec3("MASS POSS",massPos);
                //Check if a mass inside the sphere
                if(distance < 1.0){
                    //If mass is inside of a sphere. Move the mass to the surface of
                    // there sphere with a small step
                    vec3 adjustmentStep = massPos-sphereCenter;
                    adjustmentStep = stepSize*(sphereRadius-distance)*Normalize(adjustmentStep);

                    *mMasses[y][x].currentPosition += adjustmentStep;
                    *mMasses[y][x].previousPosition = *mMasses[y][x].currentPosition;
                }

            }
    }
}

void CPUClothSimulation::updateSpherePosition(vec3 deltaPos){
    //Update position of sphere
    Sphere * s = mSpheres.at(0);
    *s->position += deltaPos;

    //Update model transform
    mat4 * transf = mCPUClothScene->getTransform(SPHERE_SHADER_ID);
   // GLfloat r = s->radius;
    vec3  pos = *s->position;
     //printVec3("New Sphere pos:",pos);
    *transf = T(pos.x,pos.y,pos.z); //*S(r,r,r);

}
