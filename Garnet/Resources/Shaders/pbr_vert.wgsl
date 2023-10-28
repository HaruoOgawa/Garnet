struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightVPMat: mat4x4<f32>,
    lightDir: vec4<f32>,
    lightColor: vec4<f32>,
    cameraPos: vec4<f32>,
    baseColorFactor: vec4<f32>,
    emissiveFactor: vec4<f32>,
    time: f32,
    metallicFactor: f32,
    roughnessFactor: f32,
    normalMapScale: f32,
    occlusionStrength: f32,
    mipCount: f32,
    ShadowMapX: f32,
    ShadowMapY: f32,
    useBaseColorTexture: i32,
    useMetallicRoughnessTexture: i32,
    useEmissiveTexture: i32,
    useNormalTexture: i32,
    useOcclusionTexture: i32,
    useCubeMap: i32,
    useShadowMap: i32,
    useIBL: i32,
    useSkinMeshAnimation: i32,
    pad0_: i32,
    pad1_: i32,
    pad2_: i32,
}

struct SkinMatrixBuffer {
    SkinMat: array<mat4x4<f32>>,
}

struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32,1u>,
    gl_CullDistance: array<f32,1u>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
    @location(1) member_1: vec2<f32>,
    @location(2) member_2: vec4<f32>,
    @location(3) member_3: vec3<f32>,
    @location(4) member_4: vec3<f32>,
    @location(5) member_5: vec4<f32>,
}

var<private> inPosition_1: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inWeights0_1: vec4<f32>;
@group(0) @binding(1) 
var<storage> r_SkinMatrixBuffer: SkinMatrixBuffer;
var<private> inJoint0_1: vec4<u32>;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
var<private> f_WorldNormal: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;
var<private> f_WorldTangent: vec3<f32>;
var<private> f_WorldBioTangent: vec3<f32>;
var<private> f_LightSpacePos: vec4<f32>;

fn main_1() {
    var pos: vec4<f32>;
    var BioTangent: vec3<f32>;
    var StartSkinMatIndex: i32;
    var SkinMat: mat4x4<f32>;

    let _e34 = inPosition_1;
    pos = vec4<f32>(_e34.x, _e34.y, _e34.z, 1.0);
    let _e39 = inNormal_1;
    let _e40 = inTangent_1;
    BioTangent = cross(_e39, _e40.xyz);
    let _e44 = ubo.useSkinMeshAnimation;
    if (_e44 != 0) {
        StartSkinMatIndex = 0;
        let _e47 = inWeights0_1[0u];
        let _e49 = inJoint0_1[0u];
        let _e52 = r_SkinMatrixBuffer.SkinMat[_e49];
        let _e53 = (_e52 * _e47);
        let _e55 = inWeights0_1[1u];
        let _e57 = inJoint0_1[1u];
        let _e60 = r_SkinMatrixBuffer.SkinMat[_e57];
        let _e61 = (_e60 * _e55);
        let _e74 = mat4x4<f32>((_e53[0] + _e61[0]), (_e53[1] + _e61[1]), (_e53[2] + _e61[2]), (_e53[3] + _e61[3]));
        let _e76 = inWeights0_1[2u];
        let _e78 = inJoint0_1[2u];
        let _e81 = r_SkinMatrixBuffer.SkinMat[_e78];
        let _e82 = (_e81 * _e76);
        let _e95 = mat4x4<f32>((_e74[0] + _e82[0]), (_e74[1] + _e82[1]), (_e74[2] + _e82[2]), (_e74[3] + _e82[3]));
        let _e97 = inWeights0_1[3u];
        let _e99 = inJoint0_1[3u];
        let _e102 = r_SkinMatrixBuffer.SkinMat[_e99];
        let _e103 = (_e102 * _e97);
        SkinMat = mat4x4<f32>((_e95[0] + _e103[0]), (_e95[1] + _e103[1]), (_e95[2] + _e103[2]), (_e95[3] + _e103[3]));
        let _e117 = SkinMat;
        let _e118 = pos;
        pos = (_e117 * _e118);
    }
    let _e121 = ubo.proj;
    let _e123 = ubo.view;
    let _e126 = ubo.model;
    let _e128 = pos;
    perVertexStruct.gl_Position = (((_e121 * _e123) * _e126) * _e128);
    let _e132 = ubo.model;
    let _e133 = inNormal_1;
    f_WorldNormal = normalize((_e132 * vec4<f32>(_e133.x, _e133.y, _e133.z, 0.0)).xyz);
    let _e141 = inTexcoord_1;
    f_Texcoord = _e141;
    let _e143 = ubo.model;
    let _e144 = inPosition_1;
    f_WorldPos = (_e143 * vec4<f32>(_e144.x, _e144.y, _e144.z, 1.0));
    let _e151 = ubo.model;
    let _e152 = inTangent_1;
    f_WorldTangent = normalize((_e151 * _e152).xyz);
    let _e157 = ubo.model;
    let _e158 = BioTangent;
    f_WorldBioTangent = normalize((_e157 * vec4<f32>(_e158.x, _e158.y, _e158.z, 0.0)).xyz);
    let _e167 = ubo.lightVPMat;
    let _e169 = ubo.model;
    let _e171 = pos;
    f_LightSpacePos = ((_e167 * _e169) * _e171);
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(5) inWeights0_: vec4<f32>, @location(4) inJoint0_: vec4<u32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inWeights0_1 = inWeights0_;
    inJoint0_1 = inJoint0_;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e21 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e21);
    let _e23 = perVertexStruct.gl_Position;
    let _e24 = f_WorldNormal;
    let _e25 = f_Texcoord;
    let _e26 = f_WorldPos;
    let _e27 = f_WorldTangent;
    let _e28 = f_WorldBioTangent;
    let _e29 = f_LightSpacePos;
    return VertexOutput(_e23, _e24, _e25, _e26, _e27, _e28, _e29);
}
