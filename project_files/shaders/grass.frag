#version 150
out vec4 outColor;
in vec3 f_Normal;
void main()
{
	vec3 light = vec3(1.0,0.0,2.0);
	float diffuse  = max(dot(normalize(light),normalize(f_Normal)),0.0);

    outColor = diffuse*vec4(0.2, 0.4, 0.1, 1.0);
}
