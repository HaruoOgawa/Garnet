struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightVPMat: mat4x4<f32>,
    useSkinMeshAnimation: i32,
    JointIndexOffset: i32,
    pad0_: i32,
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
var<private> f_SphereUV: vec2<f32>;

fn main_1() {
    var BioTangent: vec3<f32>;
    var StartSkinMatIndex: i32;
    var SkinMat: mat4x4<f32>;
    var WorldPos: vec4<f32>;
    var WorldNormal: vec3<f32>;
    var WorldTangent: vec3<f32>;
    var WorldBioTangent: vec3<f32>;
    var SphereUV: vec2<f32>;
    var pi: f32;
    var theta: f32;
    var phi: f32;

    let _e46 = inNormal_1;
    let _e47 = inTangent_1;
    BioTangent = cross(_e46, _e47.xyz);
    let _e51 = ubo.useSkinMeshAnimation;
    if (_e51 != 0) {
        StartSkinMatIndex = 0;
        let _e54 = inWeights0_1[0u];
        let _e56 = inJoint0_1[0u];
        let _e58 = ubo.JointIndexOffset;
        let _e63 = r_SkinMatrixBuffer.SkinMat[(_e56 + bitcast<u32>(_e58))];
        let _e64 = (_e63 * _e54);
        let _e66 = inWeights0_1[1u];
        let _e68 = inJoint0_1[1u];
        let _e70 = ubo.JointIndexOffset;
        let _e75 = r_SkinMatrixBuffer.SkinMat[(_e68 + bitcast<u32>(_e70))];
        let _e76 = (_e75 * _e66);
        let _e89 = mat4x4<f32>((_e64[0] + _e76[0]), (_e64[1] + _e76[1]), (_e64[2] + _e76[2]), (_e64[3] + _e76[3]));
        let _e91 = inWeights0_1[2u];
        let _e93 = inJoint0_1[2u];
        let _e95 = ubo.JointIndexOffset;
        let _e100 = r_SkinMatrixBuffer.SkinMat[(_e93 + bitcast<u32>(_e95))];
        let _e101 = (_e100 * _e91);
        let _e114 = mat4x4<f32>((_e89[0] + _e101[0]), (_e89[1] + _e101[1]), (_e89[2] + _e101[2]), (_e89[3] + _e101[3]));
        let _e116 = inWeights0_1[3u];
        let _e118 = inJoint0_1[3u];
        let _e120 = ubo.JointIndexOffset;
        let _e125 = r_SkinMatrixBuffer.SkinMat[(_e118 + bitcast<u32>(_e120))];
        let _e126 = (_e125 * _e116);
        SkinMat = mat4x4<f32>((_e114[0] + _e126[0]), (_e114[1] + _e126[1]), (_e114[2] + _e126[2]), (_e114[3] + _e126[3]));
        let _e140 = SkinMat;
        let _e141 = inPosition_1;
        WorldPos = (_e140 * vec4<f32>(_e141.x, _e141.y, _e141.z, 1.0));
        let _e147 = SkinMat;
        let _e148 = inNormal_1;
        WorldNormal = normalize((_e147 * vec4<f32>(_e148.x, _e148.y, _e148.z, 0.0)).xyz);
        let _e156 = SkinMat;
        let _e157 = inTangent_1;
        WorldTangent = normalize((_e156 * _e157).xyz);
        let _e161 = SkinMat;
        let _e162 = BioTangent;
        WorldBioTangent = normalize((_e161 * vec4<f32>(_e162.x, _e162.y, _e162.z, 0.0)).xyz);
    } else {
        let _e171 = ubo.model;
        let _e172 = inPosition_1;
        WorldPos = (_e171 * vec4<f32>(_e172.x, _e172.y, _e172.z, 1.0));
        let _e179 = ubo.model;
        let _e180 = inNormal_1;
        WorldNormal = normalize((_e179 * vec4<f32>(_e180.x, _e180.y, _e180.z, 0.0)).xyz);
        let _e189 = ubo.model;
        let _e190 = inTangent_1;
        WorldTangent = normalize((_e189 * _e190).xyz);
        let _e195 = ubo.model;
        let _e196 = BioTangent;
        WorldBioTangent = normalize((_e195 * vec4<f32>(_e196.x, _e196.y, _e196.z, 0.0)).xyz);
    }
    SphereUV = vec2<f32>(0.0, 0.0);
    pi = 3.1415927410125732;
    let _e205 = inNormal_1[1u];
    theta = acos(_e205);
    let _e208 = inNormal_1[2u];
    let _e210 = inNormal_1[0u];
    phi = atan2(_e208, _e210);
    let _e212 = phi;
    let _e213 = pi;
    let _e216 = theta;
    let _e217 = pi;
    SphereUV = vec2<f32>((_e212 / (2.0 * _e213)), (_e216 / _e217));
    let _e221 = ubo.proj;
    let _e223 = ubo.view;
    let _e225 = WorldPos;
    perVertexStruct.gl_Position = ((_e221 * _e223) * _e225);
    let _e228 = WorldNormal;
    f_WorldNormal = _e228;
    let _e229 = inTexcoord_1;
    f_Texcoord = _e229;
    let _e230 = WorldPos;
    f_WorldPos = _e230;
    let _e231 = WorldTangent;
    f_WorldTangent = _e231;
    let _e232 = WorldBioTangent;
    f_WorldBioTangent = _e232;
    let _e234 = ubo.lightVPMat;
    let _e235 = WorldPos;
    f_LightSpacePos = (_e234 * _e235);
    let _e237 = SphereUV;
    f_SphereUV = _e237;
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
    let _e22 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e22);
    let _e24 = perVertexStruct.gl_Position;
    let _e25 = f_WorldNormal;
    let _e26 = f_Texcoord;
    let _e27 = f_WorldPos;
    let _e28 = f_WorldTangent;
    let _e29 = f_WorldBioTangent;
    let _e30 = f_LightSpacePos;
    let _e31 = f_SphereUV;
    return VertexOutput(_e24, _e25, _e26, _e27, _e28, _e29, _e30, _e31);
}
