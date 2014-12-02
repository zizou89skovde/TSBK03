#version 150
//#version 410 core   
uniform sampler2D u_TerrainColor;
uniform sampler2D u_TerrainReflection;
out vec4 out_Color;

in vec3 f_Normal;
in vec3 f_Position;
in vec3 f_LightPos;
in vec2 f_TextureCoord;
in float f_NormalWorld_y;
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
	vec3 waterReflection 		   = normalize(reflect(-e,n));

	/** Estimate a value of the angle **/
	float incident = abs(dot(e,n));
	float distortionAmplitude =  clamp(1.0-f_NormalWorld_y,0.0,1.0); 
	
	/** Distort the reflected or refracted image **/
	vec2 distRefl = waterReflection.xy * 0.1*distortionAmplitude;
	vec2 distRefr = waterReflection.xy * 0.4*distortionAmplitude;
	
	vec2 texRefr = clamp(vec2(0.5+gl_FragCoord.xy)/512.0+distRefr,0.0,1.0);
	vec2 texRefl = clamp(vec2(0.5+gl_FragCoord.xy)/512.0+distRefl,0.0,1.0);
	
	/** Sample refraction and reflection image **/
	vec4 colorRefraction = texture(u_TerrainColor,texRefr);
	vec4 colorReflection = texture(u_TerrainReflection,texRefl);
	
	/** Mix refraction and reflection color depending of the value of the incident angle **/
	vec4 color = mix(colorReflection,colorRefraction,incident);
		
	
	float specular = max(dot(r,e),0.0);
	specular = 1.2*pow(specular,40.0);
	
	float diffuse = max(dot(n,l),0.0);
	float ambient = 0.0;
	//color = vec4(0.4);
	out_Color = (specular+diffuse+ambient)*color+ vec4(0.0,0.0,0.0,1.0);
}
