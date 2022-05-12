#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in float Visibility;

uniform samplerCube skybox;

void main()
{    
    int amount = 400;
    vec3 grid_uv = round(TexCoords * float(amount)) / float(amount);
    FragColor = texture(skybox, grid_uv);
    FragColor = mix(vec4(1.0, 1.0, 1.0, 1.0), FragColor, 0.4);
}