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
        float TerrainHeightOffset;
    }TerrainMetaData;
#define TERRAIN_SHADER 0
#define SKYBOX_SHADER 1
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
        void initializeTerrain();
        void initializeSkyBox();
        void GenerateTerrain(TextureData *tex);
        vec3 calcNormal(GLuint x,GLuint z,GLfloat planeRes,GLfloat heightRes, TextureData * tex);
        void uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId);
        void setClip(bool enabled);

        /** Terrain model object for rendering **/
        ModelObject* mTerrainModel;

        /** Two types of terrain data containers **/
        TerrainMetaData * mTerrainMetaData;
        TextureData* mTerrainTextureData;


        /** Render targets. Used for reflecting and refracting the water **/
        const static GLuint FBOSize =  512;
        FBOstruct* mTerrainFBO;
        FBOstruct* mTerrainReflectionFBO;

        /** Pointers to screen dimension known by the main **/
        GLuint *mScreenWitdh;
        GLuint *mScreenHeight;

        static const GLfloat TerrainHeightOffset = -2.0;
        static const GLfloat TerrainHeightScale =  10.0;
        static const GLfloat TerrainPlaneScale =  40.0;


};

#endif // TERRAIN_H
