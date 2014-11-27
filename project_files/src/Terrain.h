#ifndef TERRAIN_H
#define TERRAIN_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"

//#include "LoadTGA.h"

#define TERRAIN_SHADER 0
class Terrain
{
    public:
        Terrain();
        void draw(mat4 proj, mat4 view);
        virtual ~Terrain();
    protected:
    private:
        void GenerateTerrain(TextureData *tex);
        vec3 calcNormal(GLuint x,GLuint z,GLfloat planeRes,GLfloat heightRes, TextureData * tex);
        void uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId);

        vec4 * mTerrainMetaData;

        ModelObject* mTerrainModel;
};

#endif // TERRAIN_H
