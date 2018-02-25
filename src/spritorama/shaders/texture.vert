#version 450 core

layout(location = 0) in vec2 position;

layout(location = 1) out vec2 texture_coord;

vec2 uvs[4] = vec2[]
(
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0)
);


void main()
{
    texture_coord = uvs[gl_VertexID];
    gl_Position = vec4(position, 0.0, 1.0);
}
