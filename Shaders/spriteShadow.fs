#version 330 core

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
    float alpha = texture(texture1, TexCoords).a;
    if (alpha < 0.1) discard;
}