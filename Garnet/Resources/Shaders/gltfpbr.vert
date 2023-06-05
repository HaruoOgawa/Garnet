#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
    mat4 view;
    mat4 proj;

	vec4 lightDir;
	vec4 lightColor;
	vec4 cameraPos;

	vec4 baseColorFactor;
	vec4 emissiveFactor;

    float time;
    float metallicFactor;
    float roughnessFactor;
    int   useBaseColorTexture;

    int   useMetallicRoughnessTexture;
    int   useEmissiveTexture;
    int   useNormalTexture;
    int   useOcclusionTexture;
} ubo;

layout(location = 0) out vec3 f_WorldNormal;
layout(location = 1) out vec2 f_Texcoord;
layout(location = 2) out vec4 f_WorldPos;

#define rot(a) mat2(cos(a), -sin(a), sin(a), cos(a))

void main(){
    vec4 pos = vec4(inPosition, 1.0);
    pos.xy *= rot(ubo.time * 0.5);
    pos.xz *= rot(ubo.time * 0.5);
    pos.yz *= rot(ubo.time * 0.5);

    gl_Position = ubo.proj * ubo.view * ubo.model * pos;
    f_WorldNormal = (ubo.model * vec4(inNormal, 0.0)).xyz;
    f_Texcoord = inTexcoord;
    f_WorldPos = ubo.model * vec4(inPosition, 1.0);
}