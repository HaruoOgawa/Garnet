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
    var SkinMat: mat4x4<f32>;
    var WorldPos: vec4<f32>;
    var WorldNormal: vec3<f32>;
    var WorldTangent: vec3<f32>;
    var WorldBioTangent: vec3<f32>;

    let _e36 = inNormal_1;
    let _e37 = inTangent_1;
    BioTangent = cross(_e36, _e37.xyz);
    let _e41 = ubo.useSkinMeshAnimation;
    if (_e41 != 0) {
        let _e44 = inWeights0_1[0u];
        let _e46 = inJoint0_1[0u];
        let _e49 = r_SkinMatrixBuffer.SkinMat[_e46];
        let _e50 = (_e49 * _e44);
        let _e52 = inWeights0_1[1u];
        let _e54 = inJoint0_1[1u];
        let _e57 = r_SkinMatrixBuffer.SkinMat[_e54];
        let _e58 = (_e57 * _e52);
        let _e71 = mat4x4<f32>((_e50[0] + _e58[0]), (_e50[1] + _e58[1]), (_e50[2] + _e58[2]), (_e50[3] + _e58[3]));
        let _e73 = inWeights0_1[2u];
        let _e75 = inJoint0_1[2u];
        let _e78 = r_SkinMatrixBuffer.SkinMat[_e75];
        let _e79 = (_e78 * _e73);
        let _e92 = mat4x4<f32>((_e71[0] + _e79[0]), (_e71[1] + _e79[1]), (_e71[2] + _e79[2]), (_e71[3] + _e79[3]));
        let _e94 = inWeights0_1[3u];
        let _e96 = inJoint0_1[3u];
        let _e99 = r_SkinMatrixBuffer.SkinMat[_e96];
        let _e100 = (_e99 * _e94);
        SkinMat = mat4x4<f32>((_e92[0] + _e100[0]), (_e92[1] + _e100[1]), (_e92[2] + _e100[2]), (_e92[3] + _e100[3]));
        let _e114 = SkinMat;
        let _e115 = inPosition_1;
        WorldPos = (_e114 * vec4<f32>(_e115.x, _e115.y, _e115.z, 1.0));
        let _e121 = SkinMat;
        let _e122 = inNormal_1;
        WorldNormal = normalize((_e121 * vec4<f32>(_e122.x, _e122.y, _e122.z, 0.0)).xyz);
        let _e130 = SkinMat;
        let _e131 = inTangent_1;
        WorldTangent = normalize((_e130 * _e131).xyz);
        let _e135 = SkinMat;
        let _e136 = BioTangent;
        WorldBioTangent = normalize((_e135 * vec4<f32>(_e136.x, _e136.y, _e136.z, 0.0)).xyz);
    } else {
        let _e145 = ubo.model;
        let _e146 = inPosition_1;
        WorldPos = (_e145 * vec4<f32>(_e146.x, _e146.y, _e146.z, 1.0));
        let _e153 = ubo.model;
        let _e154 = inNormal_1;
        WorldNormal = normalize((_e153 * vec4<f32>(_e154.x, _e154.y, _e154.z, 0.0)).xyz);
        let _e163 = ubo.model;
        let _e164 = inTangent_1;
        WorldTangent = normalize((_e163 * _e164).xyz);
        let _e169 = ubo.model;
        let _e170 = BioTangent;
        WorldBioTangent = normalize((_e169 * vec4<f32>(_e170.x, _e170.y, _e170.z, 0.0)).xyz);
    }
    let _e179 = ubo.proj;
    let _e181 = ubo.view;
    let _e183 = WorldPos;
    perVertexStruct.gl_Position = ((_e179 * _e181) * _e183);
    let _e186 = WorldNormal;
    f_WorldNormal = _e186;
    let _e187 = inTexcoord_1;
    f_Texcoord = _e187;
    let _e188 = WorldPos;
    f_WorldPos = _e188;
    let _e189 = WorldTangent;
    f_WorldTangent = _e189;
    let _e190 = WorldBioTangent;
    f_WorldBioTangent = _e190;
    let _e192 = ubo.lightVPMat;
    let _e193 = WorldPos;
    f_LightSpacePos = (_e192 * _e193);
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
