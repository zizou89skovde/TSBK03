#include "Terrain.h"

Terrain::Terrain()
{

    mTerrainModel = new ModelObject();

	//Load shader-kun
    GLuint shader = loadShaders("shaders/phong.vert", "shaders/phong.frag");
	mTerrainModel->setShader(shader,TERRAIN_SHADER);

    // Load model
    TextureData ttex;

    LoadTGATextureData((char*)"textures/terrain2.tga", &ttex);
    GenerateTerrain(&ttex);

    mat4 transformMatrix = IdentityMatrix();
    mTerrainModel->setTransform(transformMatrix,TERRAIN_SHADER);


}

void Terrain::GenerateTerrain(TextureData *tex){
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	GLuint x, z;

	GLfloat vertexArray[3 * vertexCount];
	GLfloat normalArray[3 * vertexCount];
	GLuint  indexArray[triangleCount*3];

    float maxHeight = 10.0;
    float planeRes  = 1.0;
    float heightRes = maxHeight/255.0;

    float planeOffset = planeRes*tex->width/2.0;
    float heightOffset = heightRes*255.0/2.0;

  //  mTerrainMetaData = new vec4(maxHeight,planeRes*tex->width,0,0);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{


// Vertex array. You need to scale this properly
            float xVal = x * planeRes - planeOffset;
            float yVal = tex->imageData[(x + z * tex->width) * (tex->bpp/8)]*heightRes - heightOffset;
            float zVal = z * planeRes - planeOffset;

			vertexArray[(x + z * tex->width)*3 + 0] = xVal;
			vertexArray[(x + z * tex->width)*3 + 1] = yVal;
			vertexArray[(x + z * tex->width)*3 + 2] = zVal;
// Normal vectors. You need to calculate these.
            vec3 normal = calcNormal(x,z,planeRes,heightRes,tex);
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

	// End of terrain generation
	// Create Model and upload to GPU:
	mTerrainModel->LoadDataToModel(
			vertexArray,
			normalArray,
			NULL,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3,
			TERRAIN_SHADER
			);

}

vec3 Terrain::calcNormal(GLuint x,GLuint z,GLfloat planeRes,GLfloat heightRes, TextureData * tex) {
    GLfloat steps[] =
    {
        1,0,
        1,1,
        0,1,
        -1,0,
        -1,-1,
        0,-1
    };

    GLfloat y = tex->imageData[(x + z * tex->width) * (tex->bpp/8)]*heightRes;
    vec3 center = vec3(x*planeRes,y*heightRes,z*planeRes);


    vec3 * triVectors = (vec3 * )malloc(sizeof(GLfloat)*3*6);

    for(GLuint i=0; i < 6; i ++){
        GLuint xIndex = x+steps[2*i];
        GLuint zIndex = z+steps[2*i+1];

        GLfloat yVal = y;
        if(xIndex >= 0 && xIndex < tex->width - 1){
            yVal = tex->imageData[(xIndex + zIndex * tex->width) * (tex->bpp/8)]*heightRes;
        }

        vec3 endPoint = vec3(xIndex*planeRes,yVal,zIndex*planeRes);
        triVectors[i] = endPoint - center;

    }
    vec3 normal = vec3(0,0,0);
    for(GLuint i=0; i < 6; i ++){
        GLuint i1 = i;
        GLuint i2 = (i+1) % 6;
        normal += Normalize(CrossProduct(triVectors[i1],triVectors[i2]));

    }
    free(triVectors);
    normal /= 6.0;

	return normal;
}


Terrain::~Terrain()
{
    //dtor
}

void Terrain::draw(mat4 proj, mat4 view){
    mTerrainModel->draw(proj,view);
}

