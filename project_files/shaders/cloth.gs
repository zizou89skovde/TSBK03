/*
#version 410 core   

layout(triangle_strip, max_vertices = 3) out;
layout(line_strip, max_vertices = 3) out;

in vec3 g_Position[3];

out vec3 f_Normal;
out vec3 f_Position;
out vec3 f_LightPos;

void main()
{
  int len = gl_in.length();
  for(int i = 0; i < len; i++)
  {
	int index = int(mod(i,len));
    gl_Position = gl_in[index].gl_Position;
	f_Position = g_Position[index];
	f_LightPos = vec3(1.0);
	f_Normal = vec3(1.0);
    EmitVertex();
  }
  EndPrimitive();
}
*/
#version 150
//#version 410 core   

layout (triangles) in;
layout(triangle_strip, max_vertices = 6) out;
//layout(line_strip, max_vertices =3) out;

uniform mat4 MVP_Matrix;
uniform mat4 MV_Matrix;
uniform mat4 VP_Matrix;
uniform sampler2D u_MassPos_Tex;
uniform sampler2D u_SpringMap_Tex;
uniform vec2 u_Vec2; // Resolution X & Y


in vec3 g_Position[3];
in vec3 g_Normal[3];
in vec3 g_LightPos[3];

out vec3 f_Normal;
out vec3 f_Position;
out vec3 f_LightPos;


vec3 readPositionWorld(vec2 tcoord,vec2 offset){
	//Add offset to tex coord, and clamp. 
	vec2 textureCoordinate =  clamp(tcoord+offset,0.0,0.99);
	return texture(u_MassPos_Tex, textureCoordinate).xyz;
}

vec3 primitiveCorner(){
	return readPositionWorld(gl_in[0].gl_Position.xy,vec2(0.0));
}

vec3 adjustSubdividedVertex(vec3 triangleCorner,vec3 vertexPosition, float numSubdivisions,float level){
	// Translate Primitive to be centred around zero
	vec3 vpos = vertexPosition - triangleCorner;
	// Scale to new subdivided size
	vec3 halfSize   = vpos/numSubdivisions;
	vpos  = halfSize;
	float halfTriangleWitdh = u_Vec2.y/2.0;
	triangleCorner -= halfTriangleWitdh*sign(round(triangleCorner))+0.4;
	// Translate back to original position, the "triangle" are biased with half size
	vpos+=triangleCorner;
	return vpos;
		
}
/*
void passTearedTriangle(){
	int numSubdivisions = 1;
	float quadSideSize = 4.0;
	vec3 triangleCorner = primitiveCorner();
	int len = gl_in.length();	
	vec3 someOffset = vec3(0.0,3.5,0.0); // TODO:
	for(int j = 0; j < numSubdivisions; ++j){
		for(int i = 0; i < len; i++){
			// Read vertex  which is index of the current mass. 
			//Used to sample from position- and spring buffer.
			vec2 textureCoords = gl_in[i].gl_Position.xy;
			
			//Read vertex position from texture.
			vec3 vertexPosition = readPositionWorld(textureCoords,vec2(0.0));
			//vertexPosition = adjustSubdividedVertex(triangleCorner,vertexPosition,float(numSubdivisions),float(j));
			
			vertexPosition += vec3(0.0,3.5,0.0);
			//Pass data to fragment shader
			f_Position = g_Position[i];
			f_LightPos = g_LightPos[i];
			f_Normal   = g_Normal[i];
			
			//Set gl_Position
			gl_Position = MVP_Matrix*vec4(vertexPosition,1.0);
			EmitVertex();
		}
	}
	EndPrimitive();
}*/


bool checkVertexConnectivity(vec2 texCoord){
	vec2 textureCoordinate =  clamp(texCoord,0.0,0.99);
//	unsigned int spring =  texture(u_SpringMap_Tex,textureCoordinate).r;
	float spring =  round(texture(u_SpringMap_Tex,textureCoordinate).r);
	// Only connected up- and downwards. Vertex now considered disconnected and will not be displayed
	return  abs(spring - 34.0) > 1.0;//  != /255.0;
}

void passSolidTriangle(){
    int len = gl_in.length();	
	vec3 someOffset = vec3(0.0,3.5,0.0); // TODO:
 
  	for(int i = 0; i < len; i++){
		// Read texture coordinate  which is index of the current mass. 
		//Used when sampling from position- and spring buffer.
		vec2 textureCoords = gl_in[i].gl_Position.xy;
		
		//Read vertex position from texture.
		vec3 vertexPosition = readPositionWorld(textureCoords,vec2(0.0));
		vertexPosition += vec3(0.0,3.5,0.0);
		//Pass data to fragment shader
		f_Position = g_Position[i];
		f_LightPos = g_LightPos[i];
		f_Normal   = g_Normal[i];
		
		//if( abs(textureCoords.x- 0.5) < 0.01 &&  abs(textureCoords.y) < 0.5){
		if(!checkVertexConnectivity(textureCoords)){
			// cheap ass way to hide vertex?!
			vec4 tmp = VP_Matrix*vec4(vertexPosition,1.0);
			tmp.w = -1.0;
			gl_Position = tmp;
			return;
		}

		//Set gl_Position
		gl_Position = VP_Matrix*vec4(vertexPosition,1.0);
		EmitVertex();
	}
	EndPrimitive();
}





void main()
{
	passSolidTriangle();
}


/** 	CODE FOR REMOVE VERTEX

		if( abs(vertexPosition.x) < 0.1){
				vec4 tmp = MVP_Matrix*vec4(vertexPosition,1.0);
				tmp.w = -1.0;
				gl_Position = tmp;
				return;
			}
		
		*/
/****
   int numSubDivisions = 2;
  float quadSideSize = 4.0;
  vec3 triCenter = primitiveCenter();
  
  	// Read vertex position
		vec3 vertexPosition = gl_in[i].gl_Position.xyz;
		// Translate Primitive to be centred around zero
		vertexPosition -= triCenter;
		// Scale to new subdivided size
		vec3 halfSize   = vertexPosition/float(numSubDivisions);
		vertexPosition  = halfSize;
		
		if(triCenter.x != 0.0){
			vec3 triCenterComponents = sign(round(triCenter));
			triCenter+= triCenterComponents*1.0;
		}
		// Translate back to original position, the "triangle" are biased with half size
		vertexPosition+=triCenter;
		
		// 
		gl_Position = MVP_Matrix*vec4(vertexPosition,1.0);
		f_Position = g_Position[i];
		f_LightPos = g_LightPos[i];
		f_Normal   = avgNormal;
		EmitVertex();

***/

