#version 150

in vec3 in_Position;
in vec2 in_TextureCoord;

out vec2 f_TexCoord;

void main(void)
{
	f_TexCoord = in_TextureCoord;
	gl_Position = vec4(in_Position, 1.0);
}
