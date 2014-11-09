#version 150

in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 frag_Normal;
out float frag_Height;
uniform mat4 MV_Matrix;
uniform mat4 MVP_Matrix;

void main(void)
{
	frag_Height = in_Position.y;
	gl_Position = MVP_Matrix * vec4(in_Position, 1.0); // This should include projection
	frag_Normal = in_Normal;
}
