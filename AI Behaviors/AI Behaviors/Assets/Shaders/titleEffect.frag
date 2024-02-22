uniform sampler2D texture;
uniform vec3 fillColor;
uniform float time;

void main() 
{
    vec2 uv = gl_TexCoord[0].xy;
    vec4 texColor = texture2D(texture, uv);

    vec3 goldColor1 = vec3(1.0, 0.84, 0); // Lighter gold
    vec3 goldColor2 = vec3(0.8, 0.68, 0.2); // Darker gold

    // Create a moving wave pattern
    float wave = sin(uv.x * 10.0 + time * 2.0) * 0.6 + 0.6;

    vec3 goldWaveColor = mix(goldColor1, goldColor2, wave);

    vec3 colorBlend = mix(fillColor, goldWaveColor, wave * texColor.a);

    gl_FragColor = vec4(colorBlend * texColor.a, texColor.a);
}
