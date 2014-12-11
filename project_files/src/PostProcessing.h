#ifndef PostProcessing_H
#define PostProcessing_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"
#include "Terrain.h"

//#define DEBUG 0

#define SHADER_LIGHT_VOLUME 0
#define SHADER_SPHERE       1
#define SHADER_SCREEN_QUAD  2
#define SHADER_SHADOW_MAP   3
class PostProcessing
{


    public:
        PostProcessing(GLuint * w, GLuint * h);
        void draw(mat4 proj, mat4 view);
        void setTerrin(Terrain* terrain){mTerrain = terrain;};
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
        Terrain * mTerrain;

		/** Shortcut to light volume shader handle **/
		GLuint mLightShaderHandle;
		GLuint mShadowShaderHandle;

		GLuint* mScreenHeight;
		GLuint* mScreenWidth;

		/** FBO light depth buffer **/
		FBOstruct mLightDepthFBO;
		FBOstruct * mLightFBO;
		FBOstruct mSceneDepthFBO;
		static const GLuint mFBOSize = 512;

		mat4 mLightProjectionMatrix;
		mat4 mLightViewMatrix;
		mat4 mVPLightMatrix;
		mat4 mMVPLightMatrix;
		mat4 mModelLightMatrix;
		mat4 mLightTextureMatrix;

		/** Light frustum **/
		static const GLfloat mFar = 15.0f;
		static const GLfloat mNear = 1.0f;
		static const GLfloat mRatio = 1.0f;
		static const GLfloat mScaleFar = 0.45f;
		static const GLfloat mScaleNear = 0.05f;
        static const GLfloat mOffsetFar = 15;
		static const GLfloat mOffsetNear = 1;

		/** Light Source Position **/
		static const GLfloat mLightRadius = 15.0;
		static const GLfloat mLightHeight = 1.0;
		static const GLfloat mLightSpeed =  0.0005;
		static const GLfloat mLightResolutiuon = 64.0;

		ModelObject * mPostProcessingModel;
        GLfloat mTime;
};

#endif // PostProcessing_H

