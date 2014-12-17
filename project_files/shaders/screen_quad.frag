#version 400
uniform sampler2D u_Texture;

in vec2 f_TexCoord;
out vec4 out_Color;

void main(){
	
	
		//out_Color = vec4(1.0);
		float color = pow(texture(u_Texture,f_TexCoord).x,2.0);
		
		out_Color  = vec4(color); 	
		out_Color.a = 0.1;
		out_Color.rgb *=1.5;
}

