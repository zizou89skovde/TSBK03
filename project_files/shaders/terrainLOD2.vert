#version 430 core

uniform sampler2D u_HeightMap;

in vec3 in_Position;
in vec2 in_TextureCoord;


void main(void)
{
	float height = texture(u_HeightMap,(in_Position.xz/20.0+1.0)/2.0).r*5.0;
	gl_Position = vec4(in_Position,1.0)+vec4(0.0,height,0.0,0.0);
}


