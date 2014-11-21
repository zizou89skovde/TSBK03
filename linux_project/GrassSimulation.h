#ifndef GRASSSIMULATION_H
#define GRASSSIMULATION_H

#include "GL_utilities.h"
#include "loadobj.h"
#include "ModelObject.h"

#define GRASS_SPHERE_SHADER_ID 0
class GrassSimulation
{
    
    
    public:
        GrassSimulation();
        virtual ~GrassSimulation();
        void draw(mat4 projectionMatrix,mat4 viewMatrix);
        void update();
    protected:
    private:
    ModelObject * mGrassScene;
};

#endif // GRASSSIMULATION_H
