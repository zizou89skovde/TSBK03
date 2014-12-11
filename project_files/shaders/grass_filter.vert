#version 150

in  vec3 in_Position;

uniform float u_GridSize;
uniform vec3 u_GridOffset;



void main(void)
{
	/* Set out texture coordinate */
	g_TextureCoord = in_Position.xy;
	
	/* Compute world coordinates */
	vec3 position = vec3(in_Position.x,0.0,in_Position.y)*u_GridSize + u_GridOffset;
	
	gl_Position = vec4(position,1.0);
}

