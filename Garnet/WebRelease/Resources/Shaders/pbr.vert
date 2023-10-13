#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;
layout(location = 3) in vec4 inTangent;

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
    mat4 view;
    mat4 proj;
	mat4 lightVPMat;

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
    float ShadowMapX;
    float ShadowMapY;

    int   useBaseColorTexture;
    int   useMetallicRoughnessTexture;
    int   useEmissiveTexture;
    int   useNormalTexture;
    
    int   useOcclusionTexture;
    int   useCubeMap;
    int   useShadowMap;
    int   useIBL;
} ubo;

layout(location = 0) out vec3 f_WorldNormal;
layout(location = 1) out vec2 f_Texcoord;
layout(location = 2) out vec4 f_WorldPos;
layout(location = 3) out vec3 f_WorldTangent;
layout(location = 4) out vec3 f_WorldBioTangent;
layout(location = 5) out vec4 f_LightSpacePos;

#define rot(a) mat2(cos(a), -sin(a), sin(a), cos(a))

void main(){
    vec4 pos = vec4(inPosition, 1.0);
    vec3 BioTangent = cross(inNormal, inTangent.xyz);

    gl_Position = ubo.proj * ubo.view * ubo.model * pos;
    f_WorldNormal = normalize((ubo.model * vec4(inNormal, 0.0)).xyz);
    f_Texcoord = inTexcoord;
    f_WorldPos = ubo.model * vec4(inPosition, 1.0);
    f_WorldTangent = normalize((ubo.model * inTangent).xyz);
    f_WorldBioTangent = normalize((ubo.model * vec4(BioTangent, 0.0)).xyz);
    f_LightSpacePos = ubo.lightVPMat * ubo.model * pos;
}