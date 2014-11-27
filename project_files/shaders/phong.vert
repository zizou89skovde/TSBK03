#version 150

in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 v_Normal;
out vec3 v_Position;
out vec3 v_LightPos;
out float v_Height;
uniform mat4 MV_Matrix;
uniform mat4 MVP_Matrix;

void main(void)
{

	vec3 light = vec3(0.0, -5.0, -6.0); 
	mat4 normalMatrix = transpose(inverse(MV_Matrix));
	v_Position 		= vec3(MV_Matrix*vec4(in_Position,1.0));
    v_LightPos 		= vec3(MV_Matrix*vec4(light,1.0));
    v_Normal		= mat3(normalMatrix)*in_Normal; 
	v_Height 		= (in_Position.y+ 13.0)/26.0;
	gl_Position = MVP_Matrix * vec4(in_Position.xyz, 1.0); 
	
}
