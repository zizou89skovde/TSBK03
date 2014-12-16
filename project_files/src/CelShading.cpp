#include "CelShading.h"

CelShading::CelShading(GLuint* w, GLuint* h)
{
    mScreenWidth = w;
    mScreenHeight = h;

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


    // Sphere Shader
    GLuint sphereShader = loadShaders("shaders/sphere.vert", "shaders/sphere.frag");
    mCelObject->setShader(sphereShader, CEL_SPHERE_SHADER_ID);
//    mCelObject->setDrawMethod(CEL_SPHERE_SHADER_ID, ARRAYS);

    // Sphere data
    Sphere* sphere = new Sphere();
    sphere->position = new vec3(mSphereOffsetX, mSphereOffsetY, mSphereOffsetZ);
    sphere->radius = mSphereRadius;
    mSphere = sphere;

    // Sphere Model
    Model* modelSphere = LoadModelPlus((char *)"models/sphere.obj");
    mCelObject->setModel(modelSphere, CEL_SPHERE_SHADER_ID);
    mCelObject->freeModelData(modelSphere);

    // Sphere Transform
    GLfloat r = sphere->radius;
    vec3  pos = *sphere->position; // stjärna?
    mat4 sphereTransform = T(pos.x, pos.y, pos.z) * S(r, r, r);
    mCelObject->setTransform(sphereTransform, CEL_SPHERE_SHADER_ID);

    mCelObject->setSpeedlinesInit(CEL_SPHERE_SHADER_ID);


    // Cel shader
   	GLuint celShader = loadShaders("shaders/celshader.vert", "shaders/celshader.frag");
    mCelObject->setShader(celShader, CEL_SCREEN_QUAD_ID, NONE);

	// Full-screen quad model
	uploadSquareModelData(mCelObject, CEL_SCREEN_QUAD_ID);

    // Set texture to cel-shade
    mCelObject->setTexture(fbo_in->texid, CEL_SCREEN_QUAD_ID, "u_Texture");

    // Load sketch image to cel shader
    loadSketch();


    // Depth derivation shader
   	GLuint deriveShader = loadShaders("shaders/celshader.vert", "shaders/deriveshader.frag");
    mCelObject->setShader(deriveShader, CEL_DERIVE_SHADER_ID, NONE);

	// Full-screen quad model
	uploadSquareModelData(mCelObject, CEL_DERIVE_SHADER_ID);

    // Create depth FBO
	generateDepthFBO(&mDepthFBO, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Set depth buffer as texture for the depth derivation shader
	mCelObject->setTexture(mFBO->depth, CEL_DERIVE_SHADER_ID , "u_DepthTexture");
}

void CelShading::draw(mat4 projectionMatrix, mat4 viewMatrix)
{
    // Draw debug sphere to previously chosen fbo (both texture & depth)
	//mCelObject->draw(CEL_SPHERE_SHADER_ID, projectionMatrix, viewMatrix);

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

    // Update sphere position
    vec3 deltaPos = vec3(mSphereSpeed, 0.0, 0.0);
    updateSpherePosition(deltaPos);
}

// Obsolete! TODO ta bort
void CelShading::drawDepth(mat4 projectionMatrix, mat4 viewMatrix)
{
	// Swap to depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, mDepthFBO.fb);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Clear previous frame values
    glClear(GL_DEPTH_BUFFER_BIT);
    // Disable color rendering, we only want to write to the Z-Buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	// Render regular scene from camera
    mCelObject->draw(CEL_SPHERE_SHADER_ID, projectionMatrix, viewMatrix);

	// Enable color rendering again
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

// Following code comes from TSBK03 literature
void CelShading::generateDepthFBO(FBOstruct* fbo, GLuint w ,GLuint h)
{
    // Try to use a texture depth component
    glGenTextures(1, &fbo->depth);
    glBindTexture(GL_TEXTURE_2D, fbo->depth);

    // GL_LINEAR does not make sense for depth texture.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Clamp to avoid artefacts on the edges
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create a framebuffer object
    glGenFramebuffers(1, &fbo->fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);

    // Instruct OpenGL that we won't bind a color texture to the FBO
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Attach the texture to FBO depth attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER,
    GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, fbo->depth, 0);

    // Check FBO status
    GLenum FBOstatus;
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE)
        printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");

    // Switch back to window-system-provided framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CelShading::loadSketch()
{
    GLuint sketchTexture;
    LoadTGATextureSimple((char *)"textures/sketchy.tga", &sketchTexture);
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

void CelShading::updateSpherePosition(vec3 deltaPos)
{
    /** Update position of sphere **/
    Sphere * s = mSphere;
    *s->position += deltaPos;
    s->position->y = sin(s->position->x);
    /** Update model transform **/
    mat4 * transf = mCelObject->getTransform(CEL_SPHERE_SHADER_ID);
    GLfloat r = s->radius;
    vec3  pos = *s->position;

    *transf = T(pos.x,pos.y,pos.z)*S(r,r,r);
}

