#version 150

uniform sampler2D u_HeightMap;
uniform float u_GridHeightScale;

uniform float u_GridSize;
uniform vec3 u_GridOffset;

in vec3 in_Position;

out vec2 g_TextureCoord;
out float g_Height;

void main(void)
{
	/* Set out texture coordinate */
	g_TextureCoord = in_Position.xy;
	
	/* Compute world coordinates */
	vec3 position = in_Position; //vec3(in_Position.x,0.0,in_Position.y); //*u_GridSize + u_GridOffset;
	
	/* Read vertex y-value (height) from heightmap */
	float height = texture(u_HeightMap,in_Position.xy).x; //vec2(1.0-in_Position.xxxxxxx,1.0-in_Position.y)
	/* Scale and bias */
	g_Height = (height-0.5)*u_GridHeightScale;

	gl_Position = vec4(position,1.0);
}

