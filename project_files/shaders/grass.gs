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


void grassInvocationDependentPosition(inout vec3 position, in float grassNoise);
void createGrassStraw(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float grassNoise);
void createNormals(inout vec3 grassNormals[NUM_OF_GRASS_VERTICES], in vec3 grassVertices[NUM_OF_GRASS_VERTICES]);
void getPosition(inout vec3 position, inout float height, inout vec2 texCoord);
void rotateGrass(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float theta, in vec3 position, in float height);

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

	/* Invocation specifc grass position in one triangle primitive */
	grassInvocationDependentPosition(position, grassNoise);

	/* Create grass straw */
	vec3 grassVertices[NUM_OF_GRASS_VERTICES] = vec3[](vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));
	createGrassStraw(grassVertices, grassNoise);

	/* Rotate grass with angle theta */
	float angle =  grassNoise*PI;
	rotateGrass(grassVertices, angle, position, height);

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

void grassInvocationDependentPosition(inout vec3 position, in float grassNoise)
{
	/* Invocation specifc grass position in one triangle primitive */
	switch (gl_InvocationID) {
		case 1:
			position.x += 5*grassNoise;
			break;
		case 2:
			position.z += 5*grassNoise;
			break;
		case 3:
			position.x -= 5*grassNoise;
			break;
		case 4:
			position.z -= 5*grassNoise;
			break;
		default:
			break;
	}
}

void createGrassStraw(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float grassNoise)
{
	/* Define grass properties */
	float grassBaseWidth = 0.00001 + 0.025*grassNoise;
	float grassDeltaHeight = 0.005 + 0.75*grassNoise;
	float grassDeltaZLast = 0.001 + 0.35*grassNoise;
	float grassDeltaZ = 0.001 + 0.05*grassNoise;

	for (int i = 0; i < NUM_OF_GRASS_VERTICES; ++i) {
		/* Special case for first vertice - On ground level */
		if (i == 0) {
			grassVertices[i] = vec3(0.0);
		}
		else if (i == NUM_OF_GRASS_VERTICES - 1) {
			grassVertices[i] = vec3(0.0, grassDeltaHeight*(i - 1), grassDeltaZLast);
		}
		/* i is odd */
		else if (mod(i, 2) == 1) {
			grassVertices[i] = vec3(grassBaseWidth, grassDeltaHeight*(i - 1), grassDeltaZ);
		}
		/* i is even */
		else {
			grassVertices[i] = vec3(0.001*grassNoise, grassDeltaHeight*(i - 1), grassDeltaZ);
		}
	}
}

void createNormals(inout vec3 grassNormals[NUM_OF_GRASS_VERTICES], in vec3 grassVertices[NUM_OF_GRASS_VERTICES])
{
	/* Brute force vectors */
	vec3 grassVerticesVec01 = grassVertices[1] - grassVertices[0];
	vec3 grassVerticesVec10 = -grassVerticesVec01;
	vec3 grassVerticesVec12 = grassVertices[2] - grassVertices[1];
	vec3 grassVerticesVec21 = -grassVerticesVec12;
	vec3 grassVerticesVec02 = grassVertices[2] - grassVertices[0];
	vec3 grassVerticesVec20 = grassVerticesVec02;
	vec3 grassVerticesVec23 = grassVertices[3] - grassVertices[2];
	vec3 grassVerticesVec32 = -grassVerticesVec23;
	vec3 grassVerticesVec31 = grassVertices[3] - grassVertices[1];
	vec3 grassVerticesVec13 = -grassVerticesVec31;
	vec3 grassVerticesVec34 = grassVertices[4] - grassVertices[3];
	vec3 grassVerticesVec43 = -grassVerticesVec34;
	vec3 grassVerticesVec24 = grassVertices[4] - grassVertices[2];
	vec3 grassVerticesVec42 = -grassVerticesVec24;
	
	/* Create normals */
	grassNormals[0] = normalize(cross(grassVerticesVec01, grassVerticesVec02));
	grassNormals[1] = (normalize(cross(grassVerticesVec13, grassVerticesVec12)) + normalize(cross(grassVerticesVec12, grassVerticesVec10)))/2.0;
	grassNormals[2] = (normalize(cross(grassVerticesVec20, grassVerticesVec21)) + normalize(cross(grassVerticesVec21, grassVerticesVec23)) + normalize(cross(grassVerticesVec23, grassVerticesVec24)))/3.0;
	grassNormals[3] = (normalize(cross(grassVerticesVec34, grassVerticesVec32)) + normalize(cross(grassVerticesVec32, grassVerticesVec31)))/2.0;
	grassNormals[4] = normalize(cross(grassVerticesVec42, grassVerticesVec43));
}

void rotateGrass(inout vec3 grassVertices[NUM_OF_GRASS_VERTICES], in float theta, in vec3 position, in float height)
{
	/* Rotate grass in origo and add translation and height */
	mat3 rotationMatrix = mat3(cos(theta), 0.0, sin(theta), 0.0, 1.0, 0.0, -sin(theta), 0.0, cos(theta));

	for (int i = 0; i < NUM_OF_GRASS_VERTICES; ++i) {
		grassVertices[i] = rotationMatrix*grassVertices[i];
		grassVertices[i] += position;
		grassVertices[i].y += height;
	}
}

