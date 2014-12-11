#version 400// core 
  
in  vec3 in_Position;

uniform mat4 MVP_Matrix;
uniform mat4 MV_Matrix;
uniform mat4 LightTextureMatrix;

uniform float u_LightNear;
uniform float u_LightFar;

uniform sampler2D u_LightDepthMap;


/* From TSBK03 literature*/
float readDepth( in vec2 coord )
{
	float zNear = u_LightNear;
	float zFar = u_LightFar;
	float z_from_depth_texture = texture(u_LightDepthMap, coord).x;
	float z_sb = 2.0 * z_from_depth_texture - 1.0; 
	float z_world = 2.0 * zNear * zFar / (zFar + zNear - z_sb * (zFar - zNear)); 
	return z_world;
}


void main(void)
{
	vec3 lightPostionWorld = vec3(0.0);
	vec4 position = vec4(in_Position,1.0);
	
	/* Determine position of frustum in lights projected viewspace */
	vec4 projectedCoordinates = LightTextureMatrix * position;
	
	/* Compute the projected texture coordinates for the depth map sampling */
	//vec4 projectedCoordinates = positionLightViewProj; //biasMat * positionLightViewProj;
	projectedCoordinates /= projectedCoordinates.w;
	
	/* Only far plane should be affected, near plane at z = 1.0, far plane >> near plane */
	if(in_Position.z > 1.5){
		/* Direction from vertex to light position(prp of the frustum) */
		vec3 vertToLight = (lightPostionWorld-position.xyz)/u_LightFar;
		
		/* Read depth from the map */
		float depth = readDepth(projectedCoordinates.st);
		
		/* If depth is smaller than the far plane, something is blocking the light*/
		if(depth < u_LightFar){
		
			/*Adjust far plane vertex to the position where the light is blocked */
			position.xyz -= vertToLight*(depth-u_LightFar);
		}
	}
	

	gl_Position = MVP_Matrix*position; 	
	
}


