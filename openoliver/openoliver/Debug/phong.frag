#version 150

out vec4 out_Color;

in vec3 frag_Normal;

void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
	float diffuse, shade;
	
	// Diffuse
	diffuse = dot(normalize(frag_Normal), light);
	diffuse = max(0.0, diffuse); // No negative light
	
	shade = diffuse;
	out_Color = vec4(shade, shade, shade, 1.0);
}
