#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;
layout(location = 3) in vec4 inTangent;
layout(location = 4) in vec4 inBioTangent;

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
    float normalMapScale;

    float occlusionStrength;
    float mipCount;
    float s_pad1;
    float s_pad2;

    int   useBaseColorTexture;
    int   useMetallicRoughnessTexture;
    int   useEmissiveTexture;
    int   useNormalTexture;
    
    int   useOcclusionTexture;
    int   t_pad_0;
    int   t_pad_1;
    int   t_pad_2;
} ubo;

layout(location = 0) out vec3 f_WorldNormal;
layout(location = 1) out vec2 f_Texcoord;
layout(location = 2) out vec4 f_WorldPos;
layout(location = 3) out vec3 f_WorldTangent;
layout(location = 4) out vec3 f_WorldBioTangent;

#define rot(a) mat2(cos(a), -sin(a), sin(a), cos(a))

void main(){
    vec4 pos = vec4(inPosition, 1.0);

    gl_Position = ubo.proj * ubo.view * ubo.model * pos;
    f_WorldNormal = normalize((ubo.model * vec4(inNormal, 0.0)).xyz);
    f_Texcoord = inTexcoord;
    f_WorldPos = ubo.model * vec4(inPosition, 1.0);
    f_WorldTangent = normalize((ubo.model * inTangent).xyz);
    f_WorldBioTangent = normalize((ubo.model * inBioTangent).xyz);
}