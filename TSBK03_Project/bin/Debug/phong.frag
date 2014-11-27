#version 150

out vec4 outColor;
in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)
in float exHeight;
void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
	float diffuse, specular, shade;
	
	// Diffuse
	diffuse = dot(normalize(exNormal), light);
	diffuse = max(0.0, diffuse); // No negative light
	
	// Specular
	vec3 r = reflect(-light, normalize(exNormal));
	vec3 v = normalize(-exSurface); // View direction
	specular = dot(r, v);
	if (specular > 0.0)
		specular = 1.0 * pow(specular, 150.0);
	specular = max(specular, 0.0);
	shade = 1.0; //0.7*diffuse + 1.0*specular;
	
	float heightAtt = exHeight/26.0;
	
	outColor = vec4(shade, shade, shade, 1.0);
}
