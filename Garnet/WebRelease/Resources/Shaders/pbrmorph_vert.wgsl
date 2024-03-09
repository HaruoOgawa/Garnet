struct MorphUniformBufferObject {
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
    MorphWeight_0_: f32,
    MorphWeight_1_: f32,
    fPad0_: f32,
    fPad1_: f32,
    useBaseColorTexture: i32,
    useMetallicRoughnessTexture: i32,
    useEmissiveTexture: i32,
    useNormalTexture: i32,
    useOcclusionTexture: i32,
    useCubeMap: i32,
    useShadowMap: i32,
    useIBL: i32,
    useSkinMeshAnimation: i32,
    useDirCubemap: i32,
    useMorph: i32,
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
var<private> inPosition_1: vec3<f32>;
@group(0) @binding(0) 
var<uniform> ubo: MorphUniformBufferObject;
var<private> inMorphVec0_1: vec3<f32>;
var<private> inMorphVec1_1: vec3<f32>;
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
    var BioTangent: vec3<f32>;
    var LocalPos: vec3<f32>;
    var SkinMat: mat4x4<f32>;
    var WorldPos: vec4<f32>;
    var WorldNormal: vec3<f32>;
    var WorldTangent: vec3<f32>;
    var WorldBioTangent: vec3<f32>;

    let _e42 = inNormal_1;
    let _e43 = inTangent_1;
    BioTangent = cross(_e42, _e43.xyz);
    let _e46 = inPosition_1;
    LocalPos = _e46;
    let _e48 = ubo.useMorph;
    if (_e48 != 0) {
        let _e50 = inMorphVec0_1;
        let _e52 = ubo.MorphWeight_0_;
        let _e54 = inMorphVec1_1;
        let _e56 = ubo.MorphWeight_1_;
        let _e59 = LocalPos;
        LocalPos = (_e59 + ((_e50 * _e52) + (_e54 * _e56)));
    }
    let _e62 = ubo.useSkinMeshAnimation;
    if (_e62 != 0) {
        let _e65 = inWeights0_1[0u];
        let _e67 = inJoint0_1[0u];
        let _e70 = r_SkinMatrixBuffer.SkinMat[_e67];
        let _e71 = (_e70 * _e65);
        let _e73 = inWeights0_1[1u];
        let _e75 = inJoint0_1[1u];
        let _e78 = r_SkinMatrixBuffer.SkinMat[_e75];
        let _e79 = (_e78 * _e73);
        let _e92 = mat4x4<f32>((_e71[0] + _e79[0]), (_e71[1] + _e79[1]), (_e71[2] + _e79[2]), (_e71[3] + _e79[3]));
        let _e94 = inWeights0_1[2u];
        let _e96 = inJoint0_1[2u];
        let _e99 = r_SkinMatrixBuffer.SkinMat[_e96];
        let _e100 = (_e99 * _e94);
        let _e113 = mat4x4<f32>((_e92[0] + _e100[0]), (_e92[1] + _e100[1]), (_e92[2] + _e100[2]), (_e92[3] + _e100[3]));
        let _e115 = inWeights0_1[3u];
        let _e117 = inJoint0_1[3u];
        let _e120 = r_SkinMatrixBuffer.SkinMat[_e117];
        let _e121 = (_e120 * _e115);
        SkinMat = mat4x4<f32>((_e113[0] + _e121[0]), (_e113[1] + _e121[1]), (_e113[2] + _e121[2]), (_e113[3] + _e121[3]));
        let _e135 = SkinMat;
        let _e136 = LocalPos;
        WorldPos = (_e135 * vec4<f32>(_e136.x, _e136.y, _e136.z, 1.0));
        let _e142 = SkinMat;
        let _e143 = inNormal_1;
        WorldNormal = normalize((_e142 * vec4<f32>(_e143.x, _e143.y, _e143.z, 0.0)).xyz);
        let _e151 = SkinMat;
        let _e152 = inTangent_1;
        WorldTangent = normalize((_e151 * _e152).xyz);
        let _e156 = SkinMat;
        let _e157 = BioTangent;
        WorldBioTangent = normalize((_e156 * vec4<f32>(_e157.x, _e157.y, _e157.z, 0.0)).xyz);
    } else {
        let _e166 = ubo.model;
        let _e167 = LocalPos;
        WorldPos = (_e166 * vec4<f32>(_e167.x, _e167.y, _e167.z, 1.0));
        let _e174 = ubo.model;
        let _e175 = inNormal_1;
        WorldNormal = normalize((_e174 * vec4<f32>(_e175.x, _e175.y, _e175.z, 0.0)).xyz);
        let _e184 = ubo.model;
        let _e185 = inTangent_1;
        WorldTangent = normalize((_e184 * _e185).xyz);
        let _e190 = ubo.model;
        let _e191 = BioTangent;
        WorldBioTangent = normalize((_e190 * vec4<f32>(_e191.x, _e191.y, _e191.z, 0.0)).xyz);
    }
    let _e200 = ubo.proj;
    let _e202 = ubo.view;
    let _e204 = WorldPos;
    perVertexStruct.gl_Position = ((_e200 * _e202) * _e204);
    let _e207 = WorldNormal;
    f_WorldNormal = _e207;
    let _e208 = inTexcoord_1;
    f_Texcoord = _e208;
    let _e209 = WorldPos;
    f_WorldPos = _e209;
    let _e210 = WorldTangent;
    f_WorldTangent = _e210;
    let _e211 = WorldBioTangent;
    f_WorldBioTangent = _e211;
    let _e213 = ubo.lightVPMat;
    let _e214 = WorldPos;
    f_LightSpacePos = (_e213 * _e214);
    return;
}

@vertex 
fn main(@location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(0) inPosition: vec3<f32>, @location(6) inMorphVec0_: vec3<f32>, @location(7) inMorphVec1_: vec3<f32>, @location(5) inWeights0_: vec4<f32>, @location(4) inJoint0_: vec4<u32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inPosition_1 = inPosition;
    inMorphVec0_1 = inMorphVec0_;
    inMorphVec1_1 = inMorphVec1_;
    inWeights0_1 = inWeights0_;
    inJoint0_1 = inJoint0_;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e25 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e25);
    let _e27 = perVertexStruct.gl_Position;
    let _e28 = f_WorldNormal;
    let _e29 = f_Texcoord;
    let _e30 = f_WorldPos;
    let _e31 = f_WorldTangent;
    let _e32 = f_WorldBioTangent;
    let _e33 = f_LightSpacePos;
    return VertexOutput(_e27, _e28, _e29, _e30, _e31, _e32, _e33);
}
