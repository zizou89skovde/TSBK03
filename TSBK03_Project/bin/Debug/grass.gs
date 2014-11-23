#version 150
 
layout (points) in;
layout (line_strip, max_vertices = 4) out;

void main()
{

	// Create a grass strip of 4 points

    gl_Position = gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.0, 2.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.0, 3.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.0, 4.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
