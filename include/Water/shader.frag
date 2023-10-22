#version 410 core

in vec4 clipSpace;
in vec2 dudvTexCoords;
in vec3 fragPos;
out vec4 FragColor;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D depthTexture;

uniform float nearPlane;
uniform float farPlane;
uniform float fresnelFactor;
uniform float waveMoveFactor;
uniform vec2 waveDir;

float ndcToWorldDepth(float depth, float nearPlane, float farPlane)
{
    return 2.0 * nearPlane * farPlane / (farPlane + nearPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));
}

void main()
{
    vec2 ndc = clipSpace.xy / clipSpace.w;
    ndc = (ndc + 1) * 0.5f;

    vec2 reflectTexCoord = vec2(1 - ndc.x, ndc.y);
    vec2 refractTexCoord = ndc;

    float totalDepth = texture(depthTexture, refractTexCoord).x;
    totalDepth = ndcToWorldDepth(totalDepth, nearPlane, farPlane);

    float fragDepth = ndcToWorldDepth(gl_FragCoord.z, nearPlane, farPlane);
    float waterDepth = totalDepth - fragDepth;

    float depthCutoff = 0.1f;
    float alpha = clamp(waterDepth / depthCutoff, 0.0f, 1.0f);

    float distortionStrength = 0.02f;
    vec2 distortion = (2.0f * texture(dudvMap, dudvTexCoords + waveMoveFactor * waveDir).rg - 1.0f) * distortionStrength *  clamp(waterDepth / 0.2, 0.0f, 1.0f);

    reflectTexCoord += distortion;
    refractTexCoord += distortion;

    reflectTexCoord = clamp(reflectTexCoord, .0001f, .9999f);   // seems to reduce dudv artifacts
    refractTexCoord = clamp(refractTexCoord, .0001f, .9999f);

    vec4 reflectionColor = texture(reflectionTexture, reflectTexCoord);
    vec4 refractionColor = texture(refractionTexture, refractTexCoord);
    
    FragColor = mix(refractionColor, reflectionColor, alpha);
    FragColor.a = alpha;

}