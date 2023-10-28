#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;
layout(location = 3) in vec4 inTangent;
layout(location = 4) in uvec4 inJoint0;
layout(location = 5) in vec4 inWeights0;

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

    int   useSkinMeshAnimation;
    int   pad0;
    int   pad1;
    int   pad2;
} ubo;

readonly layout(std430, binding = 1) buffer SkinMatrixBuffer
{
    mat4 SkinMat[];
} r_SkinMatrixBuffer;

layout(location = 0) out vec3 f_WorldNormal;
layout(location = 1) out vec2 f_Texcoord;
layout(location = 2) out vec4 f_WorldPos;
layout(location = 3) out vec3 f_WorldTangent;
layout(location = 4) out vec3 f_WorldBioTangent;
layout(location = 5) out vec4 f_LightSpacePos;
layout(location = 6) out flat vec4 f_DebugColor;
layout(location = 7) out flat ivec4 f_DebugJoint;

#define rot(a) mat2(cos(a), -sin(a), sin(a), cos(a))

void main(){
    vec4 pos = vec4(inPosition, 1.0);
    vec3 BioTangent = cross(inNormal, inTangent.xyz);

    // スキンメッシュアニメーション
    ivec4 joint = ivec4(inJoint0);
    if(ubo.useSkinMeshAnimation != 0)
    {
        // あとでUniform実装. スキンが複数個になった時対応する必要がある
        // 使用するスキンのSkinMatの開始インデックスが入っており、これをもとにSkinMatrixBufferをオフセットさせる
        // でも複数スキン対応はあとでその他いろいろとよく検討すべき
        int StartSkinMatIndex = 0;

        mat4 SkinMat =
            inWeights0.x * r_SkinMatrixBuffer.SkinMat[joint.x] +
            inWeights0.y * r_SkinMatrixBuffer.SkinMat[joint.y] +
            inWeights0.z * r_SkinMatrixBuffer.SkinMat[joint.z] +
            inWeights0.w * r_SkinMatrixBuffer.SkinMat[joint.w] 
        ;

        pos = SkinMat * pos;
    }

    //
    gl_Position = ubo.proj * ubo.view * ubo.model * pos;
    f_WorldNormal = normalize((ubo.model * vec4(inNormal, 0.0)).xyz);
    f_Texcoord = inTexcoord;
    f_WorldPos = ubo.model * vec4(inPosition, 1.0);
    f_WorldTangent = normalize((ubo.model * inTangent).xyz);
    f_WorldBioTangent = normalize((ubo.model * vec4(BioTangent, 0.0)).xyz);
    f_LightSpacePos = ubo.lightVPMat * ubo.model * pos;

    vec4 dcol = vec4(inWeights0.rgb, 1.0);
    if(joint.x > 1 || joint.y > 1 || joint.z > 1 || joint.w > 1)
    {
        dcol = vec4(0.0, 0.0, 1.0, 1.0);
    }

    f_DebugColor = dcol;
    f_DebugJoint = joint;
}