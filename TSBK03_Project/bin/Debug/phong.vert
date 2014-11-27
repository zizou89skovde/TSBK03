#version 150

in  vec3 in_Position;
in  vec3 in_Normal;
out vec3 exNormal; // Phong
out vec3 exSurface; // Phong (specular)
out float exHeight;
uniform mat4 MV_Matrix;
uniform mat4 MVP_Matrix;

void main(void)
{
	exNormal = inverse(transpose(mat3(V_Matrix))) * in_Normal; // Phong, "fake" normal transformation
ASD
	exSurface = vec3(MV_Matrix * vec4(in_Position, 1.0)); // Don't include projection here - we only want to go to view coordinates
	exHeight = in_Position.y;
	gl_Position = VP_Matrix * vec4(in_Position, 1.0); // This should include projection
}
