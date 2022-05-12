#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 aOffset;

uniform float fogDensity;
uniform float fogGradient;
uniform mat4 scale;
uniform mat4 view;
uniform mat4 projection;

out float Visibility;
out vec2 TexCoords;

void main()
{
    TexCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
    vec3 rotated = vec3(aPos.x * cos(aOffset.w), aPos.y, aPos.z * sin(aOffset.w));
    gl_Position = projection * view * scale * vec4(rotated + aOffset.xyz, 1.0);
    // fog
    float distance = length(view * scale * vec4(rotated + aOffset.xyz, 1.0));
    Visibility = exp(-pow((distance*fogDensity), fogGradient));
    Visibility = clamp(Visibility, 0.0, 1.0);
}