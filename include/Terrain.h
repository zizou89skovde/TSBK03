#ifndef TERRAIN_H
#define TERRAIN_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadTGA.h"

//#include "LoadTGA.h"
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
