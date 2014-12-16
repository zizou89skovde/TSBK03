#version 400

uniform sampler2D u_CurrentPosition;
uniform sampler2D u_PreviousPosition;
uniform sampler2D u_HeightMap;
uniform float u_SeaLevel;
uniform vec2  u_Meta;
uniform float u_Gravity;
uniform float u_DeltaTime;
uniform float u_SystemDamping;
uniform float u_EnvironmentHeight;
uniform float u_EnvironmentSize;
uniform vec4 u_RainDrop;
in vec2 f_TexCoord;

out vec4 out1;
out vec4 out2;

/* Array of spring directions  */
const float springDirections[] = float[8](
	// Struct
	1.0 ,0.0,
	0.0 ,1.0,
	-1.0,0.0,
	0.0,-1.0
	);
 

 
vec3 getVerletVelocity(vec3 position, vec3 previousPosition){
	return (position-previousPosition)/u_DeltaTime;
}


float heightOverGround(vec3 position){
	vec2 mapPosition = 0.5+(position.xz)/u_EnvironmentSize;
	float height = (texture(u_HeightMap, mapPosition).x-0.5)*u_EnvironmentHeight;
	return (u_SeaLevel-height);
}


vec3 applyNeighbourForce(vec3 centerPosition, vec3 velocity,float height){
	
	vec3 force = vec3(0.0);
	/* Step in texture to read near by mass */
	float stepSize = 1.0/(u_Meta.x-1.0); // OR .y
	
	vec3 neighbourPosition[4];
	vec4 heightDiffs;
	/* Do not update  elements on the border of the grid */
	if(  f_TexCoord.x <  stepSize || f_TexCoord.x  > 1.0-stepSize ||
		 f_TexCoord.y <  stepSize || f_TexCoord.y  > 1.0-stepSize
	  ){
		return vec3(0.0);	 
	}
		
	vec3 sumNeighbours = vec3(0.0);
	for(int i = 0; i < 4; ++i){
			
			/*Determine neighbour texture coordinate */
			vec2 step = stepSize*vec2(springDirections[2*i],springDirections[2*i+1]);
			vec2 texCoord = f_TexCoord+step;
			texCoord = clamp(texCoord,0.0,0.999);
		
			/* Read neighbour position */
			sumNeighbours += texture(u_CurrentPosition,texCoord).xyz;
		
			
	}

	force = (sumNeighbours - 4.0*centerPosition)*0.9;
	
	/* FAKE #1337 */
	float deltaHeight =  u_SeaLevel - centerPosition.y;
	force.y += 0.00004 * deltaHeight;

	if(height <= 1.5){	
		force *= clamp(height,0.0,1.0);
	}

	/* Force = acceleration since the mass is considered to be 1. */
	return  force;
}

int rainDrop(vec3 position,float height,out vec3 out1,out vec3 out2){
	int afftected = 0;
	if(height > 0.2){
		float radius = u_RainDrop.z;
		float distance = length(u_RainDrop.xy - position.xz);	
		if(distance < radius ){
			afftected = 1;
			float amplitude = min(u_RainDrop.w,abs(height));
			out1 = position - vec3(0.0,amplitude,0.0);
			out2 = position - vec3(0.0,amplitude,0.0);
		}
	}
	return afftected;
}

/* GPU implementation of the applyForces in CPUClothsimulation.cpp */
vec3 applyForces(vec3 position, vec3 velocity,float height){
	
	vec3 acceleration = vec3(0.0);

	/* Apply gravity force */
	//acceleration += vec3(0.0,-0.00001,0.0);
	
	/* Damping force */
	acceleration += velocity*u_SystemDamping;
	
	//acceleration.xz += velocity.xz*-0.5;
	/* Add force from springs */
	acceleration+= applyNeighbourForce(position,velocity,height);
	
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
	float height 		  = heightOverGround(position);
	vec3 acceleration 	  = applyForces(position,velocity,height);
	
	/* Output positions */
	vec3 outPosition 	= position;
	vec3 outPrevPosition = position;
	
	/* Let current particle be influenced by an external source */
	if(rainDrop(position,height,outPrevPosition,outPosition) == 0){
		outPrevPosition = position;
		outPosition 	= integrate(position,previousPosition,acceleration);
	}


	/* Set next position */
	out1  = vec4(outPosition,springState); 	//gl_FragData[0]  = vec4(outPosition,springState); 
	/* Set previous position */
	out2  = vec4(position,springState); 	//gl_FragData[1]  = vec4(outPrevPosition,springState); 
}

