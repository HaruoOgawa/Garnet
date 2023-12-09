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
    JointIndexOffset: i32,
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

var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inWeights0_1: vec4<f32>;
@group(0) @binding(1) 
var<storage> r_SkinMatrixBuffer: SkinMatrixBuffer;
var<private> inJoint0_1: vec4<u32>;
var<private> inPosition_1: vec3<f32>;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
var<private> f_WorldNormal: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;
var<private> f_WorldTangent: vec3<f32>;
var<private> f_WorldBioTangent: vec3<f32>;
var<private> f_LightSpacePos: vec4<f32>;

fn main_1() {
    var BioTangent: vec3<f32>;
    var StartSkinMatIndex: i32;
    var SkinMat: mat4x4<f32>;
    var WorldPos: vec4<f32>;
    var WorldNormal: vec3<f32>;
    var WorldTangent: vec3<f32>;
    var WorldBioTangent: vec3<f32>;

    let _e38 = inNormal_1;
    let _e39 = inTangent_1;
    BioTangent = cross(_e38, _e39.xyz);
    let _e43 = ubo.useSkinMeshAnimation;
    if (_e43 != 0) {
        StartSkinMatIndex = 0;
        let _e46 = inWeights0_1[0u];
        let _e48 = inJoint0_1[0u];
        let _e50 = ubo.JointIndexOffset;
        let _e55 = r_SkinMatrixBuffer.SkinMat[(_e48 + bitcast<u32>(_e50))];
        let _e56 = (_e55 * _e46);
        let _e58 = inWeights0_1[1u];
        let _e60 = inJoint0_1[1u];
        let _e62 = ubo.JointIndexOffset;
        let _e67 = r_SkinMatrixBuffer.SkinMat[(_e60 + bitcast<u32>(_e62))];
        let _e68 = (_e67 * _e58);
        let _e81 = mat4x4<f32>((_e56[0] + _e68[0]), (_e56[1] + _e68[1]), (_e56[2] + _e68[2]), (_e56[3] + _e68[3]));
        let _e83 = inWeights0_1[2u];
        let _e85 = inJoint0_1[2u];
        let _e87 = ubo.JointIndexOffset;
        let _e92 = r_SkinMatrixBuffer.SkinMat[(_e85 + bitcast<u32>(_e87))];
        let _e93 = (_e92 * _e83);
        let _e106 = mat4x4<f32>((_e81[0] + _e93[0]), (_e81[1] + _e93[1]), (_e81[2] + _e93[2]), (_e81[3] + _e93[3]));
        let _e108 = inWeights0_1[3u];
        let _e110 = inJoint0_1[3u];
        let _e112 = ubo.JointIndexOffset;
        let _e117 = r_SkinMatrixBuffer.SkinMat[(_e110 + bitcast<u32>(_e112))];
        let _e118 = (_e117 * _e108);
        SkinMat = mat4x4<f32>((_e106[0] + _e118[0]), (_e106[1] + _e118[1]), (_e106[2] + _e118[2]), (_e106[3] + _e118[3]));
        let _e132 = SkinMat;
        let _e133 = inPosition_1;
        WorldPos = (_e132 * vec4<f32>(_e133.x, _e133.y, _e133.z, 1.0));
        let _e139 = SkinMat;
        let _e140 = inNormal_1;
        WorldNormal = normalize((_e139 * vec4<f32>(_e140.x, _e140.y, _e140.z, 0.0)).xyz);
        let _e148 = SkinMat;
        let _e149 = inTangent_1;
        WorldTangent = normalize((_e148 * _e149).xyz);
        let _e153 = SkinMat;
        let _e154 = BioTangent;
        WorldBioTangent = normalize((_e153 * vec4<f32>(_e154.x, _e154.y, _e154.z, 0.0)).xyz);
    } else {
        let _e163 = ubo.model;
        let _e164 = inPosition_1;
        WorldPos = (_e163 * vec4<f32>(_e164.x, _e164.y, _e164.z, 1.0));
        let _e171 = ubo.model;
        let _e172 = inNormal_1;
        WorldNormal = normalize((_e171 * vec4<f32>(_e172.x, _e172.y, _e172.z, 0.0)).xyz);
        let _e181 = ubo.model;
        let _e182 = inTangent_1;
        WorldTangent = normalize((_e181 * _e182).xyz);
        let _e187 = ubo.model;
        let _e188 = BioTangent;
        WorldBioTangent = normalize((_e187 * vec4<f32>(_e188.x, _e188.y, _e188.z, 0.0)).xyz);
    }
    let _e197 = ubo.proj;
    let _e199 = ubo.view;
    let _e201 = WorldPos;
    perVertexStruct.gl_Position = ((_e197 * _e199) * _e201);
    let _e204 = WorldNormal;
    f_WorldNormal = _e204;
    let _e205 = inTexcoord_1;
    f_Texcoord = _e205;
    let _e206 = WorldPos;
    f_WorldPos = _e206;
    let _e207 = WorldTangent;
    f_WorldTangent = _e207;
    let _e208 = WorldBioTangent;
    f_WorldBioTangent = _e208;
    let _e210 = ubo.lightVPMat;
    let _e211 = WorldPos;
    f_LightSpacePos = (_e210 * _e211);
    return;
}

@vertex 
fn main(@location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(5) inWeights0_: vec4<f32>, @location(4) inJoint0_: vec4<u32>, @location(0) inPosition: vec3<f32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inWeights0_1 = inWeights0_;
    inJoint0_1 = inJoint0_;
    inPosition_1 = inPosition;
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
