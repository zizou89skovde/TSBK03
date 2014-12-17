#version 150

out vec4 out_Color;
uniform sampler2D u_Texture;
in vec2 f_TextureCoord;
void main(void)
{
	out_Color = 0.1*texture(u_Texture, f_TextureCoord);
}

