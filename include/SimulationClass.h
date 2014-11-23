#ifndef SIMULATIONCLASS_H
#define SIMULATIONCLASS_H
#include "VectorUtils3.h"

class SimulationClass
{
    public:
        SimulationClass(){};
        SimulationClass(GLuint* w,GLuint *h){};
        virtual ~SimulationClass(){};
        virtual void updateSpherePosition(vec3 deltaPos){};
        virtual void draw(mat4 projectionMatrix, mat4 viewMatrix){};
        virtual void update(){};
    protected:
    private:
};

#endif // SIMULATIONCLASS_H
