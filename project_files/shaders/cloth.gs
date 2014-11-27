#version 150
//#version 410 core   

layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;
//layout(line_strip, max_vertices =3) out;

uniform mat4 MVP_Matrix;
uniform mat4 MV_Matrix;
uniform mat4 VP_Matrix;
uniform sampler2D u_MassPos_Tex;
uniform vec2 u_Vec2; // Resolution X & Y

in vec3 g_Position[3];
in vec3 g_Normal[3];
in vec3 g_LightPos[3];
in vec2 g_Texcoord[3];
in highp uint g_SpringState[3];

out vec3 f_Normal;
out vec3 f_Position;
out vec3 f_LightPos;

bool checkVertexConnectivity(){
	return true;
}

void passSolidTriangle(){
    int len = gl_in.length();	
	vec3 someOffset = vec3(0.0,3.5,0.0); // TODO:
 
  	for(int i = 0; i < len; i++){
		// Read texture coordinate  which is index of the current mass. 
		//Used when sampling from position- and spring buffer.
		vec2 textureCoords = g_Texcoord[i];
		
		//Read vertex position from texture.
		//Pass data to fragment shader
		f_Position = g_Position[i];
		f_LightPos = g_LightPos[i];
		f_Normal   = g_Normal[i];
		
		//if( abs(textureCoords.x- 0.5) < 0.01 &&  abs(textureCoords.y) < 0.5){
		/*if(!checkVertexConnectivity(textureCoords)){
			// cheap ass way to hide vertex?!
			vec4 tmp = gl_in[i];
			tmp.w = -1.0;
			gl_Position = tmp;
			return;
		}*/

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

