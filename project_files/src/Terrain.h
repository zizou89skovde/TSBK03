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
#define SKYBOX_SHADER 0
#define TERRAIN_SHADER 1
#define TERRAIN_SIMPLE_SHADER 2
class Terrain
{


    public:
        Terrain(GLuint * w, GLuint * h);
        void draw(mat4 proj, mat4 view);
        void drawSimple(mat4 proj, mat4 view);
        void drawDepth(mat4 proj, mat4 view);
        TextureData * getTextureData();
        TerrainMetaData * getTerrainMetaData();
        FBOstruct * getTerrainFBO();
        FBOstruct * getTerrainReflectedFBO();
        void setExternalModels(ModelObject * modelObject);
        virtual ~Terrain();
    protected:
    private:
        void initializeTerrain();
        void initializeSkyBox();

        void GenerateTerrain(TextureData *tex);

        vec3 calcNormal(GLuint x,GLuint z,GLfloat planeRes,GLfloat heightRes, TextureData * tex);
        void uploadBufferCoordinates(ModelObject * modelObj,GLuint shaderId);

        void renderFlippedExternalModel(mat4 projectionMatrix,mat4 viewMatrix);
        void setClip(bool enabled);

        /** Terrain model object for rendering **/
        ModelObject* mTerrainModel;
        ModelObject* mTerrainDepthModel;

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

        static const GLfloat TerrainHeightOffset = 0.0;
        static const GLfloat TerrainHeightScale =  5.0;
        static const GLfloat TerrainPlaneScale =  20.0;

        std::vector<ModelObject *> mExternalModel;


};

#endif // TERRAIN_H
