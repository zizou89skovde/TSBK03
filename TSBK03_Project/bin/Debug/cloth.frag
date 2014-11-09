#version 150

out vec4 out_Color;

in vec3 v_Normal;
in float v_Height;
void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
	float diffuse, shade;
	
	// Diffuse
	diffuse = dot(normalize(v_Normal), light);
	diffuse = max(0.0, diffuse); // No negative light
	
	
	float h = v_Height/25.0;
	
	shade = diffuse;
	out_Color = vec4(1.0, 0, 0, 1.0);
}
