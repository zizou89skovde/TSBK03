
#version 150// core   

uniform sampler2D texUnit;
in  vec3 in_Position;

out vec3 f_Normal;
out vec3 f_Position;
out vec3 f_LightPos;

uniform mat4 V_Matrix;
uniform mat4 VP_Matrix;

uniform vec2 u_Resolution;

vec3 readPositionWorld(vec2 tcoord,vec2 offset){
	//Add offset to tex coord, and clamp. 
	vec2 textureCoordinate =  clamp(tcoord+offset,0.0,0.99);
	return texture(texUnit, textureCoordinate).xyz;
}

vec3 getNormalWorld(vec2 texCoord,vec3 centerPos){
	int numTriangles = 6;
	vec2 resolution = u_Resolution;
	float step = 1.0/(resolution.x);
	vec3 deltaVec[6];
	deltaVec[0] = readPositionWorld(texCoord,vec2(step,0.0)) - centerPos; // EAST
	deltaVec[1] = readPositionWorld(texCoord,vec2(0.0,step)) - centerPos; // North
	deltaVec[2] = readPositionWorld(texCoord,vec2(-step,step)) - centerPos; // North 
	deltaVec[3] = readPositionWorld(texCoord,vec2(-step,0.0)) - centerPos; // West 
	deltaVec[4] = readPositionWorld(texCoord,vec2(0.0,-step)) - centerPos; // South West 
	deltaVec[5] = readPositionWorld(texCoord,vec2(step,-step)) - centerPos; // South  

	vec3 normal = vec3(0.0);
	
	/* Normal calutlated: sum(plane normal of all the triangles connected to the current vertex) / numTriangle */
	for(int i = 0; i < numTriangles; ++i){
		int index1 = i;
		int index2 = int(mod(i+1,numTriangles));
		normal += normalize(cross(deltaVec[index1],deltaVec[index2]));
	}
	normal /= float(numTriangles);
	
	return normal;

}

void main(void)
{
	/* Calculate normal matrix */
	mat4 normalMatrix = transpose(inverse(V_Matrix));
	/* Read buffer position */
	vec2 texCoord = in_Position.xy;	
	
	/* Hard coded light position, To be uniform */
	vec4 light = vec4(0.0, 12.0, -8.0,1.0); 	
	
	/* Read data for current vertex */
	vec4 centerElement =  texture(texUnit, clamp(texCoord,0.0,0.99));
	vec3 centerPos = centerElement.xyz;
	
	/* Set output variables */	
    f_Normal		= mat3(normalMatrix)*getNormalWorld(texCoord,centerPos); 
	f_LightPos 		= vec3(V_Matrix*light);
	
	/* Quick fix */
	centerPos 		+= vec3(0.0,7.0,0.0);
	f_Position 		= vec3(V_Matrix*vec4(centerPos,1.0));
	gl_Position 	= VP_Matrix*vec4(centerPos, 1.0); 	
}
