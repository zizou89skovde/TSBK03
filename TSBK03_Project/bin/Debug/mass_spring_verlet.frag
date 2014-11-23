#version 400

uniform sampler2D u_CurrentPosition;
uniform sampler2D u_PreviousPosition;
uniform vec2 u_Resolution;
uniform float u_Gravity;
uniform vec3 u_Wind;
uniform vec3 u_SpherePosition;
uniform float u_DeltaTime;
uniform float u_SystemDamping;

uniform float u_SpringDamping;
uniform float u_SpringConstant;
uniform float u_SpringConstantBend;
uniform float u_RestLengthStruct; 
uniform float u_RestLengthShear;
uniform float u_RestLengthBend;

/** DEBUG SPHERE **/





in vec2 f_TexCoord;
out vec4 out_Color;

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

vec3 springContribution(vec3 mass1Position, vec3 mass1Velocity, vec2 textureOffset, float springConstant, float springRestLength){
	/* Reading position and velocity of the mass on the other end of selected spring */
	vec2 mass2TextureCoordinate = f_TexCoord+textureOffset;
	if(mass2TextureCoordinate.x < 0.0 || mass2TextureCoordinate.x > 1.0)
		return vec3(0.0);
	if(mass2TextureCoordinate.y < 0.0 || mass2TextureCoordinate.y > 1.0)
		return vec3(0.0);
		
	vec3 mass2Position;
	vec3 mass2Velocity;
	readMassData(mass2TextureCoordinate,mass2Position,mass2Velocity);
	
	vec3 deltaPosition = mass1Position-mass2Position;
	vec3 deltaVelocity = mass1Velocity-mass2Velocity;
	
	float springLength = length(deltaPosition);
	
	float springForce = -springConstant*(springLength - springRestLength);
	float dampForce   = u_SpringDamping * (dot(deltaPosition,deltaVelocity)/springLength); // u_SpringDamping 
	
	vec3 force = normalize(deltaPosition)*(springForce+dampForce);
	
	/* Force = acceleration since the mass is considered to be 1. */
	return force;
}

vec3 applySpringForce(vec3 position, vec3 velocity,highp uint springState){
	
	vec3 force = vec3(0.0);
	/* Step in texture to read near by mass */
	float stepSize = 1.0/(u_Resolution.x-1.0); // OR .y

	/* Loop through all 12 possible spring connectivities */
	/* Add force contribution from each spring that is currently 
	   to the mass at position */
	for(int i = 0; i < 12; ++i){
		highp uint springMask = RIGHT_STRUCTURAL_SPRING << i; 
		if((springState & springMask ) != 0){
			vec2 step = vec2(springDirections[2*i],springDirections[2*i+1]);
			vec2 textureOffset = stepSize*step;
			float restLength = getRestLength(springMask); //length(step)*2.0*u_Resolution.y/u_Resolution.x; //
			float springConstant = getSpringConstant(springMask);
			force += springContribution(position,velocity,textureOffset,springConstant,restLength);
		}
		
	}
	/* Force = acceleration since the mass is considered to be 1. */
	
	return  force;
}

vec3 sphereCollision(vec3 position){
	float sphereRadius = 1.0;
	vec3 spherePosition = u_SpherePosition; //vec3(0.0);
	vec3 delta = position-spherePosition;
	float distance = length(delta);
	
	if(distance < sphereRadius){
		position += delta; //distance*normalize(
	}
	return position;
}

/* GPU implementation of the applyForces in CPUClothsimulation.cpp */
vec3 applyForces(vec3 position, vec3 velocity,highp uint springState){
	
	vec3 acceleration = vec3(0.0);
	
	/* Apply gravity force */
	acceleration += vec3(0.0,1.0*u_Gravity,0.0);
	
	/* Apply wind force */
	acceleration += u_Wind;
	
	/* Damping force */
	acceleration += velocity*(-0.0125);//u_SystemDamping;
	
	/* Add force from springs */
	acceleration+= applySpringForce(position,velocity,springState);
	
	/* return force=acceleration, due the mass is set to 1.0 */
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
	
	/* Home made shenanigans: unpack spring state into high precision unsigned integer */
	highp uint springStateBits = uint(springState);
	
	vec3 position		  = massData.xyz;
	vec3 previousPosition = texture(u_PreviousPosition, f_TexCoord).xyz;
	vec3 velocity 		  = getVerletVelocity(position,previousPosition);
	
	vec3 acceleration 	  = applyForces(position,velocity,springStateBits);
	
	/* Temp hack. Fixed regions should be uploaded as uniform */
	if( (f_TexCoord.x < P1_X  && f_TexCoord.y > P1_Y) || (f_TexCoord.x > P2_X  && f_TexCoord.y > P1_Y))
		acceleration = vec3(0.0);
		
	vec3 nextPosition 	  = integrate(position,previousPosition,acceleration);
	
	nextPosition = sphereCollision(nextPosition);
	/*
	if((springStateBits & (RIGHT_STRUCTURAL_SPRING << 0)) == 0)
		out_Color 			  = vec4(position,springState); 
	else{
		out_Color 			  = vec4(nextPosition,springState); 
	}
	*/
	/* Home made shenanigans: pack spring state into float */
	out_Color 			  = vec4(nextPosition,springState); 
}
/*
  const uint RIGHT_STRUCTURAL_SPRING   =  0x1;
  const uint UP_STRUCTURAL_SPRING      =  0x2;
  const uint LEFT_STRUCTURAL_SPRING    =  0x4;
  const uint DOWN_STRUCTURAL_SPRING    =  0x8;
  const uint UP_RIGHT_SHEAR_SPRING     =  0x10;
  const uint UP_LEFT_SHEAR_SPRING      =  0x20;
  const uint DOWN_LEFT_SHEAR_SPRING    =  0x40;
  const uint RIGHT_BEND_SPRING         =  0x100;
  const uint UP_BEND_SPRING            =  0x200;
  const uint LEFT_BEND_SPRING          =  0x400;
  const uint DOWN_BEND_SPRING          =  0x800;
*/