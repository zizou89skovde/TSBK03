#version 400

#define NUM_OF_GRASS_VERTICES 5
#define NUM_OF_GRASS_STRAWS_PER_TRIANGLE 5

#define PI 3.141592653589793

layout (triangles, invocations = NUM_OF_GRASS_STRAWS_PER_TRIANGLE) in;
layout (triangle_strip, max_vertices = NUM_OF_GRASS_VERTICES) out;

uniform sampler2D u_GrassMask;
uniform sampler2D u_GrassNoise;

uniform mat4 VP_Matrix;
uniform mat4 V_Matrix;
uniform mat4 P_Matrix;

uniform float u_Wind;

in vec2 g_TextureCoord[3];
in float g_Height[3];

out vec3 f_Normal;


//void grassInvocationDependentPosition(inout vec3 position, in float grassNoise);
void createGrassStraw1(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float grassNoise);
void createGrassStraw2(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float grassNoise);
void createNormals(inout vec3 grassNormals[NUM_OF_GRASS_VERTICES], in vec3 grassVertices[NUM_OF_GRASS_VERTICES]);
void getPosition(inout vec3 position, inout float height, inout vec2 texCoord);
void rotateGrass(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float theta, in vec3 position, in float height, in float grassNoise);

void main()
{
	/* Get  position in a triangle (In world coordinates) */
	vec3 position = vec3(0.0);
	float height = 0;
	vec2 texCoord = vec2(0.0);
	getPosition(position, height, texCoord);

	/* Read grass mask from texture, returns value between 0 and 1 */
	float grassMask = texture(u_GrassMask, texCoord).x;

	/* Read grass noise from texture, returns value between 0 and 1 */
	float grassNoise = texture(u_GrassNoise, texCoord).x;

	/* Create grass straw */
	vec3 grassVertices[NUM_OF_GRASS_VERTICES] = vec3[](vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));

	/* Invocation specifc grass position in one triangle primitive */
	switch (gl_InvocationID) {
		case 0:
			createGrassStraw1(grassVertices, grassNoise);
			break;
		case 1:
			position.x += 4.3*grassNoise;
			createGrassStraw1(grassVertices, grassNoise);
			break;
		case 2:
			position.z += 4.7*grassNoise;
			createGrassStraw1(grassVertices, grassNoise);
			break;
		case 3:
			position.x -= 3.1*grassNoise;
			createGrassStraw2(grassVertices, grassNoise);
			break;
		case 4:
			position.z -= 3.8*grassNoise;
			createGrassStraw2(grassVertices, grassNoise);
			break;
		default:
			break;
	}

	/* Rotate grass with angle theta */
	float angle =  5.0*grassNoise*PI;
	rotateGrass(grassVertices, angle, position, height, grassNoise);

	/* Create normals*/
	vec3 grassNormals[NUM_OF_GRASS_VERTICES] = vec3[](vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
	createNormals(grassNormals, grassVertices);

	/* Transform grass to screen */
	for (int i = 0; i < NUM_OF_GRASS_VERTICES; ++i) {
		vec4 fragPos = VP_Matrix*vec4(grassVertices[i], 1.0); 
	
		f_Normal = grassNormals[i];
		if (grassMask < 0.5) {
			fragPos.w = -1.0;
		}
		gl_Position = fragPos;
	
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

void createGrassStraw1(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float grassNoise)
{
	/* Define grass straw 1 properties */
	float grassBaseWidth = 0.00001 + 0.025*grassNoise;
	float grassDeltaHeight = 0.005 + 0.75*grassNoise;
	float grassDeltaZLast = 0.001 + 0.25*grassNoise;
	float grassDeltaZ = 0.001 + 0.05*grassNoise;

	grassVertices[0] = vec3(0.0);
	grassVertices[1] = vec3(0.01*grassNoise, 0.0, grassDeltaZ);
	grassVertices[2] = vec3(0.001*grassNoise, grassDeltaHeight, grassDeltaZ);
	grassVertices[3] = vec3(grassBaseWidth + 0.001*grassNoise, grassDeltaHeight*2.0, grassDeltaZ);
	grassVertices[4] = vec3(0.001*grassNoise, grassDeltaHeight*3.0, grassDeltaZLast);
}

void createGrassStraw2(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float grassNoise)
{
	/* Define grass straw 2 properties */
	float grassBaseWidth = 0.00001 + 0.025*grassNoise;
	float grassDeltaHeight = 0.005 + 0.75*grassNoise;
	float grassDeltaZLast = 0.001 + 0.35*grassNoise;
	float grassDeltaZ = 0.001 + 0.05*grassNoise;

	grassVertices[0] = vec3(0.0);
	grassVertices[1] = vec3(-10.0*grassBaseWidth, grassDeltaHeight, 0.0);
	grassVertices[2] = vec3(0.0, grassDeltaHeight/10.0, 0.0);
	grassVertices[3] = vec3(grassBaseWidth/4.0, 0.0, 0.0);
	grassVertices[4] = vec3(10.0*grassBaseWidth, grassDeltaHeight*1.7/4.0, -2.0*grassDeltaZ);
	
}

void createNormals(inout vec3 grassNormals[NUM_OF_GRASS_VERTICES], in vec3 grassVertices[NUM_OF_GRASS_VERTICES])
{
	/* Brute force vectors */
	vec3 grassVerticesVec01 = grassVertices[1] - grassVertices[0];
	vec3 grassVerticesVec10 = -grassVerticesVec01;
	vec3 grassVerticesVec02 = grassVertices[2] - grassVertices[0];
	vec3 grassVerticesVec20 = grassVerticesVec02;
	vec3 grassVerticesVec03 = grassVertices[3] - grassVertices[0];
	vec3 grassVerticesVec30 = grassVerticesVec03;
	vec3 grassVerticesVec12 = grassVertices[2] - grassVertices[1];
	vec3 grassVerticesVec21 = -grassVerticesVec12;
	vec3 grassVerticesVec13 = grassVertices[3] - grassVertices[1];
	vec3 grassVerticesVec31 = -grassVerticesVec13;
	vec3 grassVerticesVec23 = grassVertices[3] - grassVertices[2];
	vec3 grassVerticesVec32 = -grassVerticesVec23;
	vec3 grassVerticesVec34 = grassVertices[4] - grassVertices[3];
	vec3 grassVerticesVec43 = -grassVerticesVec34;
	vec3 grassVerticesVec24 = grassVertices[4] - grassVertices[2];
	vec3 grassVerticesVec42 = -grassVerticesVec24;

	/* Create normals */
	if (gl_InvocationID == 0 || gl_InvocationID == 1 || gl_InvocationID == 2) {
		grassNormals[0] = normalize(cross(grassVerticesVec01, grassVerticesVec02));
		grassNormals[1] = (normalize(cross(grassVerticesVec13, grassVerticesVec12)) + normalize(cross(grassVerticesVec12, grassVerticesVec10)))/2.0;
		grassNormals[2] = (normalize(cross(grassVerticesVec20, grassVerticesVec21)) + normalize(cross(grassVerticesVec21, grassVerticesVec23)) + normalize(cross(grassVerticesVec23, grassVerticesVec24)))/3.0;
		grassNormals[3] = (normalize(cross(grassVerticesVec34, grassVerticesVec32)) + normalize(cross(grassVerticesVec32, grassVerticesVec31)))/2.0;
		grassNormals[4] = normalize(cross(grassVerticesVec42, grassVerticesVec43));
	}
	else {
		grassNormals[0] = (normalize(cross(grassVerticesVec03, grassVerticesVec02)) + normalize(cross(grassVerticesVec02, grassVerticesVec01)))/2.0;		
		grassNormals[1] = normalize(cross(grassVerticesVec10, grassVerticesVec12));
		grassNormals[2] = (normalize(cross(grassVerticesVec21, grassVerticesVec20)) + normalize(cross(grassVerticesVec20, grassVerticesVec23)) + normalize(cross(grassVerticesVec23, grassVerticesVec24)))/3.0;
		grassNormals[3] = (normalize(cross(grassVerticesVec34, grassVerticesVec32)) + normalize(cross(grassVerticesVec32, grassVerticesVec30)))/2.0;
		grassNormals[4] = normalize(cross(grassVerticesVec42, grassVerticesVec43));
	}
}

void rotateGrass(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float theta, in vec3 position, in float height, in float grassNoise)
{
	/* Rotate grass in origo and add translation and height */
	mat3 rotationMatrix = mat3(cos(theta), 0.0, sin(theta), 0.0, 1.0, 0.0, -sin(theta), 0.0, cos(theta));
	float wind = 0.15*cos(u_Wind + grassNoise*7.0);	

	for (int i = 0; i < NUM_OF_GRASS_VERTICES; ++i) {
		grassVertices[i] = rotationMatrix*grassVertices[i];
		grassVertices[i].xz += grassVertices[i].y * wind;
		grassVertices[i] += position;
		grassVertices[i].y += height;
	}
}

