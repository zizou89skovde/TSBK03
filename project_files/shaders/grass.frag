#version 150

uniform sampler2D u_GrassNoise;

out vec4 outColor;
in vec3 f_Normal;

void main()
{
	// Read grass noise from texture, returns value between 0 and 1
	float grassNoise = 5.0*texture(u_GrassNoise, gl_FragCoord.xy).x;
	
	vec3 light = vec3(0.2,1.0,0.0);
	float diffuse  = abs(dot(normalize(light),normalize(f_Normal)));
	float ambient = 0.3;
    outColor = (ambient+(1.0-ambient)*diffuse)*vec4(0.2, clamp(grassNoise, 0.2, 0.9), 0.1, 1.0);
}
