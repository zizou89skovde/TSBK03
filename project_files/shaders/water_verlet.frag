#version 400

uniform sampler2D u_CurrentPosition;
uniform sampler2D u_PreviousPosition;
uniform sampler2D u_HeightMap;
uniform vec2  u_Meta;
uniform float u_Gravity;
uniform float u_DeltaTime;
uniform float u_Time;
uniform float u_SystemDamping;

in vec2 f_TexCoord;
/* Array of spring directions  */
const float springDirections[] ={
	// Struct
	1.0 ,0.0,
	0.0 ,1.0,
	-1.0,0.0,
	0.0,-1.0,
	//Shear
	1.0 ,1.0,
	-1.0,1.0,
	-1.0,-1.0,
	1.0 ,-1.0,
	//Bend
	2.0,0.0,
	0.0,2.0,
	-2.0,0.0,
	0.0 ,-2.0
 };
 
   const uint RIGHT_STRUCTURAL_SPRING   =  0x1;
 
vec3 getVerletVelocity(vec3 position, vec3 previousPosition){
	return (position-previousPosition)/u_DeltaTime;
}

/* Return position and velocity for the mass at a coordinate*/
void readMassData(vec2 textureCoordinate, out vec3 position, out vec3 velocity){
	position 			  = texture(u_CurrentPosition,textureCoordinate).xyz;
	vec3 previousPosition = texture(u_PreviousPosition,textureCoordinate).xyz;
	velocity 			  = getVerletVelocity(position,previousPosition);
}

vec4 springContribution(vec3 mass1Position, vec3 mass1Velocity, vec2 textureOffset, float springConstant, float springRestLength){
	/* Reading position and velocity of the mass on the other end of selected spring */
	vec2 mass2TextureCoordinate = f_TexCoord+textureOffset;
	
	vec3 mass2Position;
	vec3 mass2Velocity;
	readMassData(mass2TextureCoordinate,mass2Position,mass2Velocity);
	/*
	vec3 deltaPosition = mass1Position-mass2Position;
	vec3 deltaVelocity = mass1Velocity-mass2Velocity;
	
	float springLength = length(deltaPosition);
	
	float springForce = -springConstant*(springLength - springRestLength);
	float dampForce   = u_SpringDamping * (dot(deltaPosition,deltaVelocity)/springLength); // u_SpringDamping 
	
	vec3 force = normalize(deltaPosition)*(springForce+dampForce);
	*/
	/* Check if spring has breaked */
	float springHasBreaked = 0.0;
	/*if(springLength > 4.0*springRestLength){
		springHasBreaked = 1.0;
	}*/
	//suspension = mass2Position.yz
	
	vec3 force = 0.001*vec3(0.0,mass2Position.y-mass1Position.y,0.0);
	/* Force = acceleration since the mass is considered to be 1. */
	return vec4(force,springHasBreaked);
}

vec3 applySpringForce(vec3 position, vec3 velocity,highp uint springState,out highp uint outSpringState){
	
	vec3 force = vec3(0.0);
	/* Step in texture to read near by mass */
	float stepSize = 1.0/(u_Meta.x); // OR .y

	/* Loop through all 12 possible spring connectivities */
	/* Add force contribution from each spring that is currently 
	   to the mass at position */ 
	outSpringState = springState;
	for(int i = 0; i < 4; ++i){
			/*Read spring contribution*/
			vec2 step = vec2(springDirections[2*i],springDirections[2*i+1]);
			vec2 textureOffset = stepSize*step;
			vec4 result = springContribution(position,velocity,textureOffset,0.25,1.0);
	
			float springHasBreaked = result.w;
			vec3 forceAddition = result.xyz;
			force += forceAddition;
	}
	/* Force = acceleration since the mass is considered to be 1. */
	return  force;
}

float waveSource(vec3 position,out vec3 out1,out vec3 out2){
	float strength = clamp((2.0 - length(position.xz)),0.0,1.0);
	if(strength >= 0.1){
		position.y = 1.0*sin(u_Time);
		out1 = position;
		out2 = position;
	}
	return strength;
}


/* GPU implementation of the applyForces in CPUClothsimulation.cpp */
vec3 applyForces(vec3 position, vec3 velocity,highp uint springState,out highp uint outSpringState){
	
	vec3 acceleration = vec3(0.0);

	/* Apply gravity force */
	//acceleration += vec3(0.0,u_Gravity,0.0);
	
	/* Damping force */
	acceleration += velocity*u_SystemDamping;
	
	/* Add force from springs */
	acceleration+= applySpringForce(position,velocity,springState,outSpringState);
	
	/* return force=acceleration, due mass is set to 1.0 */
	return acceleration;
}

/*GPU implementation of an Verlet integration */
vec3 integrate(vec3 position, vec3 previousPosition, vec3 acceleration){
		vec3 nextPosition = 2.0*position - previousPosition  + u_DeltaTime * u_DeltaTime* acceleration;	
		return nextPosition;
}

vec3 groundCollision(vec3 inPosition,vec3 outPosition,out bool colllided){
	float height = texture(u_PreviousPosition, f_TexCoord).x;
	vec2 groundXZ = (f_TexCoord-0.5)*2.0;
	return vec3(0.0);
}

void main(void)
{
	vec4 massData	 	  = texture(u_CurrentPosition, f_TexCoord);
	float springState	  = massData.w;

	vec3 position		  = massData.xyz;
	vec3 previousPosition = texture(u_PreviousPosition, f_TexCoord).xyz;
	vec3 velocity 		  = getVerletVelocity(position,previousPosition);
	
	highp uint springStateBits = uint(springState);
	highp uint nextSpringStateBits;
	vec3 acceleration 	  = applyForces(position,velocity,springStateBits,nextSpringStateBits);
	vec3 outPosition;
	vec3 outPrevPosition;
	if(waveSource(position,outPrevPosition,outPosition) < 0.1){
		outPrevPosition = position;
		outPosition 	= integrate(position,previousPosition,acceleration);
	}
	
	//groundCollision
	
	
	
	/* Set next position */
	gl_FragData[0]  = vec4(outPosition,springState); 
	/* Set previous position */
	gl_FragData[1]  = vec4(outPrevPosition,springState); 
}

