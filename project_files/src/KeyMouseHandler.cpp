#include "KeyMouseHandler.h"
#include <stdio.h>

KeyMouseHandler::KeyMouseHandler()
{
/*
    cameraEye = new vec3(0.0f, 8.0f, -5.0f);
    cameraCenter = new vec3(0.0f, 4.0f, -4.0f);
*/
    cameraEye = new vec3(0.0f, 0.0f, 0.0f);
    cameraCenter = new vec3(0.0f, 0.0f, 1.0f);
    cameraUp = new vec3(0.0f, 1.0f, 0.0f);

    mCelShading = NULL;

    xPrev = -1;
    yPrev = -1;
}

KeyMouseHandler::~KeyMouseHandler()
{
    //dtor
}


void KeyMouseHandler::keyPress(unsigned char key, int x, int y)
{
    float speed = 0.4f;

    vec3 lk = lookAtVec();
    vec3 right = rightVec();
    vec3 up    = upVec();
    float sphereSpeed = 0.02f;
    vec3 sphereDeltaPos = vec3(0,0,0);
    switch (key)
    {
        case 'w':
            *cameraEye += speed*lk;
            *cameraCenter += speed*lk;
        break;
        case 's':
            *cameraEye -= speed*lk;
            *cameraCenter -= speed*lk;
        break;
        case 'd':
            *cameraEye += speed*right;
            *cameraCenter += speed*right;
        break;
        case 'a':
            *cameraEye -= speed*right;
            *cameraCenter -= speed*right;
        break;

        case 'W':
            sphereDeltaPos = sphereSpeed*lk;
        break;
        case 'S':
            sphereDeltaPos = -sphereSpeed*lk;
        break;
        case 'D':
            sphereDeltaPos = sphereSpeed*right;
        break;
        case 'A':
             sphereDeltaPos = -sphereSpeed*right;
        break;
        case 'Z':
            sphereDeltaPos = sphereSpeed*up;
        break;
        case 'X':
            sphereDeltaPos = -sphereSpeed*up;
        break;
        case 'p':
        case 'P':
            if (mCelShading != NULL) {
                mCelShading->switchCelShading();
            }
        break;
        case 'o':
        case 'O':
            if (mCelShading != NULL) {
                mCelShading->increaseSteps();
            }
        break;
        case 'l':
        case 'L':
            if (mCelShading != NULL) {
                mCelShading->decreaseSteps();
            }
        break;
        case 'i':
        case 'I':
            if (mCelShading != NULL) {
                mCelShading->increaseContourThresh();
            }
        break;
        case 'k':
        case 'K':
            if (mCelShading != NULL) {
                mCelShading->decreaseContourThresh();
            }
        break;
        case 'u':
        case 'U':
            if (mCelShading != NULL) {
                mCelShading->fireObject();
            }
        break;
        case 'j':
        case 'J':
            if (mCelShading != NULL) {
                mCelShading->changeSpeedlines();
            }
        break;
        case 'm':
        case 'M':
            if (mCelShading != NULL) {
                mCelShading->changeGravity();
            }
        break;
    }
    if(DotProduct(sphereDeltaPos,sphereDeltaPos) != 0){
        if (mClothSimulation != NULL)
            mClothSimulation->updateSpherePosition(sphereDeltaPos);
    }

}
void print2Vec3(char* head,vec3 v){
    printf("%s x: %f  y: %f z: %f \n",head, v.x , v.y ,v.z);
}
void KeyMouseHandler::mouseHandle(int x, int y)
{
    if (xPrev > 0 && yPrev > 0) {
        GLfloat mouseFactor = 0.01f;
        float dx = mouseFactor * (x - xPrev);
        float dy = mouseFactor * (y - yPrev);

        *cameraCenter = *cameraEye + (lookAtVec() + dx * rightVec() - dy * upVec());
    }
    xPrev = x;
    yPrev = y;
   // print2Vec3((char*)"CAM EYE: ", *cameraEye);
   // print2Vec3((char*)"CAM CENTER: ", *cameraCenter);
}

mat4 KeyMouseHandler::getViewMatrix()
{
	if(mPostProcessing != NULL)
	    mPostProcessing->setCameraInfo(cameraEye,cameraCenter);

    return lookAt(cameraEye->x, cameraEye->y, cameraEye->z,
                  cameraCenter->x, cameraCenter->y, cameraCenter->z,
                  cameraUp->x, cameraUp->y, cameraUp->z);
}

void KeyMouseHandler::mouseUp()
{
    xPrev = -1;
    yPrev = -1;
}

vec3 KeyMouseHandler::lookAtVec()
{
    return Normalize(*cameraCenter - *cameraEye);
}

vec3 KeyMouseHandler::rightVec()
{
    return Normalize(CrossProduct(lookAtVec(), *cameraUp));
}

vec3 KeyMouseHandler::upVec()
{
    return CrossProduct(rightVec(), lookAtVec());
}
