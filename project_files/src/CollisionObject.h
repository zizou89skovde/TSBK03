#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H
#include "ModelObject.h"
#include "VectorUtils3.h"

#include <vector>
class CollisionObject
{

    typedef struct{
        GLfloat position[3];
        float radius;
    }Sphere_Type;

    typedef struct{
        GLfloat position[3];
        GLfloat dimension[3];
        float amplitude;

    }HeightMap_Type;

    public:
        CollisionObject();
        virtual ~CollisionObject();
        void updatePosition(vec3 deltaPos);
        void draw(mat4 projectionMatrix, mat4 viewMatrix);
        void update();
        void addSphere(GLfloat* startPosition,GLfloat sizeSphere);
        void addHeightMap(const char* filePath,GLfloat* startPosition,GLfloat * dimension);

    protected:

    private:

          /** Sphere Collision **/
          std::vector<Sphere_Type*> mSpheres;

          /** Height Map Collision **/
          std::vector<HeightMap_Type*> mHeightMaps;

          ModelObject * mModels;
};

#endif // SIMULATIONCLASS_H
