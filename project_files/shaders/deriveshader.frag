#version 150

in vec2 f_TexCoord;
uniform sampler2D u_DepthTexture;
out vec4 out_Color;
// Add texture boundaries...

void main(void)
{
    float texSize = 512.0; // Should be a uniform...
    // Distance between two pixels
    float offset = 1.0/texSize;

    // Filter center
    vec2 tc = f_TexCoord;
    vec4 c = texture(u_DepthTexture, tc);

    // Filter left
    tc.x = tc.x - offset;
    vec4 l = texture(u_DepthTexture, tc);

    // Filter right
    tc.x = tc.x + 2.0 * offset;
    vec4 r = texture(u_DepthTexture, tc);

    // Filter up
    tc.x = tc.x - 1.0 * offset;
    tc.y = tc.y - 1.0 * offset;
    vec4 u = texture(u_DepthTexture, tc);

    // Filter down
    tc.y = tc.y + 2.0 * offset;
    vec4 d = texture(u_DepthTexture, tc);

    vec4 der_x = -l + r;
    vec4 der_y = -u + d;

    float der_tot = sqrt(der_x.x * der_x.x + der_y.x * der_y.x)*20;
    out_Color = vec4(der_tot, der_tot, der_tot, 1.0);

}
