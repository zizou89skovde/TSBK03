#version 430 core

uniform sampler2D u_HeightMap;
uniform vec2 u_TextureSize;
uniform vec3 u_TerrainSize;
uniform mat4 MVP_Matrix;



out VS_OUT
{
    vec2 tc;
} vs_out;

void main(void)
{

    const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
                                   vec4( 0.5, 0.0, -0.5, 1.0),
                                   vec4(-0.5, 0.0,  0.5, 1.0),
                                   vec4( 0.5, 0.0,  0.5, 1.0));

    int x = int(mod(gl_InstanceID, u_TextureSize.x));
    int y = int(gl_InstanceID / u_TextureSize.x);
    vec2 offs = vec2(x, y);

	//p.y += texture(u_HeightMap, tc).r * 0.0;
	//float height = texture(u_HeightMap,in_Position.xy).x;


    vs_out.tc = (vertices[gl_VertexID].xz + offs + vec2(0.5))/u_TextureSize;
    //gl_Position = MVP_Matrix*(vertices[gl_VertexID] + vec4(float(x - u_TextureSize.x/2.0), 0.0, float(y - u_TextureSize.y/2.0), 0.0));


	float height = texture(u_HeightMap, vs_out.tc).x; //vec2(1.0-in_Position.xxxxxxx,1.0-in_Position.y)
	// Scale and bias
	height = (height-0.5)*50.0; //u_GridHeightScale;

	gl_Position = MVP_Matrix*(vertices[gl_VertexID] + vec4(float(x - u_TextureSize.x/2.0), height, float(y - u_TextureSize.y/2.0), 0.0));
}


