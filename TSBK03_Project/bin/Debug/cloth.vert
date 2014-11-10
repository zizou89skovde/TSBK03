#version 150


uniform sampler2D texUnit;
in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 v_Normal;
out vec3 v_Position;
out vec3 v_LightPos;
uniform mat4 MV_Matrix;
uniform mat4 MVP_Matrix;

void main(void)
{
	vec2 texCoord = in_Position.xy;
	vec3 position = texture(texUnit,texCoord).xyz;
	
	float step = 1/16.0;
	vec3 bs = position-texture(texUnit, clamp(texCoord +vec2(step,0.0),0.0,0.99)).xyz;
	vec3 bt = position-texture(texUnit, clamp(texCoord +vec2(0.0,step),0.0,0.99)).xyz;
	
	mat4 normalMatrix = transpose(inverse(MV_Matrix));
	vec3 Ps = mat3(normalMatrix)*vec3(1.0,0.0,0.0);	
	vec3 Pt = mat3(normalMatrix)*vec3(0.0,1.0,0.0);
	vec3 n  = mat3(normalMatrix)*in_Normal;
	mat3 TBN = transpose(mat3(Ps,Pt,n));
	
	vec3 normal = n + bs*cross(Pt,n) + bt*cross(n,Ps);
	normal = normalize(normal);
	vec3 light = vec3(0.0, 0.0, -1.0); 
	
	v_Position 		= TBN*vec3(MV_Matrix*vec4(position,1.0));
    v_LightPos 		= TBN*vec3(MV_Matrix*vec4(light,1.0));
    v_Normal		= normal; 
	
	gl_Position = MVP_Matrix * vec4(position.xyz, 1.0); 
	
}
