#include "PostProcessing.h"

PostProcessing::P(GLuint * w, GLuint * h)
{
    mScreenWitdh = w;
    mScreenHeight = h;
    mPostProcessingModel = new ModelObject();
    initializePostProcessing();


}

void PostProcessing::initializePostProcessing(){

}

void PostProcessing::GenerateLightVolumeMesh(GLfloat far, GLfloat near, GLuint resolution){

	GLuint x, z;
    GLfloat *vertexArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLuint *indexArray = (GLuint*)malloc(sizeof(GLuint) * triangleCount*3);


	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{


// Vertex array. You need to scale this properly
            float xVal = x * gridRes - gridOffset;
            float yVal = tex->imageData[(x + z * tex->width) * (tex->bpp/8)]*heightConversion - heightOffset;
            float zVal = z * gridRes - gridOffset;

			vertexArray[(x + z * tex->width)*3 + 0] = xVal;
			vertexArray[(x + z * tex->width)*3 + 1] = yVal;
			vertexArray[(x + z * tex->width)*3 + 2] = zVal;
// Normal vectors. You need to calculate these.
            vec3 normal = calcNormal(x,z,gridRes,heightConversion,tex);
			normalArray[(x + z * tex->width)*3 + 0] = normal.x;
			normalArray[(x + z * tex->width)*3 + 1] = normal.y;
			normalArray[(x + z * tex->width)*3 + 2] = normal.z;
// Texture coordinates. You may want to scale them.
		/*	texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;*/
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}

	// End of PostProcessing generation
	// Create Model and upload to GPU:
	mPostProcessingModel->LoadDataToModel(
			vertexArray,
			normalArray,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3,
			PostProcessing_SHADER
			);

    /** Clean up **/
    free(vertexArray);
    free(normalArray);
    free(indexArray);


  printError("PostProcessing Buffer");

}


PostProcessing::~PostProcessing()
{
    //dtor
}


void PostProcessing::draw(mat4 proj, mat4 view){




}

