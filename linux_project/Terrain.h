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
        Model* GenerateTerrain(TextureData *tex);

        ModelObject* mTerrainModel;
};

#endif // TERRAIN_H
