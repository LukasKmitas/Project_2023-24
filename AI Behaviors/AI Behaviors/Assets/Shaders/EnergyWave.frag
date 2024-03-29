uniform float time;
uniform sampler2D texture;

float electricArc(float x, float y, float t) 
{
    float value = sin(x * 20.0 + t * 4.0) * cos(y * 20.0 + t * 6.0) * sin(t * 2.0);
    return smoothstep(0.3, 0.7, value);
}

void main() 
{
    vec2 uv = gl_TexCoord[0].xy;
    vec4 baseColor = texture2D(texture, uv);
    float pulse = sin(time * 3.0) * 0.5 + 0.5;
    float electric = electricArc(uv.x, uv.y, time);
    vec4 color = vec4(0.1, 0.2, 0.7, 1.0) * pulse + vec4(0.8, 0.8, 1.0, 1.0) * electric * 1.5;
    gl_FragColor = vec4(baseColor.rgb * color.rgb, baseColor.a);
}
