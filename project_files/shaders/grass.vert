#version 150
in  vec3 in_Position;

uniform sampler2D texUnit;
uniform float u_Wind;
void main(void)
{
	vec2 texCoord = in_Position.xy;
	float size = 1.0;
	gl_Position = vec4(size*in_Position.x,0.0,size*in_Position.y,1.0);//texture(texUnit, texCoord);
}

