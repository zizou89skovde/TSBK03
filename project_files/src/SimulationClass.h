#ifndef SIMULATIONCLASS_H
#define SIMULATIONCLASS_H
#include "VectorUtils3.h"
#include <vector>
class SimulationClass
{

    public:

        typedef struct{
            vec3 *position;
            float radius;
        }Sphere;

        SimulationClass(){};
        SimulationClass(GLuint* w,GLuint *h){};
        virtual ~SimulationClass(){};
        virtual void updateSpherePosition(vec3 deltaPos){};
        virtual void draw(mat4 projectionMatrix, mat4 viewMatrix){};
        virtual void update(){};
    protected:
          /** Sphere Collision **/
          std::vector<Sphere*> mSpheres;
    private:
};

#endif // SIMULATIONCLASS_H
