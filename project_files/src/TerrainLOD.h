#ifndef TERRAINLOD_H
#define TERRAINLOD_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "ScreenSize.h"
#include "LoadTGA.h"


#define SHADER_TERRAIN_LOD 0
typedef struct{
        float TerrainSize;
        float TerrainDimension; /*Number of elements per size*/
        float TerrainResolution; /* size divided by dimension */
        float HeightScale;
        float TerrainHeightOffset;
}TerrainMetaData;

class TerrainLOD{

    public:
        TerrainLOD(){};
        virtual ~TerrainLOD(){};

        void initialize();
        void draw(mat4 projectionMatrix,mat4 viewMatrix);
        TextureData * getTextureData();
        TerrainMetaData * getTerrainMetaData();
    protected:

    private:
        void generateLODBuffers(ModelObject * modelObj,GLuint shaderId);
        void uploadSquareModelData(ModelObject * modelObj,GLuint shaderId);
        ModelObject * mTerrainLODScene;
        GLfloat mTextureSize[2];

        /** Ny meta **/
        static const GLfloat TerrainHeightOffset = 0.0;
        static const GLfloat TerrainHeightScale =  5.0;
        static const GLfloat TerrainPlaneScale =  20.0;


        /** Two types of terrain data containers **/
        TerrainMetaData * mTerrainMetaData;
        TextureData* mTerrainTextureData;
};


#endif // TERRAIN_H
