#version 400

uniform sampler2D u_CurrentPosition;
uniform sampler2D u_PreviousPosition;
uniform vec2 u_Resolution;
uniform float u_Gravity;
uniform vec3 u_Wind;
uniform vec3 u_SpherePosition;
uniform float u_SphereRadius;
uniform float u_DeltaTime;
uniform float u_SystemDamping;

uniform float u_SpringDamping;
uniform float u_SpringConstant;
uniform float u_SpringConstantBend;
uniform float u_RestLengthStruct; 
uniform float u_RestLengthShear;
uniform float u_RestLengthBend;
uniform float u_FixPointConstant;

out vec4 out1;
out vec4 out2;

/** DEBUG SPHERE **/
in vec2 f_TexCoord;

/* Array of spring directions  */
const float springDirections[] = float[24](
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
 );
/* The spring state are encoded according to following bit table: */

/* STRUCTURAL SRPINGS:  */
  const uint RIGHT_STRUCTURAL_SPRING   =  0x1;
  const uint UP_STRUCTURAL_SPRING      =  0x2;
  const uint LEFT_STRUCTURAL_SPRING    =  0x4;
  const uint DOWN_STRUCTURAL_SPRING    =  0x8;

/*  SHEAR SPRINGS: */
  const uint UP_RIGHT_SHEAR_SPRING     =  0x10;
  const uint UP_LEFT_SHEAR_SPRING      =  0x20;
  const uint DOWN_LEFT_SHEAR_SPRING    =  0x40;
  const uint DOWN_RIGHT_SHEAR_SPRING   =  0x80; 

/*  BEND SPRINGS: */
  const uint RIGHT_BEND_SPRING         =  0x100;
  const uint UP_BEND_SPRING            =  0x200;
  const uint LEFT_BEND_SPRING          =  0x400;
  const uint DOWN_BEND_SPRING          =  0x800;
  
  
  //DEBUG 
  
  //FIX POINTS
  #define P1_X 1.1
  #define P2_X 0.9
  
  
 #define P1_Y 0.98
 #define P2_Y 0.98
  



vec3 getVerletVelocity(vec3 position, vec3 previousPosition){
	return (position-previousPosition)/u_DeltaTime;
}

/* Return position and velocity for the mass at a coordinate*/
void readMassData(vec2 textureCoordinate, out vec3 position, out vec3 velocity){

	position 			  = texture(u_CurrentPosition,textureCoordinate).xyz;
	vec3 previousPosition = texture(u_PreviousPosition,textureCoordinate).xyz;
	
	velocity = getVerletVelocity(position,previousPosition);
}

float getRestLength(uint springState){
	float restLength = 0.0;
	if(springState < UP_RIGHT_SHEAR_SPRING)
		restLength = u_RestLengthStruct;
	else if(springState < RIGHT_BEND_SPRING)
		restLength = u_RestLengthShear;
	else
		restLength = u_RestLengthBend;
		
	return restLength;
}

float getSpringConstant(uint springState){
	/*Note: that the structural and shear springs share the same constant */
	float springConstant = 0.0;
	if(springState < RIGHT_BEND_SPRING)
		springConstant = u_SpringConstant;
	else
		springConstant = u_SpringConstantBend;
		
	return springConstant;
}

vec4 springContribution(vec3 mass1Position, vec3 mass1Velocity, vec2 textureOffset, float springConstant, float springRestLength){
	/* Reading position and velocity of the mass on the other end of selected spring */
	vec2 mass2TextureCoordinate = f_TexCoord+textureOffset;
	/*if(mass2TextureCoordinate.x < 0.0 || mass2TextureCoordinate.x > 1.0)
		return vec4(0.0);
	if(mass2TextureCoordinate.y < 0.0 || mass2TextureCoordinate.y > 1.0)
		return vec4(0.0);*/
		
	vec3 mass2Position;
	vec3 mass2Velocity;
	readMassData(mass2TextureCoordinate,mass2Position,mass2Velocity);
	
	vec3 deltaPosition = mass1Position-mass2Position;
	vec3 deltaVelocity = mass1Velocity-mass2Velocity;
	
	float springLength = length(deltaPosition);
	
	float extra = 1.0 ;//clamp(springLength/springRestLength,1.0,4.0);
	float springForce = -extra*springConstant*(springLength - springRestLength);
	float dampForce   = u_SpringDamping * (dot(deltaPosition,deltaVelocity)/springLength); // u_SpringDamping 
	
	vec3 force = normalize(deltaPosition)*(springForce+dampForce);
	
	/* Check if spring has breaked */
	float springHasBreaked = 0.0;
	/*if(springLength > 4.0*springRestLength){
		springHasBreaked = 1.0;
	}*/
	/* Force = acceleration since the mass is considered to be 1. */
	return vec4(force,springHasBreaked);
}

vec3 applySpringForce(vec3 position, vec3 velocity,highp uint springState,out highp uint outSpringState){
	
	vec3 force = vec3(0.0);
	/* Step in texture to read near by mass */
	float stepSize = 1.0/(u_Resolution.x-1.0); // OR .y

	/* Loop through all 12 possible spring connectivities */
	/* Add force contribution from each spring that is currently 
	   to the mass at position */ 
	outSpringState = springState;
	for(int i = 0; i < 12; ++i){
		highp uint springMask = RIGHT_STRUCTURAL_SPRING << i; 
		if((springState & springMask ) != 0){
			
			float restLength = getRestLength(springMask); 
			float springConstant = getSpringConstant(springMask);
			
			/*Read spring contribution*/
			vec2 step = vec2(springDirections[2*i],springDirections[2*i+1]);
			vec2 textureOffset = stepSize*step;
			vec4 result = springContribution(position,velocity,textureOffset,springConstant,restLength);
			
			/*Check if spring has breaked */
			float springHasBreaked = result.w;
			if(springHasBreaked != 0.0){
				/*If spring break remove spring from the spring state variable */
				outSpringState &= ~springMask;	
			}else{
				vec3 forceAddition = result.xyz;
				force += forceAddition;
			}
	
		}
		
	}
	/* Force = acceleration since the mass is considered to be 1. */

	return  force;
}

vec3 collisionForce(vec3 position){
	float sphereRadius = u_SphereRadius+0.05;
	vec3 spherePosition = u_SpherePosition; //vec3(0.0);
	vec3 delta = position-spherePosition;
	float distance = length(delta);

	vec3 collisionForce = vec3(0.0);
	if(distance < sphereRadius){
		float adjustDistance = sphereRadius - distance;
		collisionForce = 1.01*adjustDistance*normalize(delta);
	}
	return collisionForce;
}

/* GPU implementation of the applyForces in CPUClothsimulation.cpp */
vec3 applyForces(vec3 position, vec3 velocity,highp uint springState,out highp uint outSpringState){
	
	vec3 acceleration = vec3(0.0);
	
	/* Apply gravity force */
	acceleration += vec3(0.0,u_Gravity,0.0);
	
	/* Apply wind force */
	//acceleration += u_Wind;
	
	/* Collision Force */
	acceleration += collisionForce(position);
	
	
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

void main(void)
{
	vec4 massData	 	  = texture(u_CurrentPosition, f_TexCoord);
	float springState	  = massData.w;

	vec3 position		  = massData.xyz;
	vec3 previousPosition = texture(u_PreviousPosition, f_TexCoord).xyz;
	vec3 velocity 		  = getVerletVelocity(position,previousPosition);
	
	/* Home made shenanigans: unpack spring state into high precision unsigned integer */
	highp uint springStateBits = uint(springState);
	highp uint nextSpringStateBits;
	vec3 acceleration 	  = applyForces(position,velocity,springStateBits,nextSpringStateBits);
	
	/* Temp hack. Fixed regions should be uploaded as uniform */
	if(  (f_TexCoord.x > 0.95 || mod(f_TexCoord.x,0.25) < 0.05  ) && f_TexCoord.y > P1_Y  ){
		acceleration = vec3(0.0);	
	}
	
	vec3 nextPosition 	  = integrate(position,previousPosition,acceleration);
	
	
		

	/* Set next position */
	out1 = vec4(nextPosition,float(nextSpringStateBits));
	/* Set previous position */
	out2 = vec4(position,float(nextSpringStateBits));

}
