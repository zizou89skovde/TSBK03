#include "CollisionObject.h"

CollisionObject::CollisionObject(){
    mModels  = new ModelObject();
};

CollisionObject::~CollisionObject(){

}

void CollisionObject::addSphere(GLfloat* startPosition,GLfloat sizeSphere){

}

void CollisionObject::addHeightMap(const char* filePath,GLfloat* startPosition,GLfloat * dimension){


}


void CollisionObject::updatePosition(vec3 deltaPos){

}

void CollisionObject::draw(mat4 projectionMatrix, mat4 viewMatrix) {


}

void update(){


}
