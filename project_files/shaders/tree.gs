#version 400

#define NUM_OF_TREE_VERTICES 16
#define NUM_OF_POINTS_PER_CIRCLE NUM_OF_TREE_VERTICES/2

//#define PI 3.141592653589793

layout (triangles) in;
layout (triangle_strip, max_vertices = NUM_OF_TREE_VERTICES) out;

uniform sampler2D u_TreeMask;
uniform sampler2D u_TreeNoise;

uniform mat4 VP_Matrix;
uniform mat4 V_Matrix;
uniform mat4 P_Matrix;

//uniform float u_Wind;

in vec2 g_TextureCoord[3];
in float g_Height[3];

out vec3 f_Normal;
void createTreeStraw1(inout vec3 treeVertices[NUM_OF_TREE_VERTICES], in float treeNoise);
void createLog(inout vec3 treeVertices[NUM_OF_TREE_VERTICES], inout float length, inout float radius_bottom, inout float radius_top, inout float level, in float treeNoise);
void createNormals(inout vec3 treeNormals[NUM_OF_TREE_VERTICES], in vec3 treeVertices[NUM_OF_TREE_VERTICES]);
void getPosition(inout vec3 position, inout float height, inout vec2 texCoord);

//void rotateTree(inout vec3 treeVertices[NUM_OF_TREE_VERTICES], in float theta, in vec3 position, in float height, in float treeNoise);

void main()
{
	/* Get  position in a triangle (In world coordinates) */
	vec3 position = vec3(0.0);
	float height = 0.0;
	vec2 texCoord = vec2(0.0);
	getPosition(position, height, texCoord);

	/* Read Tree noise from texture, returns value between 0 and 1 */
	float treeNoise = texture(u_TreeNoise, texCoord).x;

	/* Init tree properties */
	float length = 2.0;
	float radius_bottom = 6.0;
	float radius_top = 2.0;
	float level = 0.0;

	/* Create Tree */
//	vec3 treeVertices[NUM_OF_TREE_VERTICES] = vec3[](vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
	vec3 treeVertices[NUM_OF_TREE_VERTICES];
	for (int i = 0; i < NUM_OF_TREE_VERTICES; ++i) {
		treeVertices[i] = vec3(0.0, 0.0, 0.0);
	}
//createTreeStraw1(treeVertices, treeNoise);
	createLog(treeVertices, length, radius_bottom, radius_top, level, treeNoise);

	/* Rotate Tree with angle theta */
	//float angle =  5.0*treeNoise*PI;

//	rotateTree(treeVertices, angle, position, height, treeNoise);
	for (int i = 0; i < NUM_OF_TREE_VERTICES; ++i) {
		//treeVertices[i] = rotationMatrix*treeVertices[i];
		//treeVertices[i].xz += treeVertices[i].y * wind;
		treeVertices[i] += position;
		treeVertices[i].y += height;
	}


	/* Create normals*/
//	vec3 treeNormals[NUM_OF_TREE_VERTICES] = vec3[](vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
	vec3 treeNormals[NUM_OF_TREE_VERTICES];
	for (int i = 0; i < NUM_OF_TREE_VERTICES; ++i) {
		treeNormals[i] = vec3(0.0, 0.0, 0.0);
	}
	createNormals(treeNormals, treeVertices);

	/* Transform Tree to screen */
	for (int i = 0; i < NUM_OF_TREE_VERTICES; ++i) {
		f_Normal = treeNormals[i];
		gl_Position = VP_Matrix*vec4(treeVertices[i], 1.0);
	
		EmitVertex();
	}
	EndPrimitive();
}

void getPosition(inout vec3 position, inout float height, inout vec2 texCoord)
{
	/* Get position of input triangle primitive */
	for (int i = 0; i < 3; ++i) {
		position += gl_in[i].gl_Position.xyz; 
		height += g_Height[i];
		texCoord += g_TextureCoord[i];
	}
	position /= 3.0;
	height /= 3.0;
	texCoord /= 3.0;
}

void createLog(inout vec3 treeVertices[NUM_OF_TREE_VERTICES], inout float length, inout float radius_bottom, inout float radius_top, inout float level, in float treeNoise)
{
	/* Define Tree properties */
	const float PI = 3.141592653589793;
	float deltaAngle = 2.0*PI/(float(NUM_OF_POINTS_PER_CIRCLE)- 1.0);
	vec3 pos_bottom[NUM_OF_POINTS_PER_CIRCLE];
	vec3 pos_top[NUM_OF_POINTS_PER_CIRCLE];

	for (int i = 0; i < NUM_OF_POINTS_PER_CIRCLE; ++i) {
	 	pos_bottom[i] = vec3(radius_bottom*cos(float(i)*deltaAngle), 0.0, radius_bottom*sin(float(i)*deltaAngle));
		pos_top[i] = vec3(radius_top*cos(float(i)*deltaAngle), length, radius_top*sin(float(i)*deltaAngle));
	}

	for (int j = 0; j < 2*NUM_OF_POINTS_PER_CIRCLE; ++j) {
		// j is odd
		if (j % 2 == 0) {
			treeVertices[j] = pos_bottom[j/2];
		}
		// j is even
		else {
			treeVertices[j] = pos_top[(j - 1)/2];
		}
	}
}


void createNormals(inout vec3 treeNormals[NUM_OF_TREE_VERTICES], in vec3 treeVertices[NUM_OF_TREE_VERTICES])
{
	vec3 delta1 = vec3(0.0);
	vec3 delta2 = vec3(0.0);
	vec3 delta3 = vec3(0.0);
	vec3 delta4 = vec3(0.0);

	for (int i = 0; i < 2*NUM_OF_POINTS_PER_CIRCLE; ++i) {
		vec3 currentVert = treeVertices[i];
		// j is even
		if (i % 2 == 0) {
			
			if ((i + 2) == NUM_OF_TREE_VERTICES) {
				int indexPlus2 = 0;
				delta1 = treeVertices[indexPlus2] - currentVert;
				
				
				delta2 = treeVertices[i + 1] - currentVert;
				delta3 = treeVertices[i - 1] - currentVert;
				delta4 = treeVertices[i - 2] - currentVert;
			}		
			if ((i + 1) == NUM_OF_TREE_VERTICES) {
				int indexPlus1 = 0;
				delta2 = treeVertices[indexPlus1] - currentVert;

				delta1 = treeVertices[i + 2] - currentVert;				
				delta3 = treeVertices[i - 1] - currentVert;
				delta4 = treeVertices[i - 2] - currentVert;
			}
			if ((i - 1) == -1) {
				int indexMinus1 = NUM_OF_TREE_VERTICES - 1;
				delta3 = treeVertices[indexMinus1] - currentVert;

				delta1 = treeVertices[i + 2] - currentVert;
				delta2 = treeVertices[i + 1] - currentVert;
				delta4 = treeVertices[i - 2] - currentVert;
			}
			if ((i - 2) == -2) {
				int indexMinus2 = NUM_OF_TREE_VERTICES - 2;
				delta4 = treeVertices[indexMinus2] - currentVert;

				delta1 = treeVertices[i + 2] - currentVert;
				delta2 = treeVertices[i + 1] - currentVert;
				delta3 = treeVertices[i - 1] - currentVert;
			}
else {

			treeNormals[i] = (normalize(cross(delta1, delta2)) + normalize(cross(delta2, delta3)) + normalize(cross(delta3, delta4)))/3.0;
		}
		// i is odd
		else {

			if ((i + 2) == NUM_OF_TREE_VERTICES) {
				int indexPlus2 = 0;
				delta1 = treeVertices[indexPlus2] - currentVert;

				delta2 = treeVertices[i + 1] - currentVert;
				delta3 = treeVertices[i - 1] - currentVert;
				delta4 = treeVertices[i - 2] - currentVert;
			}		
			else if ((i + 1) == NUM_OF_TREE_VERTICES) {
				int indexPlus1 = 0;
				delta2 = treeVertices[indexPlus1] - currentVert;

				delta1 = treeVertices[i + 2] - currentVert;				
				delta3 = treeVertices[i - 1] - currentVert;
				delta4 = treeVertices[i - 2] - currentVert;
			}
			else if ((i - 1) == -1) {
				int indexMinus1 = NUM_OF_TREE_VERTICES - 1;
				delta3 = treeVertices[indexMinus1] - currentVert;

				delta1 = treeVertices[i + 2] - currentVert;
				delta2 = treeVertices[i + 1] - currentVert;
				delta4 = treeVertices[i - 2] - currentVert;
			}
			else if ((i - 2) == -2) {
				int indexMinus2 = NUM_OF_TREE_VERTICES - 2;
				delta1 = treeVertices[indexMinus2] - currentVert;

				delta1 = treeVertices[i + 2] - currentVert;
				delta2 = treeVertices[i + 1] - currentVert;
				delta3 = treeVertices[i - 1] - currentVert;
			}
			else {

				treeNormals[i] = treeNormals[indexMinus1];
			}
		}
	}

}

	// Brute force vectors now 0-15
/*	vec3 treeVerticesVec01 = treeVertices[1] - treeVertices[0];
	vec3 treeVerticesVec10 = -treeVerticesVec01;
	vec3 treeVerticesVec02 = treeVertices[2] - treeVertices[0];
	vec3 treeVerticesVec20 = treeVerticesVec02;
	vec3 treeVerticesVec015 = treeVertices[15] - treeVertices[0];


	vec3 treeVerticesVec03 = treeVertices[3] - treeVertices[0];
	vec3 treeVerticesVec30 = treeVerticesVec03;
	vec3 treeVerticesVec12 = treeVertices[2] - treeVertices[1];
	vec3 treeVerticesVec21 = -treeVerticesVec12;
	vec3 treeVerticesVec13 = treeVertices[3] - treeVertices[1];
	vec3 treeVerticesVec31 = -treeVerticesVec13;
	vec3 treeVerticesVec23 = treeVertices[3] - treeVertices[2];
	vec3 treeVerticesVec32 = -treeVerticesVec23;
	vec3 treeVerticesVec34 = treeVertices[4] - treeVertices[3];
	vec3 treeVerticesVec43 = -treeVerticesVec34;
	vec3 treeVerticesVec24 = treeVertices[4] - treeVertices[2];
	vec3 treeVerticesVec42 = -treeVerticesVec24;

	// Create normals 
	treeNormals[0] = normalize(cross(treeVerticesVec01, treeVerticesVec02));
	treeNormals[1] = (normalize(cross(treeVerticesVec13, treeVerticesVec12)) + normalize(cross(treeVerticesVec12, treeVerticesVec10)))/2.0;
	treeNormals[2] = (normalize(cross(treeVerticesVec20, treeVerticesVec21)) + normalize(cross(treeVerticesVec21, treeVerticesVec23)) + normalize(cross(treeVerticesVec23, treeVerticesVec24)))/3.0;
	treeNormals[3] = (normalize(cross(treeVerticesVec34, treeVerticesVec32)) + normalize(cross(treeVerticesVec32, treeVerticesVec31)))/2.0;
	treeNormals[4] = normalize(cross(treeVerticesVec42, treeVerticesVec43));





/*
void rotateTree(inout vec3 treeVertices[NUM_OF_TREE_VERTICES], in float theta, in vec3 position, in float height, in float treeNoise)
{
	 //Rotate Tree in origo and add translation and height 
	//mat3 rotationMatrix = mat3(cos(theta), 0.0, sin(theta), 0.0, 1.0, 0.0, -sin(theta), 0.0, cos(theta));
	//float wind = 0.15*cos(u_Wind + treeNoise*7.0);	

	for (int i = 0; i < NUM_OF_TREE_VERTICES; ++i) {
		//treeVertices[i] = rotationMatrix*treeVertices[i];
		//treeVertices[i].xz += treeVertices[i].y * wind;
		treeVertices[i] += position;
		treeVertices[i].y += height;
	}
}
*/
