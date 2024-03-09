struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightVPMat: mat4x4<f32>,
    edgeSize: f32,
    MorphWeight_0_: f32,
    MorphWeight_1_: f32,
    fPad0_: f32,
    useSkinMeshAnimation: i32,
    useMorph: i32,
    drawPathIndex: i32,
    pad1_: i32,
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
    @location(6) member_6: vec2<f32>,
}

var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
var<private> inPosition_1: vec3<f32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inMorphVec0_1: vec3<f32>;
var<private> inMorphVec1_1: vec3<f32>;
var<private> inWeights0_1: vec4<f32>;
@group(0) @binding(1) 
var<storage> r_SkinMatrixBuffer: SkinMatrixBuffer;
var<private> inBone0_1: vec4<u32>;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
var<private> f_WorldNormal: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;
var<private> f_WorldTangent: vec3<f32>;
var<private> f_WorldBioTangent: vec3<f32>;
var<private> f_LightSpacePos: vec4<f32>;
var<private> f_SphereUV: vec2<f32>;

fn main_1() {
    var BioTangent: vec3<f32>;
    var LocalPos: vec3<f32>;
    var SkinMat: mat4x4<f32>;
    var WorldPos: vec4<f32>;
    var WorldNormal: vec3<f32>;
    var WorldTangent: vec3<f32>;
    var WorldBioTangent: vec3<f32>;
    var VNormal: vec4<f32>;
    var SphereUV: vec2<f32>;
    var ViewSpaceOutline: bool;
    var CameraPos: vec4<f32>;
    var CameraNormal: vec3<f32>;

    let _e53 = inNormal_1;
    let _e54 = inTangent_1;
    BioTangent = cross(_e53, _e54.xyz);
    let _e57 = inPosition_1;
    LocalPos = _e57;
    let _e59 = ubo.useMorph;
    if (_e59 != 0) {
        let _e61 = inMorphVec0_1;
        let _e63 = ubo.MorphWeight_0_;
        let _e65 = inMorphVec1_1;
        let _e67 = ubo.MorphWeight_1_;
        let _e70 = LocalPos;
        LocalPos = (_e70 + ((_e61 * _e63) + (_e65 * _e67)));
    }
    let _e73 = ubo.useSkinMeshAnimation;
    if (_e73 != 0) {
        let _e76 = inWeights0_1[0u];
        let _e78 = inBone0_1[0u];
        let _e81 = r_SkinMatrixBuffer.SkinMat[_e78];
        let _e82 = (_e81 * _e76);
        let _e84 = inWeights0_1[1u];
        let _e86 = inBone0_1[1u];
        let _e89 = r_SkinMatrixBuffer.SkinMat[_e86];
        let _e90 = (_e89 * _e84);
        let _e103 = mat4x4<f32>((_e82[0] + _e90[0]), (_e82[1] + _e90[1]), (_e82[2] + _e90[2]), (_e82[3] + _e90[3]));
        let _e105 = inWeights0_1[2u];
        let _e107 = inBone0_1[2u];
        let _e110 = r_SkinMatrixBuffer.SkinMat[_e107];
        let _e111 = (_e110 * _e105);
        let _e124 = mat4x4<f32>((_e103[0] + _e111[0]), (_e103[1] + _e111[1]), (_e103[2] + _e111[2]), (_e103[3] + _e111[3]));
        let _e126 = inWeights0_1[3u];
        let _e128 = inBone0_1[3u];
        let _e131 = r_SkinMatrixBuffer.SkinMat[_e128];
        let _e132 = (_e131 * _e126);
        SkinMat = mat4x4<f32>((_e124[0] + _e132[0]), (_e124[1] + _e132[1]), (_e124[2] + _e132[2]), (_e124[3] + _e132[3]));
        let _e146 = SkinMat;
        let _e147 = LocalPos;
        WorldPos = (_e146 * vec4<f32>(_e147.x, _e147.y, _e147.z, 1.0));
        let _e153 = SkinMat;
        let _e154 = inNormal_1;
        WorldNormal = normalize((_e153 * vec4<f32>(_e154.x, _e154.y, _e154.z, 0.0)).xyz);
        let _e162 = SkinMat;
        let _e163 = inTangent_1;
        WorldTangent = normalize((_e162 * _e163).xyz);
        let _e167 = SkinMat;
        let _e168 = BioTangent;
        WorldBioTangent = normalize((_e167 * vec4<f32>(_e168.x, _e168.y, _e168.z, 0.0)).xyz);
    } else {
        let _e177 = ubo.model;
        let _e178 = LocalPos;
        WorldPos = (_e177 * vec4<f32>(_e178.x, _e178.y, _e178.z, 1.0));
        let _e185 = ubo.model;
        let _e186 = inNormal_1;
        WorldNormal = normalize((_e185 * vec4<f32>(_e186.x, _e186.y, _e186.z, 0.0)).xyz);
        let _e195 = ubo.model;
        let _e196 = inTangent_1;
        WorldTangent = normalize((_e195 * _e196).xyz);
        let _e201 = ubo.model;
        let _e202 = BioTangent;
        WorldBioTangent = normalize((_e201 * vec4<f32>(_e202.x, _e202.y, _e202.z, 0.0)).xyz);
    }
    let _e211 = ubo.view;
    let _e212 = WorldNormal;
    VNormal = (_e211 * vec4<f32>(_e212.x, _e212.y, _e212.z, 0.0));
    let _e218 = VNormal;
    SphereUV = ((_e218.xy * 0.5) + vec2<f32>(0.5));
    let _e224 = ubo.drawPathIndex;
    if (_e224 == 2) {
        ViewSpaceOutline = false;
        let _e226 = ViewSpaceOutline;
        if _e226 {
            let _e228 = ubo.view;
            let _e229 = WorldPos;
            CameraPos = (_e228 * _e229);
            let _e232 = ubo.view;
            let _e233 = WorldNormal;
            CameraNormal = (_e232 * vec4<f32>(_e233.x, _e233.y, _e233.z, 0.0)).xyz;
            let _e240 = CameraNormal;
            let _e244 = ubo.edgeSize;
            let _e247 = CameraPos;
            let _e249 = (_e247.xy + ((normalize(_e240).xy * _e244) * 0.0010000000474974513));
            CameraPos[0u] = _e249.x;
            CameraPos[1u] = _e249.y;
            let _e255 = ubo.proj;
            let _e256 = CameraPos;
            perVertexStruct.gl_Position = (_e255 * _e256);
        } else {
            let _e259 = WorldNormal;
            let _e262 = ubo.edgeSize;
            let _e265 = WorldPos;
            let _e267 = (_e265.xyz + ((normalize(_e259) * _e262) * 0.0010000000474974513));
            WorldPos[0u] = _e267.x;
            WorldPos[1u] = _e267.y;
            WorldPos[2u] = _e267.z;
            let _e275 = ubo.proj;
            let _e277 = ubo.view;
            let _e279 = WorldPos;
            perVertexStruct.gl_Position = ((_e275 * _e277) * _e279);
        }
    } else {
        let _e283 = ubo.proj;
        let _e285 = ubo.view;
        let _e287 = WorldPos;
        perVertexStruct.gl_Position = ((_e283 * _e285) * _e287);
    }
    let _e290 = WorldNormal;
    f_WorldNormal = _e290;
    let _e291 = inTexcoord_1;
    f_Texcoord = _e291;
    let _e292 = WorldPos;
    f_WorldPos = _e292;
    let _e293 = WorldTangent;
    f_WorldTangent = _e293;
    let _e294 = WorldBioTangent;
    f_WorldBioTangent = _e294;
    let _e296 = ubo.lightVPMat;
    let _e297 = WorldPos;
    f_LightSpacePos = (_e296 * _e297);
    let _e299 = SphereUV;
    f_SphereUV = _e299;
    return;
}

@vertex 
fn main(@location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(0) inPosition: vec3<f32>, @location(6) inMorphVec0_: vec3<f32>, @location(7) inMorphVec1_: vec3<f32>, @location(5) inWeights0_: vec4<f32>, @location(4) inBone0_: vec4<u32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inPosition_1 = inPosition;
    inMorphVec0_1 = inMorphVec0_;
    inMorphVec1_1 = inMorphVec1_;
    inWeights0_1 = inWeights0_;
    inBone0_1 = inBone0_;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e26 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e26);
    let _e28 = perVertexStruct.gl_Position;
    let _e29 = f_WorldNormal;
    let _e30 = f_Texcoord;
    let _e31 = f_WorldPos;
    let _e32 = f_WorldTangent;
    let _e33 = f_WorldBioTangent;
    let _e34 = f_LightSpacePos;
    let _e35 = f_SphereUV;
    return VertexOutput(_e28, _e29, _e30, _e31, _e32, _e33, _e34, _e35);
}
