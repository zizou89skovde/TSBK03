#version 430

out vec4 out_Color;
in vec3 g_Normal;
//in vec3 f_Normal;

void main(void)
{
	vec3 worldLightDirection = normalize(vec3(0.0,1.0,0.0));
	float diffuse = max(dot(g_Normal,worldLightDirection),0.0);
	out_Color = (diffuse+0.1)*vec4(0.4, 0.2, 0.0, 1.0);
}

