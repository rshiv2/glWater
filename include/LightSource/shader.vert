#version 410 core

layout (location = 0) in vec3 aPos;

uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 reflectionClippingPlane;

out vec3 fColor;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0f);
    gl_ClipDistance[0] = dot(reflectionClippingPlane, worldPos);
    gl_Position = projection * view * worldPos; 
    fColor = color;
}