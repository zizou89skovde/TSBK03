#version 150
in  vec3 in_Position;

uniform mat4 MV_Matrix;
uniform mat4 MVP_Matrix;

void main(void)
{
	gl_Position = MVP_Matrix * vec4(in_Position.xyz, 1.0); 
}

