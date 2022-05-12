#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out float Visibility;

uniform float fogDensity;
uniform float fogGradient;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
    // fog
    float distance = length(view * model * vec4(aPos, 1.0f));
    Visibility = exp(-pow((distance*fogDensity), fogGradient));
    Visibility = clamp(Visibility, 0.0, 1.0);
}