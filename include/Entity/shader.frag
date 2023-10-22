#version 330 core

struct Material {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   sampler2D texture_diffuse1;
   sampler2D texture_specular1;
   sampler2D texture_normal1;
   float shininess;
};

struct PointLight {
   vec3 position;          // defined in world space
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float kConstant;
   float kLinear;
   float kQuadratic;
};

struct DirectionalLight {
   vec3 direction;         // defined in world space

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

in vec3 Normal;
in mat3 TBN;
in vec3 FragPos;           // defined in world space
in vec2 TexCoord;
in vec4 color;

#define MAX_NUM_POINT_LIGHTS 4
uniform int numPointLights;
uniform PointLight pointLights[MAX_NUM_POINT_LIGHTS];

#define MAX_NUM_DIR_LIGHTS 1
uniform int numDirLights;
uniform DirectionalLight dirLights[MAX_NUM_DIR_LIGHTS];

uniform vec3 viewPos;      // defined in world space 
uniform Material material;
uniform bool useDiffuseColor;
uniform bool useSpecularColor;
uniform bool useNormalMap;

out vec4 FragColor;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 viewDir, vec3 normal, vec3 diffuseColor, vec3 specularColor)
{

   vec3 ambient =  diffuseColor * light.ambient;

   vec3 lightDir = normalize(-light.direction);
   vec3 diffuse = max(dot(lightDir, normal), 0.0f) * diffuseColor * light.diffuse;

   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
   vec3 specular = spec * specularColor * light.specular;

   return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 viewDir, vec3 normal, vec3 diffuseColor, vec3 specularColor)
{
   vec3 lightDir = normalize(light.position - FragPos);
   float lightDist = length(light.position - FragPos);
   float attenuation = 1.0f / (light.kConstant + light.kLinear * lightDist + light.kQuadratic * lightDist * lightDist);

   vec3 ambient = diffuseColor * light.ambient;
   vec3 diffuse = max(dot(lightDir, normal), 0.0f) * diffuseColor * light.diffuse;

   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
   vec3 specular = spec * specularColor * light.specular;

   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;

   return ambient + diffuse + specular;
}

void main()
{
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 diffuseColor = useDiffuseColor ? material.diffuse : vec3(texture(material.texture_diffuse1, TexCoord));
   vec3 specularColor = useSpecularColor ? material.specular : vec3(texture(material.texture_specular1, TexCoord));

   vec3 normal = Normal;
   if (useNormalMap) {
      normal = texture(material.texture_normal1, TexCoord).xyz;
      normal = normal * 2.0f - 1.0f;
      normal = normalize(TBN * normal);
   }

   vec3 colorRGB = vec3(0.0f, 0.0f, 0.0f);

   for (int i = 0; i < min(numDirLights, MAX_NUM_DIR_LIGHTS); i++) {
      colorRGB += calculateDirectionalLight(dirLights[i], viewDir, normal, diffuseColor, specularColor);
   }
   
   for (int i = 0; i < min(numPointLights, MAX_NUM_POINT_LIGHTS) ; i++) {
      colorRGB += calculatePointLight(pointLights[i], viewDir, normal, diffuseColor, specularColor);
   }
   
   FragColor = vec4(colorRGB, 1.0f);
}