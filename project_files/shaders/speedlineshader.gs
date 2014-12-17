#version 400

layout (points) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4 MVP_Matrix;
uniform mat4 MV_Matrix;
uniform mat4 P_Matrix;

void main()
{
    vec4 inVert = gl_in[0].gl_Position;
    vec4 outVert1 = inVert; 
    vec4 outVert2 = inVert + vec4(1.0); 


    gl_Position = outVert1;
    EmitVertex();
    gl_Position = outVert2;
	EmitVertex();

	EndPrimitive();
}
