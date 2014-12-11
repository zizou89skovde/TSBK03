#version 400
uniform sampler2D u_SceneDepthMap;
uniform float u_CameraNear;
uniform float u_CameraFar;
uniform float u_ScreenWidth;
uniform float u_ScreenHeight;


in vec3 f_LightPosition;
in vec3 f_Position;

out vec4 out_Color;


/* From TSBK03 literature*/
float readDepth( in vec2 coord )
{
	float zNear = u_CameraNear;
	float zFar = u_CameraFar;
	float z_from_depth_texture = texture(u_SceneDepthMap, coord).x;
	float z_sb = 2.0 * z_from_depth_texture - 1.0; 
	float z_world = 2.0 * zNear * zFar / (zFar + zNear - z_sb * (zFar - zNear)); 
	return z_world;
}

/* InScatter routine as an approximation of the light contribution
*  over a line segment. This function is Copied from 
*  http://blog.mmacklin.com/2010/05/29/in-scattering-demo/
*/
float InScatter(vec3 dir, vec3 lightPos, float d)
{
	// light to ray origin
	vec3 q = -lightPos;
	 
	// coefficients
	float b = 0.7*dot(dir, q);
	float c = 1.0*dot(q, q);
	 
	// evaluate integral
	float s = 1.0f / sqrt(c - b*b);
	float l = s * (atan( (d + b) * s) - atan( b*s ));
	return l;

}

void main(void)
{
	vec2 textureCoords = (gl_FragCoord.xy+0.5)/u_ScreenWidth;
    float sceneDepth = readDepth(textureCoords);
	
	/* Distance from eye to current position in the light frustum */
    float distance = length(f_Position);
	
	/* Need to check if something  is blocking the view of the light frustum
	   If that is the case. Set distance to the depth of the scene at this position */
	distance = min(sceneDepth,distance);
	
	/* Direction to from the eye to the current position in the light frustum */
    vec3 direction = normalize(f_Position);

	
    //volumeDepth = min(volumeDepth, depthInCS); // clamp to scene depth
	/* Compute air light contribution over distance from eye to light */
    float res = 15.0*InScatter(direction,f_LightPosition,distance);

    if (gl_FrontFacing) {
        res*=-1.0;
    }
	vec4 color = vec4(1.0);
    out_Color = color * res;

}
