#version 150

in vec2 f_TexCoord;
uniform sampler2D u_Texture;
uniform sampler2D u_Contours;
uniform sampler2D u_SketchTexture;
out vec4 out_Color;

// Quantize given color in given number of steps
float quantize(float color, float steps)
{
    int iter = 0;
    float color_quant = 1/steps;
    while(iter < steps) {
        if (color < color_quant)
            return color_quant - 1/steps; // Rounds down
            //return color_quant; // Rounds up
        iter++;
        color_quant = (iter + 1)/steps;
    }
    return 1.0;
}

vec4 sketchColor(vec4 color_in)
{
    if (sqrt(dot(color_in, color_in)) < 0.5)
        return texture(u_SketchTexture, f_TexCoord);
    else
        return vec4(0.0, 0.0, 0.0, 1.0);
}

void main(void)
{
    // Quantize color
    float steps = 5;
    vec4 color = texture(u_Texture, f_TexCoord);
    color.x = quantize(color.x, steps);
    color.y = quantize(color.y, steps);
    color.z = quantize(color.z, steps);

    // Draw contours in image
    vec4 contour = texture(u_Contours, f_TexCoord);
    if (contour.x > 0.1)
        //color = vec4(1, 1, 1, 1.0); // White contour
        color = vec4(0, 0, 0, 1.0); // Black contour

    out_Color = color;
    //out_Color = sketchColor(color);
}
