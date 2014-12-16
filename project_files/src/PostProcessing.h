#ifndef PostProcessing_H
#define PostProcessing_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"
#include "Environment.h"

//#define DEBUG 0
//#define SHADOW_MAP 1

#define SHADER_LIGHT_VOLUME 0
#define SHADER_SPHERE       1
#define SHADER_LIGHT_EFFECT  2
#define SHADER_SHADOW_MAP   3
class PostProcessing
{


    public:
        PostProcessing(GLuint * w, GLuint * h);
        void draw(mat4 proj, mat4 view);
        void setTerrin(Environment* terrain){mTerrain = terrain;};
        void setCameraInfo(vec3* cameraEye,vec3* cameraCenter);
        virtual ~PostProcessing();
    protected:
    private:
		void drawLightDepth(mat4 proj, mat4 view);
		void drawLightVolume(mat4 proj, mat4 view);
		void drawShadows(mat4 proj, mat4 view);
        void initializePostProcessing();

        void lightLookAt(vec3 lightPos, vec3 lightDir);

		void generateFrustumMesh(GLfloat far, GLfloat near, GLuint dimension);
        void generateDepthFBO(FBOstruct * fbo, GLuint w ,GLuint h);
        void uploadSquareModelData(ModelObject * modelObj,GLuint shaderId);

        /** Terrain Handle **/
        Environment * mTerrain;

		GLuint* mScreenHeight;
		GLuint* mScreenWidth;

		/** FBO light depth buffer **/
		FBOstruct mLightDepthFBO;
		FBOstruct * mLightFBO;
		FBOstruct mSceneDepthFBO;
		static const GLuint mFBOWidth  = SCREEN_WIDTH;
		static const GLuint mFBOHeight = SCREEN_HEIGHT;

		mat4 mLightProjectionMatrix;
		mat4 mLightViewMatrix;
		mat4 mVPLightMatrix;
		mat4 mMVPLightMatrix;
		mat4 mModelLightMatrix;
		mat4 mLightTextureMatrix;

		/** Light frustum **/
		#define FAR_LIGHT 10
		static const GLfloat mFar = FAR_LIGHT;
		static const GLfloat mNear = 1.0f;
		static const GLfloat mRatio     = 1.0f;
		static const GLfloat mScaleFar  = 0.2f;
		static const GLfloat mScaleNear = 0.05f;
        static const GLfloat mOffsetFar = FAR_LIGHT;
		static const GLfloat mOffsetNear = 1;

		/** Light Source Position **/
		static const GLfloat mLightRadius = 3.0;
		static const GLfloat mLightHeight = 6.0;
		static const GLfloat mLightSpeed =  0.0005;
		static const GLfloat mLightResolutiuon = 64.0;

		ModelObject * mPostProcessingModel;
        GLfloat mTime;

        vec3 * mCameraEye;
        vec3 * mCameraCenter;
};

#endif // PostProcessing_H

