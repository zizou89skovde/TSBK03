#version 150


layout (points) in;
layout(triangle_strip, max_vertices = 3) out;

/*

layout (points) in;
layout (line_strip, max_vertices = 4) out;
*/
uniform mat4 VP_Matrix;
uniform mat4 V_Matrix;
uniform mat4 P_Matrix;
 


void main()
{

	for(int i = 0; i < 1; i++){
		gl_Position = VP_Matrix*(gl_in[i].gl_Position); 
		EmitVertex();
	}
	// Create a grass strip of 4 points
	/*
	
    gl_Position =  vec4(-0.1, 0.0, 0.0, 1.0); ///(gl_in[0].gl_Position +
    EmitVertex();

    gl_Position =  vec4(-0.0, 0.0, 0.0, 1.0);
    EmitVertex();

    gl_Position =  vec4(0.1, 0.0, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(0.2, 0.0, 0.0, 1.0); //VP_Matrix
    EmitVertex();
*/
    EndPrimitive();
}

