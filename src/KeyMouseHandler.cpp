#include "KeyMouseHandler.h"
#include <stdio.h>

KeyMouseHandler::KeyMouseHandler()
{
    cameraEye = new vec3(5.0f, 20.0f, 5.0f);
    cameraCenter = new vec3(1.0f, 0.0f, 0.0f);
    *cameraCenter += *cameraEye;
    cameraUp = new vec3(0.0f, 1.0f, 0.0f);

    xPrev = -1;
    yPrev = -1;
}

KeyMouseHandler::~KeyMouseHandler()
{
    //dtor
}


void KeyMouseHandler::keyPress(unsigned char key, int x, int y)
{
    float speed = 2.0f;

    vec3 lk = lookAtVec();
    vec3 right = rightVec();

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
    }

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
}

mat4 KeyMouseHandler::getViewMatrix()
{
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
