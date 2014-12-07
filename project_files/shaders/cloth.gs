#version 150
//#version 410 core   

layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;
//layout(line_strip, max_vertices =3) out;

uniform mat4 MVP_Matrix;
uniform mat4 MV_Matrix;
uniform mat4 VP_Matrix;
uniform sampler2D u_MassPos_Tex;
uniform vec2 u_Resolution;

in vec3 g_Position[3];
in vec3 g_Normal[3];
in vec3 g_LightPos[3];
in vec2 g_Texcoord[3];
in highp uint g_SpringState[3];

out vec3 f_Normal;
out vec3 f_Position;
out vec3 f_LightPos;

const highp uint SpringTable[25] = uint[](
	0x000,0x000,0x800,0x000,0x000,
	0x000,0x040,0x008,0x080,0x000,
	0x400,0x004,0x000,0x001,0x100,
	0x000,0x020,0x002,0x010,0x000,
	0x000,0x000,0x200,0x000,0x000
);

highp uint parseConnection(vec2 deltaVec){
vec2 resolution = u_Resolution;
	float step = 1.0/(resolution.x-1.0);
	vec2 v = deltaVec/step;
	int i = int(v.x);
	int j = int(v.y);
	int index = i+2*j;
	return SpringTable[index];
	
}

bool checkVertexConnectivity(highp uint springState,int i){
	int index1 = int(mod(i+1,3));
	int index2 = int(mod(i+2,3));

	vec2 delta1 = g_Texcoord[index1] - g_Texcoord[i];  
	vec2 delta2 = g_Texcoord[index2] - g_Texcoord[i];	

	/* Parse which spring code depending on which direction the neighbour vertex reside */
	highp uint springCode1 = parseConnection(delta1);
	highp uint springCode2 = parseConnection(delta2);


	/* Check first connection */	
	highp uint status1 = springState & springCode1;


	/* Check the other connection */
	highp uint status2 = springState & springCode2;

	/* If any of the statuses are 0 the spring has broken  */
	return (status1 != uint(0) && status2 != uint(0));
}

void passSolidTriangle(){
    int len = gl_in.length();	
	vec3 someOffset = vec3(0.0,3.5,0.0); // TODO:
 
  	for(int i = 0; i < len; i++){
		// Read texture coordinate  which is index of the current mass. 
		// Used when sampling from position- and spring buffer.
		vec2 textureCoords = g_Texcoord[i];
		
		//Read vertex position from texture.
		//Pass data to fragment shader
		f_Position = g_Position[i];
		f_LightPos = g_LightPos[i];
		f_Normal   = g_Normal[i];
/*7		if(g_SpringState[i] == uint(0)){
		if(!checkVertexConnectivity(g_SpringState[i],i)){
			// cheap ass way to hide vertex?!
			vec4 tmp = gl_in[i].gl_Position;
			tmp.w = -1.0;
			gl_Position = tmp;
			EndPrimitive();			
			return;
		}
*/
		//Set gl_Position
		gl_Position = VP_Matrix*gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}





void main()
{
	passSolidTriangle();
}

