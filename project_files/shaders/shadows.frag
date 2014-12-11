#version 400
uniform sampler2D u_SceneDepth;
uniform sampler2D u_LightDepth;

uniform mat4 LightTextureMatrix;


uniform vec3 u_CameraRight;
uniform vec3 u_CameraUp;
uniform vec3 u_CameraLook;
uniform vec3 u_CameraPosition;

uniform float u_CameraNear;
uniform float u_CameraFar;

in vec2 f_TexCoord;
in vec3 f_Position;

out vec4 out_Color;

//#define DEBUG_WORLD_POSITION 
//#define DEBUG_WORLD_POSITION_Y 

/* From TSBK03 literature*/
float readSceneDepth( in vec2 coord )
{
	float zNear = u_CameraNear;
	float zFar = u_CameraFar;
	float z_from_depth_texture = texture(u_SceneDepth, coord).x;
	float z_sb = 2.0 * z_from_depth_texture - 1.0; 
	float z_world = 2.0 * zNear * zFar / (zFar + zNear - z_sb * (zFar - zNear)); 
	return z_world;
}
float readSceneDepth( in float depth )
{
	float zNear = u_CameraNear;
	float zFar = u_CameraFar;
	float z_from_depth_texture = depth;
	float z_sb = 2.0 * z_from_depth_texture - 1.0; 
	float z_world = 2.0 * zNear * zFar / (zFar + zNear - z_sb * (zFar - zNear)); 
	return z_world;
}

void main(void)
{
    float sceneDepth = readSceneDepth(f_TexCoord);
	float shadowing = 0.0;
	
	float redValueX   = 0.0;
	float greenValueZ = 0.0;
	float grayValueY  =  0.0;
	if(sceneDepth < u_CameraFar*0.9){
		
		/**  Compute ~world position of the current pixel **/
		vec3 worldPosition = u_CameraPosition + (u_CameraRight *f_Position.x + u_CameraUp * f_Position.y + u_CameraLook)*sceneDepth;
		
		//worldPosition.xyz *= 2.0;
		grayValueY = ((worldPosition.y/2.5) + 1.0)/2.0;
		redValueX = (worldPosition.x/10.0 + 1.0)/2.0;
		greenValueZ = (worldPosition.z/10.0 + 1.0)/2.0;
		
		
		// Compute projected texture coordinates //
		vec4 projectedCoordinates = LightTextureMatrix *vec4(worldPosition,1.0);
		float clip = projectedCoordinates.w;	
		projectedCoordinates /= clip;
		
		float plz = projectedCoordinates.z - 0.002;	
		
		float lightDepth = texture(u_LightDepth, projectedCoordinates.st).x;
		lightDepth = (lightDepth - 0.5)*2.0;
		
		if(clip > 0.0){
			if( lightDepth < plz)
				shadowing = 0.5;
		}
		
	}
	vec4 color = vec4(0.0);
#ifdef DEBUG_WORLD_POSITION
	color.r = redValueX;
	color.g = greenValueZ; 
	color.a = 1.0;
/*#else ifdef DEBUG_WORLD_POSITION_Y
	color = vec4(grayValueY);
	color.a = 1.0;
	#endif*/
#else
	color.a = shadowing;
#endif
    out_Color = color;

}
