#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform sampler2D u_GrassMask;

out float outValue;

void main() 
{

	/* Get  position in a triangle (In world coordinates) */
	vec3 position = vec3(0.0);
	float height = 0;
	vec2 texCoord = vec2(0.0);
	getPosition(position, height, texCoord);

	/* Read grass mask from texture, returns value between 0 and 1 */
	float grassMask = texture(u_GrassMask, texCoord).x;

    for (int i = 0; i < 3; i++) {
        outValue = geoValue[0] + i;
        EmitVertex();
    }

    EndPrimitive();
}

