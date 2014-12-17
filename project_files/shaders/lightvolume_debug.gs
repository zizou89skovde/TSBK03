#version 400 core
layout (triangles) in;

//layout(triangle_strip, max_vertices = 3) out;
layout(line_strip, max_vertices =3) out;

uniform mat4 VP_Matrix;

in vec3 g_Normal[3];

out vec3 f_Normal[3];

void main(){
  	for(int i = 0; i < 3; i++){
		gl_Position = gl_in[i].gl_Position;
		f_Normal[i] = g_Normal[i];
		EmitVertex();
	}
	EndPrimitive();
}
