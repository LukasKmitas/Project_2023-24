uniform float time;
uniform sampler2D texture;

void main() 
{
    // Texture coordinates
    vec2 uv = gl_TexCoord[0].xy;
    // Sample the texture
    vec4 texColor = texture2D(texture, uv);
    
    // Create a simple color shift effect over time
    float shift = sin(time) * 0.5 + 0.5; // Oscillates between 0 and 1
    vec4 colorShift = vec4(shift, 1.0 - shift, texColor.b, texColor.a);
    
    // Output the color
    gl_FragColor = texColor * colorShift;
}
