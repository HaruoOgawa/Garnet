struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightVPMat: mat4x4<f32>,
    edgeSize: f32,
    fPad0_: f32,
    fPad1_: f32,
    fPad2_: f32,
    MorphWeight_0_: f32,
    MorphWeight_1_: f32,
    MorphWeight_2_: f32,
    MorphWeight_3_: f32,
    MorphWeight_4_: f32,
    MorphWeight_5_: f32,
    MorphWeight_6_: f32,
    MorphWeight_7_: f32,
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
var<private> inMorphVec2_1: vec3<f32>;
var<private> inMorphVec3_1: vec3<f32>;
var<private> inMorphVec4_1: vec3<f32>;
var<private> inMorphVec5_1: vec3<f32>;
var<private> inMorphVec6_1: vec3<f32>;
var<private> inMorphVec7_1: vec3<f32>;
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

    let _e65 = inNormal_1;
    let _e66 = inTangent_1;
    BioTangent = cross(_e65, _e66.xyz);
    let _e69 = inPosition_1;
    LocalPos = _e69;
    let _e71 = ubo.useMorph;
    if (_e71 != 0) {
        let _e73 = inMorphVec0_1;
        let _e75 = ubo.MorphWeight_0_;
        let _e77 = inMorphVec1_1;
        let _e79 = ubo.MorphWeight_1_;
        let _e82 = inMorphVec2_1;
        let _e84 = ubo.MorphWeight_2_;
        let _e87 = inMorphVec3_1;
        let _e89 = ubo.MorphWeight_3_;
        let _e92 = inMorphVec4_1;
        let _e94 = ubo.MorphWeight_4_;
        let _e97 = inMorphVec5_1;
        let _e99 = ubo.MorphWeight_5_;
        let _e102 = inMorphVec6_1;
        let _e104 = ubo.MorphWeight_6_;
        let _e107 = inMorphVec7_1;
        let _e109 = ubo.MorphWeight_7_;
        let _e112 = LocalPos;
        LocalPos = (_e112 + ((((((((_e73 * _e75) + (_e77 * _e79)) + (_e82 * _e84)) + (_e87 * _e89)) + (_e92 * _e94)) + (_e97 * _e99)) + (_e102 * _e104)) + (_e107 * _e109)));
    }
    let _e115 = ubo.useSkinMeshAnimation;
    if (_e115 != 0) {
        let _e118 = inWeights0_1[0u];
        let _e120 = inBone0_1[0u];
        let _e123 = r_SkinMatrixBuffer.SkinMat[_e120];
        let _e124 = (_e123 * _e118);
        let _e126 = inWeights0_1[1u];
        let _e128 = inBone0_1[1u];
        let _e131 = r_SkinMatrixBuffer.SkinMat[_e128];
        let _e132 = (_e131 * _e126);
        let _e145 = mat4x4<f32>((_e124[0] + _e132[0]), (_e124[1] + _e132[1]), (_e124[2] + _e132[2]), (_e124[3] + _e132[3]));
        let _e147 = inWeights0_1[2u];
        let _e149 = inBone0_1[2u];
        let _e152 = r_SkinMatrixBuffer.SkinMat[_e149];
        let _e153 = (_e152 * _e147);
        let _e166 = mat4x4<f32>((_e145[0] + _e153[0]), (_e145[1] + _e153[1]), (_e145[2] + _e153[2]), (_e145[3] + _e153[3]));
        let _e168 = inWeights0_1[3u];
        let _e170 = inBone0_1[3u];
        let _e173 = r_SkinMatrixBuffer.SkinMat[_e170];
        let _e174 = (_e173 * _e168);
        SkinMat = mat4x4<f32>((_e166[0] + _e174[0]), (_e166[1] + _e174[1]), (_e166[2] + _e174[2]), (_e166[3] + _e174[3]));
        let _e188 = SkinMat;
        let _e189 = LocalPos;
        WorldPos = (_e188 * vec4<f32>(_e189.x, _e189.y, _e189.z, 1.0));
        let _e195 = SkinMat;
        let _e196 = inNormal_1;
        WorldNormal = normalize((_e195 * vec4<f32>(_e196.x, _e196.y, _e196.z, 0.0)).xyz);
        let _e204 = SkinMat;
        let _e205 = inTangent_1;
        WorldTangent = normalize((_e204 * _e205).xyz);
        let _e209 = SkinMat;
        let _e210 = BioTangent;
        WorldBioTangent = normalize((_e209 * vec4<f32>(_e210.x, _e210.y, _e210.z, 0.0)).xyz);
    } else {
        let _e219 = ubo.model;
        let _e220 = LocalPos;
        WorldPos = (_e219 * vec4<f32>(_e220.x, _e220.y, _e220.z, 1.0));
        let _e227 = ubo.model;
        let _e228 = inNormal_1;
        WorldNormal = normalize((_e227 * vec4<f32>(_e228.x, _e228.y, _e228.z, 0.0)).xyz);
        let _e237 = ubo.model;
        let _e238 = inTangent_1;
        WorldTangent = normalize((_e237 * _e238).xyz);
        let _e243 = ubo.model;
        let _e244 = BioTangent;
        WorldBioTangent = normalize((_e243 * vec4<f32>(_e244.x, _e244.y, _e244.z, 0.0)).xyz);
    }
    let _e253 = ubo.view;
    let _e254 = WorldNormal;
    VNormal = (_e253 * vec4<f32>(_e254.x, _e254.y, _e254.z, 0.0));
    let _e260 = VNormal;
    SphereUV = ((_e260.xy * 0.5) + vec2<f32>(0.5));
    let _e266 = ubo.drawPathIndex;
    if (_e266 == 2) {
        ViewSpaceOutline = false;
        let _e268 = ViewSpaceOutline;
        if _e268 {
            let _e270 = ubo.view;
            let _e271 = WorldPos;
            CameraPos = (_e270 * _e271);
            let _e274 = ubo.view;
            let _e275 = WorldNormal;
            CameraNormal = (_e274 * vec4<f32>(_e275.x, _e275.y, _e275.z, 0.0)).xyz;
            let _e282 = CameraNormal;
            let _e286 = ubo.edgeSize;
            let _e289 = CameraPos;
            let _e291 = (_e289.xy + ((normalize(_e282).xy * _e286) * 0.0010000000474974513));
            CameraPos[0u] = _e291.x;
            CameraPos[1u] = _e291.y;
            let _e297 = ubo.proj;
            let _e298 = CameraPos;
            perVertexStruct.gl_Position = (_e297 * _e298);
        } else {
            let _e301 = WorldNormal;
            let _e304 = ubo.edgeSize;
            let _e307 = WorldPos;
            let _e309 = (_e307.xyz + ((normalize(_e301) * _e304) * 0.0010000000474974513));
            WorldPos[0u] = _e309.x;
            WorldPos[1u] = _e309.y;
            WorldPos[2u] = _e309.z;
            let _e317 = ubo.proj;
            let _e319 = ubo.view;
            let _e321 = WorldPos;
            perVertexStruct.gl_Position = ((_e317 * _e319) * _e321);
        }
    } else {
        let _e325 = ubo.proj;
        let _e327 = ubo.view;
        let _e329 = WorldPos;
        perVertexStruct.gl_Position = ((_e325 * _e327) * _e329);
    }
    let _e332 = WorldNormal;
    f_WorldNormal = _e332;
    let _e333 = inTexcoord_1;
    f_Texcoord = _e333;
    let _e334 = WorldPos;
    f_WorldPos = _e334;
    let _e335 = WorldTangent;
    f_WorldTangent = _e335;
    let _e336 = WorldBioTangent;
    f_WorldBioTangent = _e336;
    let _e338 = ubo.lightVPMat;
    let _e339 = WorldPos;
    f_LightSpacePos = (_e338 * _e339);
    let _e341 = SphereUV;
    f_SphereUV = _e341;
    return;
}

@vertex 
fn main(@location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(0) inPosition: vec3<f32>, @location(6) inMorphVec0_: vec3<f32>, @location(7) inMorphVec1_: vec3<f32>, @location(8) inMorphVec2_: vec3<f32>, @location(9) inMorphVec3_: vec3<f32>, @location(10) inMorphVec4_: vec3<f32>, @location(11) inMorphVec5_: vec3<f32>, @location(12) inMorphVec6_: vec3<f32>, @location(13) inMorphVec7_: vec3<f32>, @location(5) inWeights0_: vec4<f32>, @location(4) inBone0_: vec4<u32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
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
    inBone0_1 = inBone0_;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e38 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e38);
    let _e40 = perVertexStruct.gl_Position;
    let _e41 = f_WorldNormal;
    let _e42 = f_Texcoord;
    let _e43 = f_WorldPos;
    let _e44 = f_WorldTangent;
    let _e45 = f_WorldBioTangent;
    let _e46 = f_LightSpacePos;
    let _e47 = f_SphereUV;
    return VertexOutput(_e40, _e41, _e42, _e43, _e44, _e45, _e46, _e47);
}
