#version 410 core

layout (location = 0) in vec3 aPos;         // world space

uniform mat4 view;
uniform mat4 projection;

out vec3 fTexCoord;

void main()
{
    fTexCoord = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0f);
    gl_Position = pos.xyww;
}