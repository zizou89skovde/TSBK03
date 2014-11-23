#ifndef GRASSSIMULATION_H
#define GRASSSIMULATION_H



#include "GL_utilities.h"
#include "ModelObject.h"
#include "loadTGA.h"

#define GRASS_SPHERE_SHADER_ID 0
#define GRASS_SHADER_ID 1


#define FLOATS_PER_VERTEX 3
#define FLOATS_PER_TEXEL 4
#define VERTICES_PER_TRIANGLE 3

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

    	void uploadBufferCoordinates(ModelObject * modelobject,GLuint shaderId);
};

#endif // GRASSSIMULATION_H
