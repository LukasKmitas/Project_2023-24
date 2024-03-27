uniform float time;
uniform vec4 outlineColor;

void main()
{
    float pulsing = (sin(time * 2.0) + 1.0) / 2.0;
    gl_FragColor = vec4(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a * pulsing);
}
