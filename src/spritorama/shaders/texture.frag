#version 450 core

layout(location = 1) in vec2 texture_coord;

layout(binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 outColor;


void main()
{
    vec4 col = texture(tex, texture_coord);
    outColor = col;
}
