#include "CelShading.h"

CelShading::CelShading(GLuint* w, GLuint* h)
{
    mScreenWidth = w;
    mScreenHeight = h;
    mCelShadingSwitch = 0.0;
    mCelShadingSteps = 5.0;
    mCelShadingContourThresh = 0.3;

    srand (static_cast <unsigned> (time(0)));
}

CelShading::~CelShading()
{

}

void CelShading::initialize(FBOstruct* fbo_in)
{
    // Save FBO to be used later
    mFBO = fbo_in;

    // Initialize contour FBO
    mContourFBO = initFBO(SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	// Initalize celshader object
    mCelObject = new ModelObject();


    // Object Shader
    GLuint objectShader = loadShaders("shaders/sphere.vert", "shaders/object_speed.frag");
    mCelObject->setShader(objectShader, CEL_OBJECT_SHADER_ID);

    // Object data
    Object* object = new Object();
    object->position = new vec3(mObjectOffsetX, mObjectOffsetY, mObjectOffsetZ);
    object->velocity = new vec3(0.0, 0.0, 0.0);
    object->rotation = new vec3(0.0, 0.5, 0.5);
    object->angular = new vec3(0.05, 0.0, 0.0);
    object->radius = mObjectRadius;
    mObject = object;

    // Object Model
    Model* modelObject = LoadModelPlus((char *)"models/bunny.obj");
    mCelObject->setModel(modelObject, CEL_OBJECT_SHADER_ID);
    mCelObject->freeModelData(modelObject);

    // Object Transform
    GLfloat r = object->radius;
    vec3  pos = *object->position;
    mat4 objectTransform = T(pos.x, pos.y, pos.z) * S(r, r, r);
    mCelObject->setTransform(objectTransform, CEL_OBJECT_SHADER_ID);

    mCelObject->setSpeedlinesInit(CEL_OBJECT_SHADER_ID);
    mCelObject->setSpeedModelsInit(CEL_OBJECT_SHADER_ID);
    GLfloat speedlinesAlpha = 1.0;
    mCelObject->setUniformFloat(&speedlinesAlpha, 1, CEL_OBJECT_SHADER_ID, "u_SpeedlinesAlpha");


    // Cel shader
   	GLuint celShader = loadShaders("shaders/celshader.vert", "shaders/celshader.frag");
    mCelObject->setShader(celShader, CEL_SCREEN_QUAD_ID, NONE);

	// Full-screen quad model
	uploadSquareModelData(mCelObject, CEL_SCREEN_QUAD_ID);

    // Set texture to cel-shade
    mCelObject->setTexture(fbo_in->texid, CEL_SCREEN_QUAD_ID, "u_Texture");

    // Set uniforms
    mCelObject->setUniformFloat(&mCelShadingSwitch, 1, CEL_SCREEN_QUAD_ID, "u_CelShadingSwitch");
    mCelObject->setUniformFloat(&mCelShadingSteps, 1, CEL_SCREEN_QUAD_ID, "u_Steps");
    mCelObject->setUniformFloat(&mCelShadingContourThresh, 1, CEL_SCREEN_QUAD_ID, "u_ContourThresh");
    // Load sketch image to cel shader
    loadSketch();


    // Depth derivation shader
   	GLuint deriveShader = loadShaders("shaders/celshader.vert", "shaders/deriveshader.frag");
    mCelObject->setShader(deriveShader, CEL_DERIVE_SHADER_ID, NONE);

	// Full-screen quad model
	uploadSquareModelData(mCelObject, CEL_DERIVE_SHADER_ID);

	// Set depth buffer as texture for the depth derivation shader
	mCelObject->setTexture(mFBO->depth, CEL_DERIVE_SHADER_ID , "u_DepthTexture");
}

void CelShading::switchCelShading()
{
    if (mCelShadingSwitch < 1.0) {
        mCelShadingSwitch = 2.0;
        printf("Turning cel shading ON\n");
    }
    else {
        mCelShadingSwitch = 0.0;
        printf("Turning cel shading OFF\n");
    }
    mCelObject->setUniformFloat(&mCelShadingSwitch, 1, CEL_SCREEN_QUAD_ID, "u_CelShadingSwitch");
}

void CelShading::increaseSteps()
{
    ++mCelShadingSteps;
    mCelObject->setUniformFloat(&mCelShadingSteps, 1, CEL_SCREEN_QUAD_ID, "u_Steps");
}

void CelShading::decreaseSteps()
{
    --mCelShadingSteps;
    mCelObject->setUniformFloat(&mCelShadingSteps, 1, CEL_SCREEN_QUAD_ID, "u_Steps");
}

void CelShading::increaseContourThresh()
{
    mCelShadingContourThresh += 0.05;
    mCelObject->setUniformFloat(&mCelShadingContourThresh, 1, CEL_SCREEN_QUAD_ID, "u_ContourThresh");
}

void CelShading::decreaseContourThresh()
{
    mCelShadingContourThresh -= 0.05;
    mCelObject->setUniformFloat(&mCelShadingContourThresh, 1, CEL_SCREEN_QUAD_ID, "u_ContourThresh");
}

void CelShading::draw(mat4 projectionMatrix, mat4 viewMatrix)
{
    // Draw debug object to previously chosen fbo (both texture & depth)
	mCelObject->draw(CEL_OBJECT_SHADER_ID, projectionMatrix, viewMatrix);

    useFBO(mContourFBO, 0L, 0L);
    glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
    mCelObject->draw(CEL_DERIVE_SHADER_ID, projectionMatrix, viewMatrix);

    // Set what to use as texture
    mCelObject->setTexture(mFBO->texid, CEL_SCREEN_QUAD_ID, "u_Texture");
    mCelObject->setTexture(mContourFBO->texid, CEL_SCREEN_QUAD_ID, "u_Contours");

	// Perform celshading and present buffer on screen (through quad)
	useFBO(0L, 0L, 0L);
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	mCelObject->draw(CEL_SCREEN_QUAD_ID, projectionMatrix, viewMatrix);

    // Update object position
    //vec3 deltaPos = vec3(mObjectSpeed, 0.0, 0.0);
    //updateObjectPosition(deltaPos);
    updateObjectTransform();
}

void CelShading::loadSketch()
{
    GLuint sketchTexture;
    LoadTGATextureSimple((char *)"textures/sketchy1.tga", &sketchTexture);
    mCelObject->setTexture(sketchTexture, CEL_SCREEN_QUAD_ID, "u_SketchTexture");
}

void CelShading::uploadSquareModelData(ModelObject* modelObj, GLuint shaderId)
{
    GLfloat square[] = {
                    -1,-1,0,
                    -1,1, 0,
                    1,1, 0,
                    1,-1, 0};
    GLfloat squareTexCoord[] = {
                     0, 0,
                     0, 1,
                     1, 1,
                     1, 0};

    GLuint squareIndices[] = {0, 2, 1, 0, 3, 2};
    modelObj->LoadDataToModel(
        square,
        NULL,
        squareTexCoord,
        NULL,
        squareIndices,
        4,
        2*3,
        shaderId);

    printError("Cloth Gen Position Buffer Coordinates ");
}

void CelShading::fireObject()
{
    vec3* pos = mObject->position;
    vec3* rot = mObject->rotation;

    // Reset positions and velocities
    *pos = vec3(mObjectOffsetX, mObjectOffsetY, mObjectOffsetZ);
    *rot = vec3(0.0, 0.5, 0.5);
    *mObject->velocity = vec3(randFloat(0.0, 0.5), randFloat(0.3, 0.6), 0.0);
    *mObject->angular = vec3(randFloat(0.0, 0.3), 0.0, 0.0);

    // Update transform
    mat4* transf = mCelObject->getTransform(CEL_OBJECT_SHADER_ID);
    *transf = T(pos->x, pos->y, pos->z) * Rx(rot->x) * Ry(rot->y) * Rz(rot->z);
}

GLfloat CelShading::randFloat(GLfloat min, GLfloat max)
{
    float r3 = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
}

void CelShading::updateObjectTransform()
{
    // Add speeds
    vec3* pos = mObject->position;
    vec3* rot = mObject->rotation;
    *pos = *pos + *mObject->velocity;
    *rot = *rot + *mObject->angular;

    // Change velocities due to drag and gravity
    *mObject->velocity *= 0.97;
    *mObject->angular *= 0.99;
    mObject->velocity->y -= 0.0035;

    // Update transform
    mat4* transf = mCelObject->getTransform(CEL_OBJECT_SHADER_ID);
    *transf = T(pos->x, pos->y, pos->z) * Rx(rot->x) * Ry(rot->y) * Rz(rot->z);
}

void CelShading::updateObjectPosition(vec3 deltaPos)
{
    /** Update position of object **/
    Object * s = mObject;
    *s->position += deltaPos;
    s->position->y = sin(s->position->x);
    /** Update model transform **/
    mat4 * transf = mCelObject->getTransform(CEL_OBJECT_SHADER_ID);
    GLfloat r = s->radius;
    vec3  pos = *s->position;

    *transf = T(pos.x,pos.y,pos.z)*S(r,r,r);
}

