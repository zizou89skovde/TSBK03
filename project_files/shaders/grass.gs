#version 150


layout (triangles) in;
layout(triangle_strip, max_vertices = 4) out;

uniform sampler2D u_GrassMask;

uniform mat4 VP_Matrix;
uniform mat4 V_Matrix;
uniform mat4 P_Matrix;

in vec2 g_TextureCoord[3];


out vec3 f_Normal;
void main()
{
	//In world coordinates
	vec3 averPos = vec3(0.0);
	vec2 averTexCoord = vec2(0.0);
	for(int i = 0; i < 3; i++){
		averTexCoord += g_TextureCoord[i];
		averPos  += gl_in[i].gl_Position.xyz; 
	}
	averPos /=3.0;
	averTexCoord /=3.0;
	
	/* Read grass mask */
	float grassMask = texture(u_GrassMask,averTexCoord).x; /* läser från textur. får tillbaka värde mellan 0.0 och 1.0 */

	/* Transform to view coordinates  */
	//averPos = (V_Matrix*vec4(averPos,1.0)).xyz;
	
	vec3 grassVertices[4];
	grassVertices[0] = averPos;
	grassVertices[1] = averPos + vec3(0.5,0.5,0.0);
	grassVertices[2] = averPos + vec3(-0.5,0.5,0.0);
	grassVertices[3] = averPos + vec3(0.0,1.5,0.0);
	
	
	
	
	//Transform to Screen 
	for(int i = 0; i < 4; i++){
			
		vec4 fragPos = VP_Matrix*vec4(grassVertices[i],1.0); 
		if(grassMask < 0.5)
			fragPos.w = -1.0;
		gl_Position = fragPos;
		EmitVertex();
	}
    EndPrimitive();
}

