#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 invTransposeModel;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 reflectionClippingPlane;

uniform float texCoordScale;  // shrink or magnify texture (useful for textures that wrap)

out vec3 Normal;
out mat3 TBN;
out vec3 FragPos;
out vec2 TexCoord;

void main()
{
   Normal = normalize(mat3(invTransposeModel) * aNormal);   // computed in world space
   vec3 Tangent = normalize(mat3(invTransposeModel) * aTangent);
   vec3 Bitangent = normalize(mat3(invTransposeModel) * aBitangent);
   TBN = mat3(Tangent, Bitangent, Normal);

   TexCoord = aTexCoord * texCoordScale;
   vec4 worldPos = model * vec4(aPos, 1.0f);
   gl_ClipDistance[0] = dot(worldPos, reflectionClippingPlane);
   FragPos = worldPos.xyz;                                  // computed in world space
   gl_Position = projection * view * worldPos;

}