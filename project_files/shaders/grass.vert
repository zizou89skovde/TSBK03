#version 150
in  vec3 in_Position;



uniform sampler2D u_HeightMap;
uniform float u_GridHeightScale;

uniform float u_GridSize;
uniform vec3 u_GridOffset;

out vec2 g_TextureCoord;
out float g_Height;

void main(void)
{
	/* Set out texture coordinate */
	g_TextureCoord = in_Position.xy;
	
	/* Compute world coordinates */
	vec3 position = vec3(in_Position.x,0.0,in_Position.y)*20.0 - 10.0; //u_GridSize + u_GridOffset;
	
	/* Read vertex y-value (height) from heightmap */
	float height = texture(u_HeightMap,in_Position.xy).x; //vec2(1.0-in_Position.xxxxxxx,1.0-in_Position.y)
	/* Scale and bias */
	g_Height = (height-0.5)*5.0; //u_GridHeightScale;

	gl_Position = vec4(position,1.0);
}

