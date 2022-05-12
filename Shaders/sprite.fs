#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float Visibility;

uniform sampler2D texture1;
uniform float ke;

void main()
{    
    int amount = 96;
    vec2 grid_uv = round(TexCoords * float(amount)) / float(amount);
    vec4 color = clamp(texture(texture1, grid_uv), 0.0, 1.0);
    if(color.a < 0.1) discard;
    if(ke > 0.99) {
        float x = max(color.x, max(color.y, color.z));
        FragColor = vec4(1.0);//vec4(vec3(x), 1.0);
    }
    else FragColor = mix(vec4(0.8, 0.8, 0.8, 1.0), color, Visibility);
    //FragColor = color;
}