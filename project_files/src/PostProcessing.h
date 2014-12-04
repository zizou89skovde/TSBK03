#ifndef PostProcessing_H
#define PostProcessing_H

#include "ModelObject.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"

#define SHADER_LIGHT_VOLUME 0
#define SHADER_SPHERE 1
class PostProcessing
{


    public:
        PostProcessing(GLuint * w, GLuint * h);
        void draw(mat4 proj, mat4 view);
        virtual ~PostProcessing();
    protected:
    private:
		void drawLightDepth();
        void initializePostProcessing();
		void generateFrustumMesh(GLfloat far, GLfloat near, GLuint dimension);

		/* Shortcut to light volume shader handle */
		GLuint mLightShaderHandle;

		GLuint* mScreenHeight;
		GLuint* mScreenWidth;
		/* FBO light depth buffer */
		FBOstruct * mLightDepthFBO;
		static const GLuint mFBOSize = 512;		

		mat4 mLightProjectionMatrix;
		mat4 mLightViewMatrix;
		mat4 mVPLightMatrix;

		/* Light frustum */
		static const GLfloat mFar = 20.0f;
		static const GLfloat mNear = 1.0f;
		static const GLfloat mRatio = 1.0f;

		ModelObject * mPostProcessingModel;
		
};

#endif // PostProcessing_H
