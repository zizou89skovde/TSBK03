#version 150

// define the number of CPs in the output patch                                                 
layout (vertices = 3) out; 

in vec3 v_Position[];    


// attributes of the output CPs                                                                 
out vec3 tes_Position[]; 

void main()
{
    tes_Position[gl_InvocationID] = v_Position[gl_InvocationID];
	float tessLevelInner = 2.0;
	float tessLevelOuter = 2.0;
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = tessLevelInner;
        gl_TessLevelOuter[0] = tessLevelOuter;
        gl_TessLevelOuter[1] = tessLevelOuter;
        gl_TessLevelOuter[2] = tessLevelOuter;
    }
}

// eval

#version 150

layout (triangles, equal_spacing, cw) in;

uniform mat4 MV_Matrix;
uniform mat4 MVP_Matrix;

in vec3 tes_Position[];
out vec3 f_Postion;

void main()
{
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];

    tePosition = p0 + p1 + p2;
    gl_Position = uProj * uModelView * vec4(tePosition, 1);
}