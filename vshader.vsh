#version 300 es

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 v_texCoord;
out vec3 vertexPos;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0f);

    vertexPos = position;
}
