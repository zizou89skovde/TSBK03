#ifndef TREESIMULATION_H
#define TREESIMULATION_H

#include "GL_utilities.h"
#include "ModelObject.h"
#include "LoadTGA.h"
#include "Terrain.h"

#define TREE_SPHERE_SHADER_ID 0
#define TREE_SHADER_ID 1

#define FLOATS_PER_VERTEX 3
#define FLOATS_PER_TEXEL 4
#define VERTICES_PER_TRIANGLE 3

class TreeSimulation
{
    public:
        TreeSimulation();
        virtual ~TreeSimulation();
        void initialize();
        void draw(mat4 projectionMatrix,mat4 viewMatrix);
        void setTerrain(Terrain * terrain);
        void update();

    protected:

    private:
        Terrain * mTerrain;
    	ModelObject * mTreeScene;
        GLfloat previousTime;
		GLfloat angle;

    	void uploadBufferCoordinates(ModelObject * modelobject,GLuint shaderId);
};

#endif // TREESIMULATION_H
