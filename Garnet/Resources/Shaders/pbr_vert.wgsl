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
    @location(6) @interpolate(flat) member_6: vec4<f32>,
    @location(7) @interpolate(flat) member_7: vec4<i32>,
}

var<private> inPosition_1: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
var<private> inJoint0_1: vec4<u32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inWeights0_1: vec4<f32>;
@group(0) @binding(1) 
var<storage> r_SkinMatrixBuffer: SkinMatrixBuffer;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
var<private> f_WorldNormal: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;
var<private> f_WorldTangent: vec3<f32>;
var<private> f_WorldBioTangent: vec3<f32>;
var<private> f_LightSpacePos: vec4<f32>;
var<private> f_DebugColor: vec4<f32>;
var<private> f_DebugJoint: vec4<i32>;

fn main_1() {
    var pos: vec4<f32>;
    var BioTangent: vec3<f32>;
    var joint: vec4<i32>;
    var StartSkinMatIndex: i32;
    var SkinMat: mat4x4<f32>;
    var dcol: vec4<f32>;
    var phi_225_: bool;
    var phi_232_: bool;
    var phi_239_: bool;

    let _e39 = inPosition_1;
    pos = vec4<f32>(_e39.x, _e39.y, _e39.z, 1.0);
    let _e44 = inNormal_1;
    let _e45 = inTangent_1;
    BioTangent = cross(_e44, _e45.xyz);
    let _e48 = inJoint0_1;
    joint = bitcast<vec4<i32>>(_e48);
    let _e51 = ubo.useSkinMeshAnimation;
    if (_e51 != 0) {
        StartSkinMatIndex = 0;
        let _e54 = inWeights0_1[0u];
        let _e56 = joint[0u];
        let _e59 = r_SkinMatrixBuffer.SkinMat[_e56];
        let _e60 = (_e59 * _e54);
        let _e62 = inWeights0_1[1u];
        let _e64 = joint[1u];
        let _e67 = r_SkinMatrixBuffer.SkinMat[_e64];
        let _e68 = (_e67 * _e62);
        let _e81 = mat4x4<f32>((_e60[0] + _e68[0]), (_e60[1] + _e68[1]), (_e60[2] + _e68[2]), (_e60[3] + _e68[3]));
        let _e83 = inWeights0_1[2u];
        let _e85 = joint[2u];
        let _e88 = r_SkinMatrixBuffer.SkinMat[_e85];
        let _e89 = (_e88 * _e83);
        let _e102 = mat4x4<f32>((_e81[0] + _e89[0]), (_e81[1] + _e89[1]), (_e81[2] + _e89[2]), (_e81[3] + _e89[3]));
        let _e104 = inWeights0_1[3u];
        let _e106 = joint[3u];
        let _e109 = r_SkinMatrixBuffer.SkinMat[_e106];
        let _e110 = (_e109 * _e104);
        SkinMat = mat4x4<f32>((_e102[0] + _e110[0]), (_e102[1] + _e110[1]), (_e102[2] + _e110[2]), (_e102[3] + _e110[3]));
        let _e124 = SkinMat;
        let _e125 = pos;
        pos = (_e124 * _e125);
    }
    let _e128 = ubo.proj;
    let _e130 = ubo.view;
    let _e133 = ubo.model;
    let _e135 = pos;
    perVertexStruct.gl_Position = (((_e128 * _e130) * _e133) * _e135);
    let _e139 = ubo.model;
    let _e140 = inNormal_1;
    f_WorldNormal = normalize((_e139 * vec4<f32>(_e140.x, _e140.y, _e140.z, 0.0)).xyz);
    let _e148 = inTexcoord_1;
    f_Texcoord = _e148;
    let _e150 = ubo.model;
    let _e151 = inPosition_1;
    f_WorldPos = (_e150 * vec4<f32>(_e151.x, _e151.y, _e151.z, 1.0));
    let _e158 = ubo.model;
    let _e159 = inTangent_1;
    f_WorldTangent = normalize((_e158 * _e159).xyz);
    let _e164 = ubo.model;
    let _e165 = BioTangent;
    f_WorldBioTangent = normalize((_e164 * vec4<f32>(_e165.x, _e165.y, _e165.z, 0.0)).xyz);
    let _e174 = ubo.lightVPMat;
    let _e176 = ubo.model;
    let _e178 = pos;
    f_LightSpacePos = ((_e174 * _e176) * _e178);
    let _e180 = inWeights0_1;
    let _e181 = _e180.xyz;
    dcol = vec4<f32>(_e181.x, _e181.y, _e181.z, 1.0);
    let _e187 = joint[0u];
    let _e188 = (_e187 > 1);
    phi_225_ = _e188;
    if !(_e188) {
        let _e191 = joint[1u];
        phi_225_ = (_e191 > 1);
    }
    let _e194 = phi_225_;
    phi_232_ = _e194;
    if !(_e194) {
        let _e197 = joint[2u];
        phi_232_ = (_e197 > 1);
    }
    let _e200 = phi_232_;
    phi_239_ = _e200;
    if !(_e200) {
        let _e203 = joint[3u];
        phi_239_ = (_e203 > 1);
    }
    let _e206 = phi_239_;
    if _e206 {
        dcol = vec4<f32>(0.0, 0.0, 1.0, 1.0);
    }
    let _e207 = dcol;
    f_DebugColor = _e207;
    let _e208 = joint;
    f_DebugJoint = _e208;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(4) inJoint0_: vec4<u32>, @location(5) inWeights0_: vec4<f32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inJoint0_1 = inJoint0_;
    inWeights0_1 = inWeights0_;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e23 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e23);
    let _e25 = perVertexStruct.gl_Position;
    let _e26 = f_WorldNormal;
    let _e27 = f_Texcoord;
    let _e28 = f_WorldPos;
    let _e29 = f_WorldTangent;
    let _e30 = f_WorldBioTangent;
    let _e31 = f_LightSpacePos;
    let _e32 = f_DebugColor;
    let _e33 = f_DebugJoint;
    return VertexOutput(_e25, _e26, _e27, _e28, _e29, _e30, _e31, _e32, _e33);
}
