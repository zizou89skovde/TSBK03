#ifndef TERRAIN_H
#define TERRAIN_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"

//#include "LoadTGA.h"
typedef struct{
        float TerrainSize;
        float TerrainDimension; /*Number of elements per size*/
        float TerrainResolution; /* size divided by dimension */
        float HeightScale;
    }TerrainMetaData;
#define TERRAIN_SHADER 0
class Terrain
{


    public:
        Terrain(GLuint * w, GLuint * h);
        void draw(mat4 proj, mat4 view);
        TextureData * getTextureData();
        TerrainMetaData * getTerrainMetaData();
        FBOstruct * getTerrainFBO();
        FBOstruct * getTerrainReflectedFBO();
        virtual ~Terrain();
    protected:
    private:
        void GenerateTerrain(TextureData *tex);
        vec3 calcNormal(GLuint x,GLuint z,GLfloat planeRes,GLfloat heightRes, TextureData * tex);
        void uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId);

       TerrainMetaData * mTerrainMetaData;

        TextureData* mTerrainTextureData;
        ModelObject* mTerrainModel;

        FBOstruct* mTerrainFBO;
        FBOstruct* mTerrainReflectionFBO;
        GLuint *mScreenWitdh;
        GLuint *mScreenHeight;
};

#endif // TERRAIN_H
