#version 400// core 
  


in  vec3 in_Position;


uniform mat4 VP_Matrix;
uniform mat4 V_Matrix;
uniform mat4 VP_LightMatrix;

uniform sampler2D u_DepthMap;


const mat4 biasMat =   mat4(0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0);

void main(void)
{
	vec4 position = vec4(in_Position,1.0);
	vec4 positionLight = LightViewProjectionMatrix * in_Position;

	vec4 projectedCoordinates = biasMat * positionLight;
	if(inPosition.x > 1.5){
		float depth = texture(u_DepthMap,projectedCoordinates.xy).r;
		depth
	}
		

	gl_Position = VP_Matrix*vec4(in_Position.xyz, 1.0); 	
	
}

