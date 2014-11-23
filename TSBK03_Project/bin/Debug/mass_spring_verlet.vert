#version 400

//uniform mat4 VP_Matrix;
in  vec3 in_Position;
in  vec2 in_TextureCoord;
out vec2 f_TexCoord;
void main(void)
{
	f_TexCoord = vec2(in_TextureCoord.x,in_TextureCoord.y) ;
	gl_Position = vec4(in_Position,1.0);
}
