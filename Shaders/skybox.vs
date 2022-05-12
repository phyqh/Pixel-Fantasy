#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out float Visibility;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
   
    // fog
    float distance = length(view * vec4(aPos, 1.0));
    Visibility = exp(-pow((distance*0.05), 1.5));
    Visibility = clamp(Visibility, 0.0, 1.0);
}  