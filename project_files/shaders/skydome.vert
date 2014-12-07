#version 150
out float gl_ClipDistance[1];

in  vec3 in_Position;
in  vec2 in_TextureCoord;
out vec2 f_TextureCoord;
uniform mat4 V_Matrix;
uniform mat4 P_Matrix;
uniform float u_Flip;

void main(void)
{

	f_TextureCoord = in_TextureCoord;
	vec3 position = 8.0*in_Position.xyz;
	gl_ClipDistance[0] = 1.0;
	if(u_Flip  < -0.5){
		vec3 plane = vec3(0.0,1.0,0.0);
		gl_ClipDistance[0] = dot(plane,position);
	}
	position.y *= u_Flip;
	
	position  = mat3(V_Matrix)*position;
	gl_Position    = P_Matrix * vec4(position, 1.0); 
}

