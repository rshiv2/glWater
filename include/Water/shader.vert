#version 410 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec4 clipSpace;       // clip space coordinates of vertex
out vec3 fragPos;
out vec2 dudvTexCoords;

void main()
{
    fragPos = (view * vec4(aPos, 1.0f)).xyz;
    clipSpace = projection * view * vec4(aPos, 1.0f);
    gl_Position = clipSpace;
    dudvTexCoords = aPos.xz;

}