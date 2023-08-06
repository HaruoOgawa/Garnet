#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform TestBuffer{
    vec4 UBOColor;
    float time;
    float pad_0;
    float pad_1;
    float pad_2;

} testUBO;

#ifdef USE_OPENGL
layout(binding = 2) uniform sampler2D u_texture;
layout(binding = 4) uniform sampler2D u_NormalTexture;
layout(binding = 11) uniform samplerCube cubemapTexture;
#else
layout(binding = 2) uniform texture2D u_texture;
layout(binding = 3) uniform sampler u_sampler;

layout(binding = 4) uniform texture2D u_NormalTexture;
layout(binding = 5) uniform sampler u_NormalSampler;
#endif

layout(location = 0) out vec4 outColor;

void main() {
    vec3 col = vec3(0.0);
    col.rg = fragTexCoord;

#ifdef USE_OPENGL
    col = mix(texture(u_texture, fragTexCoord).rgb, texture(u_NormalTexture, fragTexCoord).rgb, sin(testUBO.time) * 0.5 + 0.5);
#else
    col = mix(texture(sampler2D(u_texture, u_sampler), fragTexCoord).rgb, texture(sampler2D(u_NormalTexture, u_NormalSampler), fragTexCoord).rgb, sin(testUBO.time) * 0.5 + 0.5);
#endif

    col += texture(cubemapTexture, vec3(0.0)).rgb;

    outColor = vec4(col, 1.0);
}