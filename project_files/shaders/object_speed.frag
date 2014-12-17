#version 150

out vec4 out_Color;
uniform float u_SpeedlinesAlpha;
in vec3 v_Normal;
in vec3 v_Position;
in vec3 v_LightPos;
void main(void)
{
	vec4 color = vec4(1.0,1.0,1.0,u_SpeedlinesAlpha);
    
	vec3 normal = v_Normal;
	
	vec3  lightVector 	= normalize(v_LightPos-v_Position);	
	vec3 l = normalize(lightVector);
	vec3 e = normalize(-v_Position);
	vec3 n = normalize(normal);
	vec3 r = reflect(-l,n);
	
	float specular = max(dot(r,e),0.0);
	specular = pow(specular,40.0);
	
	float diffuse = max(dot(n,l),0.0);
	float ambient = 0.0;

	out_Color = (specular+diffuse+ambient)*color;
}
