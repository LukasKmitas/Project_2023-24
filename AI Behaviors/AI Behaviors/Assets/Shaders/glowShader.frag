uniform sampler2D texture;
uniform float glowRadius;
uniform vec4 glowColor;

void main() 
{
    vec2 texCoords = gl_TexCoord[0].xy;
    vec4 texColor = texture2D(texture, texCoords);
    float glow = glowRadius * (10.0 - length(gl_TexCoord[0].xy - vec2(0.5, 0.5)) * 2.0);
    gl_FragColor = texColor + glowColor * glow;
}
