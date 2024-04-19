uniform sampler2D texture;
uniform float time;
uniform vec2 center;
uniform float frequency; 
uniform float amplitude; 

void main()
 {
    vec2 texCoords = gl_TexCoord[0].xy;

    float distanceFromCenter = distance(texCoords, center);
    float ripple = sin(distanceFromCenter * frequency - time) * amplitude;

    vec2 adjustedTexCoords = texCoords + normalize(texCoords - center) * ripple;

    gl_FragColor = texture2D(texture, adjustedTexCoords);
}
