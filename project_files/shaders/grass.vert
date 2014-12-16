#version 150
in  vec3 in_Position;

uniform mat4 MV_Matrix;

uniform sampler2D u_HeightMap;
uniform float u_GridHeightScale;

uniform float u_GridSize;
uniform vec3 u_GridOffset;

out vec2 g_TextureCoord;
out float g_Height;

void main(void)
{

/*
	// Set out texture coordinate
	g_TextureCoord = in_Position.xy;
	
	// Compute world coordinates
	vec3 position = vec3(in_Position.x,0.0,in_Position.z)*u_GridSize + u_GridOffset;
	
	// Read vertex y-value (height) from heightmap
	float height = texture(u_HeightMap,in_Position.xz).x; 
	
	// Scale and bias
	g_Height = (height-0.5)*u_GridHeightScale;

	// Output to next shader
	gl_Position = vec4(position,1.0);

*/	
	vec4 position = vec4(in_Position,1.0);
	position.y  = texture(u_HeightMap,in_Position.xz).r;
	gl_Position = position;
	
}

