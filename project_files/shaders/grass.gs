#version 150


layout (triangles) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 VP_Matrix;
uniform mat4 V_Matrix;
uniform mat4 P_Matrix;


void main()
{
	//In world coordinates
	vec3 averPos = vec3(0.0);
	for(int i = 0; i < 3; i++){
		averPos  += gl_in[i].gl_Position.xyz; 
	}
	averPos /=3.0;
	/* Transform to view coordinates  */
	//averPos = (V_Matrix*vec4(averPos,1.0)).xyz;

	
	
	vec3 grassVertices[4];
	grassVertices[0] = averPos;
	grassVertices[1] = averPos + vec3(0.5,0.5,0.0);
	grassVertices[2] = averPos + vec3(-0.5,0.5,0.0);
	grassVertices[3] = averPos + vec3(0.0,1.5,0.0);
	
	
	
	
	//Transform to Screen 
	for(int i = 0; i < 4; i++){
		gl_Position = VP_Matrix*vec4(grassVertices[i],1.0); 
		EmitVertex();
	}
    EndPrimitive();
}

