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
    MorphWeight_2_: f32,
    MorphWeight_3_: f32,
    MorphWeight_4_: f32,
    MorphWeight_5_: f32,
    MorphWeight_6_: f32,
    MorphWeight_7_: f32,
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
var<private> inMorphVec2_1: vec3<f32>;
var<private> inMorphVec3_1: vec3<f32>;
var<private> inMorphVec4_1: vec3<f32>;
var<private> inMorphVec5_1: vec3<f32>;
var<private> inMorphVec6_1: vec3<f32>;
var<private> inMorphVec7_1: vec3<f32>;
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

    let _e54 = inNormal_1;
    let _e55 = inTangent_1;
    BioTangent = cross(_e54, _e55.xyz);
    let _e58 = inPosition_1;
    LocalPos = _e58;
    let _e60 = ubo.useMorph;
    if (_e60 != 0) {
        let _e62 = inMorphVec0_1;
        let _e64 = ubo.MorphWeight_0_;
        let _e66 = inMorphVec1_1;
        let _e68 = ubo.MorphWeight_1_;
        let _e71 = inMorphVec2_1;
        let _e73 = ubo.MorphWeight_2_;
        let _e76 = inMorphVec3_1;
        let _e78 = ubo.MorphWeight_3_;
        let _e81 = inMorphVec4_1;
        let _e83 = ubo.MorphWeight_4_;
        let _e86 = inMorphVec5_1;
        let _e88 = ubo.MorphWeight_5_;
        let _e91 = inMorphVec6_1;
        let _e93 = ubo.MorphWeight_6_;
        let _e96 = inMorphVec7_1;
        let _e98 = ubo.MorphWeight_7_;
        let _e101 = LocalPos;
        LocalPos = (_e101 + ((((((((_e62 * _e64) + (_e66 * _e68)) + (_e71 * _e73)) + (_e76 * _e78)) + (_e81 * _e83)) + (_e86 * _e88)) + (_e91 * _e93)) + (_e96 * _e98)));
    }
    let _e104 = ubo.useSkinMeshAnimation;
    if (_e104 != 0) {
        let _e107 = inWeights0_1[0u];
        let _e109 = inJoint0_1[0u];
        let _e112 = r_SkinMatrixBuffer.SkinMat[_e109];
        let _e113 = (_e112 * _e107);
        let _e115 = inWeights0_1[1u];
        let _e117 = inJoint0_1[1u];
        let _e120 = r_SkinMatrixBuffer.SkinMat[_e117];
        let _e121 = (_e120 * _e115);
        let _e134 = mat4x4<f32>((_e113[0] + _e121[0]), (_e113[1] + _e121[1]), (_e113[2] + _e121[2]), (_e113[3] + _e121[3]));
        let _e136 = inWeights0_1[2u];
        let _e138 = inJoint0_1[2u];
        let _e141 = r_SkinMatrixBuffer.SkinMat[_e138];
        let _e142 = (_e141 * _e136);
        let _e155 = mat4x4<f32>((_e134[0] + _e142[0]), (_e134[1] + _e142[1]), (_e134[2] + _e142[2]), (_e134[3] + _e142[3]));
        let _e157 = inWeights0_1[3u];
        let _e159 = inJoint0_1[3u];
        let _e162 = r_SkinMatrixBuffer.SkinMat[_e159];
        let _e163 = (_e162 * _e157);
        SkinMat = mat4x4<f32>((_e155[0] + _e163[0]), (_e155[1] + _e163[1]), (_e155[2] + _e163[2]), (_e155[3] + _e163[3]));
        let _e177 = SkinMat;
        let _e178 = LocalPos;
        WorldPos = (_e177 * vec4<f32>(_e178.x, _e178.y, _e178.z, 1.0));
        let _e184 = SkinMat;
        let _e185 = inNormal_1;
        WorldNormal = normalize((_e184 * vec4<f32>(_e185.x, _e185.y, _e185.z, 0.0)).xyz);
        let _e193 = SkinMat;
        let _e194 = inTangent_1;
        WorldTangent = normalize((_e193 * _e194).xyz);
        let _e198 = SkinMat;
        let _e199 = BioTangent;
        WorldBioTangent = normalize((_e198 * vec4<f32>(_e199.x, _e199.y, _e199.z, 0.0)).xyz);
    } else {
        let _e208 = ubo.model;
        let _e209 = LocalPos;
        WorldPos = (_e208 * vec4<f32>(_e209.x, _e209.y, _e209.z, 1.0));
        let _e216 = ubo.model;
        let _e217 = inNormal_1;
        WorldNormal = normalize((_e216 * vec4<f32>(_e217.x, _e217.y, _e217.z, 0.0)).xyz);
        let _e226 = ubo.model;
        let _e227 = inTangent_1;
        WorldTangent = normalize((_e226 * _e227).xyz);
        let _e232 = ubo.model;
        let _e233 = BioTangent;
        WorldBioTangent = normalize((_e232 * vec4<f32>(_e233.x, _e233.y, _e233.z, 0.0)).xyz);
    }
    let _e242 = ubo.proj;
    let _e244 = ubo.view;
    let _e246 = WorldPos;
    perVertexStruct.gl_Position = ((_e242 * _e244) * _e246);
    let _e249 = WorldNormal;
    f_WorldNormal = _e249;
    let _e250 = inTexcoord_1;
    f_Texcoord = _e250;
    let _e251 = WorldPos;
    f_WorldPos = _e251;
    let _e252 = WorldTangent;
    f_WorldTangent = _e252;
    let _e253 = WorldBioTangent;
    f_WorldBioTangent = _e253;
    let _e255 = ubo.lightVPMat;
    let _e256 = WorldPos;
    f_LightSpacePos = (_e255 * _e256);
    return;
}

@vertex 
fn main(@location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(0) inPosition: vec3<f32>, @location(6) inMorphVec0_: vec3<f32>, @location(7) inMorphVec1_: vec3<f32>, @location(8) inMorphVec2_: vec3<f32>, @location(9) inMorphVec3_: vec3<f32>, @location(10) inMorphVec4_: vec3<f32>, @location(11) inMorphVec5_: vec3<f32>, @location(12) inMorphVec6_: vec3<f32>, @location(13) inMorphVec7_: vec3<f32>, @location(5) inWeights0_: vec4<f32>, @location(4) inJoint0_: vec4<u32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inPosition_1 = inPosition;
    inMorphVec0_1 = inMorphVec0_;
    inMorphVec1_1 = inMorphVec1_;
    inMorphVec2_1 = inMorphVec2_;
    inMorphVec3_1 = inMorphVec3_;
    inMorphVec4_1 = inMorphVec4_;
    inMorphVec5_1 = inMorphVec5_;
    inMorphVec6_1 = inMorphVec6_;
    inMorphVec7_1 = inMorphVec7_;
    inWeights0_1 = inWeights0_;
    inJoint0_1 = inJoint0_;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e37 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e37);
    let _e39 = perVertexStruct.gl_Position;
    let _e40 = f_WorldNormal;
    let _e41 = f_Texcoord;
    let _e42 = f_WorldPos;
    let _e43 = f_WorldTangent;
    let _e44 = f_WorldBioTangent;
    let _e45 = f_LightSpacePos;
    return VertexOutput(_e39, _e40, _e41, _e42, _e43, _e44, _e45);
}
