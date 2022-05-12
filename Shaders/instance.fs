#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float Visibility;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoords);
    if(FragColor.a < 0.1) discard;
    FragColor = mix(vec4(0.8, 0.8, 0.8, 1.0), FragColor, Visibility);
}