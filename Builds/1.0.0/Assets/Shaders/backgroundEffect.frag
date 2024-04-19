uniform sampler2D texture;
uniform float time;

void main() 
{
    vec2 uv = gl_TexCoord[0].xy;
    
    uv.y += sin(uv.x * 2.0 + time) * 0.005;
    uv.x += cos(uv.y * 2.0 + time) * 0.005;

    vec4 texColor = texture2D(texture, uv);

    gl_FragColor = texColor;
}
