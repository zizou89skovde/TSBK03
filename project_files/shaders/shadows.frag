#version 400
uniform sampler2D u_SceneDepth;
uniform sampler2D u_LightDepth;

uniform float u_CameraNear;
uniform float u_CameraFar;

uniform float u_LightNear;
uniform float u_LightFar;

uniform mat4 ViewInvMatrix;
uniform mat4 MVP_LightMatrix;

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

float readLightDepth( in vec2 coord )
{
	float zNear = u_LightNear;
	float zFar = u_LightFar;
	float z_from_depth_texture = texture(u_LightDepth, coord).x;
	float z_sb = 2.0 * z_from_depth_texture - 1.0; 
	float z_world = 2.0 * zNear * zFar / (zFar + zNear - z_sb * (zFar - zNear)); 
	return z_world;
}
/* TODO: Move bias mat to the CPU-code */
const mat4 biasMat =   mat4(0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0);


void main(void)
{

    float sceneDepth = readSceneDepth(f_TexCoord);
	
	vec3 fragPosition = f_Position;
	fragPosition.y = 1.0;
	vec3 viewPosition = fragPosition*sceneDepth;
	
	/* Determine position of frustum in lights projected viewspace */
	vec4 worldPosition = vec4(viewPosition,1.0) * ViewInvMatrix;
	worldPosition.w = 1.0;
	vec4 projectLightCoord = worldPosition* MVP_LightMatrix;
	
	/* Compute the projected texture coordinates for the depth map sampling */
	vec4 projectedCoordinates = biasMat * projectLightCoord;
	projectedCoordinates /= projectedCoordinates.w;
	
	float lightDepth = readLightDepth(projectedCoordinates.xy);
	float illumination = 0.0;
	if(projectLightCoord.w > 0.0)
		illumination = clamp(abs(lightDepth - sceneDepth),0.0,1.0);
    
	vec4 color = vec4(0.0);
	color.a = illumination;
    out_Color = color;

}