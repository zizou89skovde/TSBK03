#version 150
out vec4 outColor;
in vec3 f_Normal;
void main()
{
	vec3 light = vec3(0.2,1.0,0.0);
	float diffuse  = abs(dot(normalize(light),normalize(f_Normal)));
	float ambient = 0.4;
    outColor = (ambient+(1.0-ambient)*diffuse)*vec4(0.2, 0.6, 0.1, 1.0);
}
