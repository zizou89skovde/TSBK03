#version 430 core
in vec3 in_Position;
uniform sampler2D u_HeightMap;

void main(void)
{
	vec4 position = vec4(in_Position,1.0);
	position.y  = texture(u_HeightMap,in_Position.xz).r;
	gl_Position = position;
}


