#version 150
in  vec3 in_Position;



uniform sampler2D u_HeightMap;
uniform float u_GridHeightScale;

uniform float u_GridSize;
uniform vec3 u_GridOffset;

void main(void)
{

	
	
	/* Compute world coordinates */
	vec3 position = vec3(in_Position.x,0.0,in_Position.y)*u_GridSize + u_GridOffset;
	
	/* Read vertex y-value (height) from heightmap */
	float height = texture(u_HeightMap,in_Position.xy).x;
	/* Scale and bias */
	height = (height-0.5)*2.0*u_GridHeightScale;
	
	/* Set y position */
	position.y = height;
	
	gl_Position = vec4(position,1.0);
}

