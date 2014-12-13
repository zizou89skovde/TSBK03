
#version 150// core   

uniform sampler2D texUnit;
in  vec3 in_Position;
uniform mat4 VP_Matrix;

void main(void)
{

	/* Read buffer position */
	vec2 texCoord = in_Position.xy;	
		
	/* Read data for current vertex */
	vec4 centerElement =  texture(texUnit, clamp(texCoord,0.0,0.99));
	vec3 centerPos = centerElement.xyz;

	gl_Position 	= VP_Matrix*vec4(centerPos, 1.0); 	
}

