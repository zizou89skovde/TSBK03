#version 400

//uniform mat4 VP_Matrix;
in  vec3 in_Position;
in  vec2 in_TextureCoord;
out vec2 f_TexCoord;
out vec3 f_Position;
void main(void)
{
	f_TexCoord = in_TextureCoord;
	f_Position = in_Position;
	gl_Position = vec4(in_Position,1.0);
}
