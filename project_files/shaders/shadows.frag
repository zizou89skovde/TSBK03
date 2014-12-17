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

uniform float u_LightFar;
uniform float u_LightNear;

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
float readLightDepth( in float depth )
{
	float zNear = u_LightNear;
	float zFar = u_LightFar;
	float z_from_depth_texture = depth;
	float z_sb = 2.0 * z_from_depth_texture - 1.0; 
	float z_world = 2.0 * zNear * zFar / (zFar + zNear - z_sb * (zFar - zNear)); 
	return z_world;
}

float smoothShadow(vec2 texCoord){

	float res = 0.0;
	for(int i = 0; i < 4; i++){
	
		float ds  = float(i%2)- 0.5;
		float dt  = float(i/2)- 0.5;
		
		vec2 t = texCoord + vec2(ds,dt)/512.0;
		
		res +=texture(u_LightDepth, t).x;
		
	}
	res /= 4.0;
	return res;
	
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
#ifdef DEBUG_WORLD_POSITION 
		//worldPosition.xyz *= 2.0;
		grayValueY = ((worldPosition.y/2.5) + 1.0)/2.0;
		redValueX = (worldPosition.x/10.0 + 1.0)/2.0;
		greenValueZ = (worldPosition.z/10.0 + 1.0)/2.0;
#endif
		
		// Compute projected texture coordinates //
		vec4 projectedCoordinates = LightTextureMatrix *vec4(worldPosition,1.0);
		float clip = projectedCoordinates.w;	
		projectedCoordinates /= clip;
		
		float plz = projectedCoordinates.z - 0.02;	
		
		float lightDepth 	  = texture(u_LightDepth, projectedCoordinates.st).x;//smoothShadow(projectedCoordinates.st); //texture(u_LightDepth, projectedCoordinates.st).x;
		float lightDepthWorld = readLightDepth(lightDepth);
		
		lightDepth = (lightDepth - 0.5)*2.0;
		
		
		
		if(clip > 0.0){
			if( lightDepth < plz)
				shadowing = 0.5;
		}
		
		/* Shadow appear at a certain distance **/
		shadowing *= clamp(u_LightFar*0.8-lightDepthWorld,0.0,1.0);
		
		/** Clip shadowing area as the shape of a cone **/
		shadowing *= (1.0 - 5.0*length(projectedCoordinates.st - vec2(0.5)));
		
	}
	vec4 shadowColor = vec4(0.0);
#ifdef DEBUG_WORLD_POSITION
	shadowColor.r = redValueX;
	shadowColor.g = greenValueZ; 
	shadowColor.a = 1.0;
/*#else ifdef DEBUG_WORLD_POSITION_Y
	color = vec4(grayValueY);
	color.a = 1.0;
	#endif*/
#else
	shadowColor.a = shadowing;
#endif
    out_Color = shadowColor;

}
