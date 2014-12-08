#version 400
uniform sampler2D u_Texture;

in vec2 f_TexCoord;
out vec4 out_Color;

void main(){
	
	
		//out_Color = vec4(1.0);
		out_Color  = clamp(texture(u_Texture,f_TexCoord),0.0,0.8); 	
	
}

