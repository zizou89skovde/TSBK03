#version 150

in vec2 f_TexCoord;
uniform sampler2D u_Texture;
out vec4 out_Color;

void main(void)
{
    out_Color = texture(u_Texture, f_TexCoord);
}
