#version 420
layout (triangles) in;

//layout(triangle_strip, max_vertices = 3) out;
layout(line_strip, max_vertices =3) out;

uniform mat4 VP_Matrix;

void main()
{

  	for(int i = 0; i < 3; i++){
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}
