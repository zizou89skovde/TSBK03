#ifndef GRASSSIMULATION_H
#define GRASSSIMULATION_H

#include "GL_utilities.h"
#include "ModelObject.h"
#include "LoadTGA.h"
#include "Environment.h"

#define GRASS_SHADER_ID	0

#define FLOATS_PER_VERTEX 3
#define FLOATS_PER_TEXEL 4


class GrassSimulation
{
    public:
        GrassSimulation();
        virtual ~GrassSimulation();
        void initialize();
        void draw(mat4 projectionMatrix,mat4 viewMatrix);
        void setTerrain(Environment * terrain);
        void update();
       void setEnvironment(Environment * environment){mEnvironment = environment;}

    protected:

    private:
        Environment * mEnvironment;
    	ModelObject * mGrassScene;
        GLfloat previousTime;
		GLfloat angle;

    	void uploadBufferCoordinates(ModelObject * modelobject,TextureData * maskTexture,GLuint shaderId);
};

#endif // GRASSSIMULATION_H
