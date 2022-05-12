#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform float pixelization;

void main()
{    
    vec2 grid_uv = round(TexCoords * pixelization) / pixelization;
    vec4 color = clamp(texture(texture1, grid_uv), 0.0, 1.0);
    if(color.a < 0.1) discard;
    FragColor = color;
}