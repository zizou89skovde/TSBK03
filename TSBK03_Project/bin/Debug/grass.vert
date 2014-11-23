in  vec3 in_Position;
out vec3 out_Position;

uniform sampler2D texUnit;
uniform float u_Wind;
void main(void)
{
	vec2 texCoord = in_Position.xy;
	gl_Position = texture(texUnit, texCoord);
}

