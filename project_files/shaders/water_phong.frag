#version 150
//#version 410 core   
uniform sampler2D u_TerrainColor;
uniform sampler2D u_TerrainReflection;
out vec4 out_Color;

in vec3 f_Normal;
in vec3 f_Position;
in vec3 f_LightPos;
in vec2 f_TextureCoord;
void main(void)
{

    
	vec3 normal = f_Normal;
	vec3 position = f_Position;
	vec3 lightPosition = f_LightPos;
	vec3  lightVector 	= normalize(lightPosition);
	vec3 l = normalize(lightVector);
	vec3 e = normalize(-position);
	vec3 n = normalize(normal);
	vec3 r = reflect(-l,n);
	
	/** Let reflect eye to water-vector around water normal **/
	vec3 waterReflection = normalize(reflect(e,n));
	//vec3 waterReflection = normalize(reflect(e,n));
	
	/** Estimate a value of the angle **/
	float incident = abs(dot(waterReflection,n));
	
	/** Distort the reflected or refracted image **/
	vec2 waterDistortion = vec2(0.0); //waterReflection.xy * 0.05;
	vec2 tcoord = vec2(0.5+gl_FragCoord.xy)/512.0 + waterDistortion;
	
	/** Sample refraction and reflection image **/
	vec4 colorRefraction = texture(u_TerrainColor,tcoord);
	vec4 colorReflection = texture(u_TerrainReflection,tcoord);
	
	/** Mix refraction and reflection color depending of the value of the incident angle **/
	vec4 color = mix(colorRefraction,colorReflection,0.5*incident);
		
	
	float specular = max(dot(r,e),0.0);
	specular = pow(specular,40.0);
	
	float diffuse = max(dot(n,-l),0.0);
	float ambient = 0.0;
	color = vec4(0.4);
	out_Color = (specular+diffuse+ambient)*color+ vec4(0.0,0.0,0.0,1.0);
}
