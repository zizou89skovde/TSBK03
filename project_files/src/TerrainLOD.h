#ifndef TERRAINLOD_H
#define TERRAINLOD_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "ScreenSize.h"
#include "Environment.h"
#include "LoadTGA.h"


#define SHADER_TERRAIN_LOD 0
#define VERTICES_PER_TRIANGLE 3
#define FLOATS_PER_POSITION 3
#define INDICES_PER_QUAD 6
class TerrainLOD{

    public:
        TerrainLOD(){};
        virtual ~TerrainLOD(){};

        void initialize();
        void draw(mat4 projectionMatrix,mat4 viewMatrix);
        void setEnvironment(Environment * environment){mEnvironment = environment;}

        //void TerrainLOD::setCameraInfo(vec3 * cameraEye, vec3* cameraCenter);
    protected:

    private:
        void generateTerrain();
        void generateLODBuffers(ModelObject * modelObj,GLuint shaderId);
        void uploadSquareModelData(ModelObject * modelObj,GLuint shaderId);
        void debug();

        ModelObject * mTerrainLODScene;
        GLfloat mTextureSize[2];
        Environment * mEnvironment;

        static const GLuint TerrainBaseDimension = 64;




};


#endif // TERRAIN_H
