#version 400
uniform sampler2D u_SceneDepth;
uniform sampler2D u_LightDepth;

uniform mat4 MVP_LightMatrix;
uniform mat4 ViewInvMatrix;


uniform float u_CameraNear;
uniform float u_CameraFar;

uniform float u_LightNear;
uniform float u_LightFar;




in vec2 f_TexCoord;
in vec3 f_Position;

out vec4 out_Color;

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

float readLightDepth( in vec2 coord )
{
	float zNear = u_LightNear;
	float zFar = u_LightFar;
	float z_from_depth_texture = texture(u_LightDepth, coord).x;
	float z_sb = 2.0 * z_from_depth_texture - 1.0; 
	float z_world = 2.0 * zNear * zFar / (zFar + zNear - z_sb * (zFar - zNear)); 
	return z_world;
}

void main(void)
{

//	float sceneDepthProj = texture(u_SceneDepth, f_TexCoord).x;	
    float sceneDepth = readSceneDepth(f_TexCoord);

	vec3 fragPosition = vec3(f_Position.xy,1.0);
	vec3 viewPosition = fragPosition*sceneDepth;
	vec3 worldPosition = (vec4(viewPosition,1.0)*ViewInvMatrix).xyz;
	vec4 a = MVP_LightMatrix*vec4(0,0,0,0);
	worldPosition += a.xyz*0.0;
	float shadowing =(worldPosition.y/5.0 + 1.0)/2.0;
	if(sceneDepth > u_CameraFar*0.9)
		shadowing = 0.0;
	/*if(sceneDepth < u_CameraFar*0.9){
		
		vec3 fragPosition = vec3(0.5*f_Position.xy,1.0);
		vec3 viewPosition = fragPosition*sceneDepth;
		//Determine position of frustum in lights projected viewspace //
		//vec4 worldPosition = ViewInvMatrix*vec4(viewPosition,1.0);
		//worldPosition.w = 1.0;
		mat4 camToLightProj = MVP_LightMatrix*inverse(ViewInvMatrix);
		vec4 projectedCoordinates = camToLightProj*vec4(viewPosition,1.0);
		projectedCoordinates /= projectedCoordinates.w;
		vec2 texCoordLight = projectedCoordinates.xy *vec2(0.5) + vec2(0.5);
		// Compute the projected texture coordinates for the depth map sampling //
	//	vec4 projectedCoordinates = biasMat * projectLightCoord;
		float clip = projectedCoordinates.w;
		projectedCoordinates /= projectedCoordinates.w;
		
		float plz = projectedCoordinates.z - 0.002;
		
		float lightDepth = texture(u_LightDepth, texCoordLight.st).x; //readLightDepth(projectedCoordinates.xy);
		shadowing = lightDepth;
		lightDepth = (lightDepth - 0.5)*2.0;
		
		if(clip > 0.0){
			if( lightDepth < plz)
				shadowing = 0.5;
		}
	}*/
	vec4 color = vec4(1.0)*shadowing;
	color.a = 1.0;
    out_Color = color;

}
