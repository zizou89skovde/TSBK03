// TO TESSELATION SHADER
/*
#version 410 core   
in  vec3 in_Position;
in  vec3 in_Normal;
out vec3 tcs_Position;
//uniform mat4 MV_Matrix;
void main(void)
{
	
	
	tcs_Position 		= in_Position + in_Normal*0.0;
}
*/
/*



// TO FRAGMENT SHADER
#version 410 core   
uniform mat4 MVP_Matrix;
uniform mat4 MV_Matrix;


in  vec4 in_Position;
in  vec3 in_Normal;
out  vec3 f_Normal;
out  vec3 f_Position;
out  vec3 f_LightPos;


void main(void)
{
	vec3 light = vec3(0.0, 8.0, -8.0);
	mat4 normalMatrix = transpose(inverse(MV_Matrix));
	f_Position = vec3(MV_Matrix*vec4(in_Position.xyz,1.0)) + in_Normal*0.0;
	f_Normal=mat3(normalMatrix)*in_Normal; 
	f_LightPos = vec3(normalMatrix*vec4(light,1.0));
	gl_Position = MVP_Matrix * vec4(in_Position.xyz, 1.0); 
   
}
*/


#version 150// core   
// TO GEOMERTY SHADER


uniform sampler2D texUnit;
in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 g_Normal;
out vec3 g_Position;
out vec3 g_LightPos;

uniform mat4 V_Matrix;

uniform vec2 u_Resolution;

vec3 readPositionWorld(vec2 tcoord,vec2 offset){
	//Add offset to tex coord, and clamp. 
	vec2 textureCoordinate =  clamp(tcoord+offset,0.0,0.99);
	return texture(texUnit, textureCoordinate).xyz;
}

vec3 getNormalWorld(vec2 texCoord,vec3 centerPos){
	int numTriangles = 6;
	vec2 resolution = u_Resolution;
	float step = 1.0/resolution.x;
	vec3 deltaVec[6];
	deltaVec[0] = readPositionWorld(texCoord,vec2(step,0.0)) - centerPos; // EAST
	deltaVec[1] = readPositionWorld(texCoord,vec2(step,step)) - centerPos; // North East
	deltaVec[2] = readPositionWorld(texCoord,vec2(0.0,step)) - centerPos; // North 
	deltaVec[3] = readPositionWorld(texCoord,vec2(-step,0.0)) - centerPos; // West 
	deltaVec[4] = readPositionWorld(texCoord,vec2(-step,-step)) - centerPos; // South West 
	deltaVec[5] = readPositionWorld(texCoord,vec2(0.0,-step)) - centerPos; // South  
	
	//Normal
	vec3 normal = vec3(0.0);
	
	//Normal calutlated: sum(plane normal of all the triangles connected to the current vertex) / numTriangle 
	
	for(int i = 0; i < numTriangles; ++i){
		int index1 = i;
		int index2 = int(mod(i+1,numTriangles));
		normal += normalize(cross(deltaVec[index1],deltaVec[index2]));
	}
	normal /= float(numTriangles)+in_Normal.x*0.0;
	
	return normal;

}

void main(void)
{
	vec2 texCoord = in_Position.xy;	
	vec4 light = vec4(0.0, 8.0, 8.0,1.0); 	
	vec3 centerPos = readPositionWorld(texCoord,vec2(0.0));
	mat4 normalMatrix = transpose(inverse(V_Matrix));
	
    g_Normal		= mat3(normalMatrix)*getNormalWorld(texCoord,centerPos); 
	g_LightPos 		= vec3(V_Matrix*light);
	gl_Position = vec4(in_Position.xyz, 1.0); 	
}

/*
// TO GEOMERTY SHADER

#version 410 core   

uniform sampler2D texUnit;
in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 g_Normal;
out vec3 g_Position;
out vec3 g_LightPos;

uniform mat4 MV_Matrix;

uniform vec2 u_Vec2;
void main(void)
{

	vec3 someOffset = vec3(0.0,3.5,0.0);

	vec2 texCoord = in_Position.xy;
	vec3 position = texture(texUnit,clamp(texCoord,0.0,0.99)).xyz;
	position += someOffset;
	
	vec2 resolution = u_Vec2;
	float step = 1.0/resolution.x;
	vec3 posWest  = texture(texUnit, clamp(texCoord +vec2(step,0.0),0.0,0.99)).xyz;
	vec3 posEast  = texture(texUnit, clamp(texCoord +vec2(-step,0.0),0.0,0.99)).xyz;
	vec3 posNorth = texture(texUnit, clamp(texCoord +vec2(0.0,step),0.0,0.99)).xyz;
	vec3 posSouth = texture(texUnit, clamp(texCoord +vec2(0.0,-step),0.0,0.99)).xyz;
	
	vec3 delta1 = posEast - posWest + in_Normal*0.0;
	vec3 delta2 = posNorth-posSouth;
	vec3 normal = normalize(cross(delta1,delta2));

	mat4 normalMatrix = transpose(inverse(MV_Matrix));

	vec3 light = vec3(0.0, 8.0, -8.0); 	
	
	g_Position 		= vec3(MV_Matrix*vec4(in_Position,1.0)) + in_Normal*0.0;
    g_LightPos 		= vec3(normalMatrix*vec4(light,1.0));
    g_Normal		= mat3(normalMatrix)*normal; 
	gl_Position = MVP_Matrix *vec4(position.xyz, 1.0); 
	
}
*/