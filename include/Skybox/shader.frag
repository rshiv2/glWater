#version 410 core

in vec3 fTexCoord;
uniform samplerCube skybox;
//uniform float exposure;
out vec4 FragColor;

void main()
{
    float exposure = 1.0;
    float gamma = 2.2;
    FragColor = texture(skybox, fTexCoord);
    FragColor.xyz = exposure * pow(FragColor.xyz, vec3(gamma));
}