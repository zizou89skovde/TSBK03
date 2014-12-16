#version 150


uniform sampler2D texUnit;
in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 v_Normal;
out vec3 v_Position;
out vec3 v_LightPos;
uniform mat4 MV_Matrix;
uniform mat4 MVP_Matrix;
uniform mat4 VP_Matrix;
uniform mat4 V_Matrix;

void main(void)
{

	vec3 light = vec3(0.0, -5.0, -6.0); 
	mat4 normalMatrix = transpose(inverse(V_Matrix));
	v_Position 		= vec3(V_Matrix*vec4(in_Position,1.0));
    v_LightPos 		= vec3(V_Matrix*vec4(light,1.0));
    v_Normal		= mat3(normalMatrix)*in_Normal; 
	
	gl_Position = VP_Matrix * vec4(in_Position.xyz, 1.0); 
	
}
