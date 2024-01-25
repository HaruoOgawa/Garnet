struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightVPMat: mat4x4<f32>,
    edgeSize: f32,
    fPad0_: f32,
    fPad1_: f32,
    fPad2_: f32,
    useSkinMeshAnimation: i32,
    JointIndexOffset: i32,
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
    var VNormal: vec4<f32>;
    var SphereUV: vec2<f32>;

    let _e45 = inNormal_1;
    let _e46 = inTangent_1;
    BioTangent = cross(_e45, _e46.xyz);
    let _e50 = ubo.useSkinMeshAnimation;
    if (_e50 != 0) {
        StartSkinMatIndex = 0;
        let _e53 = inWeights0_1[0u];
        let _e55 = inJoint0_1[0u];
        let _e57 = ubo.JointIndexOffset;
        let _e62 = r_SkinMatrixBuffer.SkinMat[(_e55 + bitcast<u32>(_e57))];
        let _e63 = (_e62 * _e53);
        let _e65 = inWeights0_1[1u];
        let _e67 = inJoint0_1[1u];
        let _e69 = ubo.JointIndexOffset;
        let _e74 = r_SkinMatrixBuffer.SkinMat[(_e67 + bitcast<u32>(_e69))];
        let _e75 = (_e74 * _e65);
        let _e88 = mat4x4<f32>((_e63[0] + _e75[0]), (_e63[1] + _e75[1]), (_e63[2] + _e75[2]), (_e63[3] + _e75[3]));
        let _e90 = inWeights0_1[2u];
        let _e92 = inJoint0_1[2u];
        let _e94 = ubo.JointIndexOffset;
        let _e99 = r_SkinMatrixBuffer.SkinMat[(_e92 + bitcast<u32>(_e94))];
        let _e100 = (_e99 * _e90);
        let _e113 = mat4x4<f32>((_e88[0] + _e100[0]), (_e88[1] + _e100[1]), (_e88[2] + _e100[2]), (_e88[3] + _e100[3]));
        let _e115 = inWeights0_1[3u];
        let _e117 = inJoint0_1[3u];
        let _e119 = ubo.JointIndexOffset;
        let _e124 = r_SkinMatrixBuffer.SkinMat[(_e117 + bitcast<u32>(_e119))];
        let _e125 = (_e124 * _e115);
        SkinMat = mat4x4<f32>((_e113[0] + _e125[0]), (_e113[1] + _e125[1]), (_e113[2] + _e125[2]), (_e113[3] + _e125[3]));
        let _e139 = SkinMat;
        let _e140 = inPosition_1;
        WorldPos = (_e139 * vec4<f32>(_e140.x, _e140.y, _e140.z, 1.0));
        let _e146 = SkinMat;
        let _e147 = inNormal_1;
        WorldNormal = normalize((_e146 * vec4<f32>(_e147.x, _e147.y, _e147.z, 0.0)).xyz);
        let _e155 = SkinMat;
        let _e156 = inTangent_1;
        WorldTangent = normalize((_e155 * _e156).xyz);
        let _e160 = SkinMat;
        let _e161 = BioTangent;
        WorldBioTangent = normalize((_e160 * vec4<f32>(_e161.x, _e161.y, _e161.z, 0.0)).xyz);
    } else {
        let _e170 = ubo.model;
        let _e171 = inPosition_1;
        WorldPos = (_e170 * vec4<f32>(_e171.x, _e171.y, _e171.z, 1.0));
        let _e178 = ubo.model;
        let _e179 = inNormal_1;
        WorldNormal = normalize((_e178 * vec4<f32>(_e179.x, _e179.y, _e179.z, 0.0)).xyz);
        let _e188 = ubo.model;
        let _e189 = inTangent_1;
        WorldTangent = normalize((_e188 * _e189).xyz);
        let _e194 = ubo.model;
        let _e195 = BioTangent;
        WorldBioTangent = normalize((_e194 * vec4<f32>(_e195.x, _e195.y, _e195.z, 0.0)).xyz);
    }
    let _e204 = ubo.view;
    let _e205 = WorldNormal;
    VNormal = (_e204 * vec4<f32>(_e205.x, _e205.y, _e205.z, 0.0));
    let _e211 = VNormal;
    SphereUV = ((_e211.xy * 0.5) + vec2<f32>(0.5));
    let _e217 = ubo.drawPathIndex;
    if (_e217 == 2) {
        let _e219 = WorldNormal;
        let _e222 = ubo.edgeSize;
        let _e225 = WorldPos;
        let _e227 = (_e225.xyz + ((normalize(_e219) * _e222) * 0.009999999776482582));
        WorldPos[0u] = _e227.x;
        WorldPos[1u] = _e227.y;
        WorldPos[2u] = _e227.z;
    }
    let _e235 = ubo.proj;
    let _e237 = ubo.view;
    let _e239 = WorldPos;
    perVertexStruct.gl_Position = ((_e235 * _e237) * _e239);
    let _e242 = WorldNormal;
    f_WorldNormal = _e242;
    let _e243 = inTexcoord_1;
    f_Texcoord = _e243;
    let _e244 = WorldPos;
    f_WorldPos = _e244;
    let _e245 = WorldTangent;
    f_WorldTangent = _e245;
    let _e246 = WorldBioTangent;
    f_WorldBioTangent = _e246;
    let _e248 = ubo.lightVPMat;
    let _e249 = WorldPos;
    f_LightSpacePos = (_e248 * _e249);
    let _e251 = SphereUV;
    f_SphereUV = _e251;
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
