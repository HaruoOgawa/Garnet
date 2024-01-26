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
    var VNormal: vec4<f32>;
    var SphereUV: vec2<f32>;

    let _e42 = inNormal_1;
    let _e43 = inTangent_1;
    BioTangent = cross(_e42, _e43.xyz);
    let _e47 = ubo.useSkinMeshAnimation;
    if (_e47 != 0) {
        StartSkinMatIndex = 0;
        let _e50 = inWeights0_1[0u];
        let _e52 = inJoint0_1[0u];
        let _e54 = ubo.JointIndexOffset;
        let _e59 = r_SkinMatrixBuffer.SkinMat[(_e52 + bitcast<u32>(_e54))];
        let _e60 = (_e59 * _e50);
        let _e62 = inWeights0_1[1u];
        let _e64 = inJoint0_1[1u];
        let _e66 = ubo.JointIndexOffset;
        let _e71 = r_SkinMatrixBuffer.SkinMat[(_e64 + bitcast<u32>(_e66))];
        let _e72 = (_e71 * _e62);
        let _e85 = mat4x4<f32>((_e60[0] + _e72[0]), (_e60[1] + _e72[1]), (_e60[2] + _e72[2]), (_e60[3] + _e72[3]));
        let _e87 = inWeights0_1[2u];
        let _e89 = inJoint0_1[2u];
        let _e91 = ubo.JointIndexOffset;
        let _e96 = r_SkinMatrixBuffer.SkinMat[(_e89 + bitcast<u32>(_e91))];
        let _e97 = (_e96 * _e87);
        let _e110 = mat4x4<f32>((_e85[0] + _e97[0]), (_e85[1] + _e97[1]), (_e85[2] + _e97[2]), (_e85[3] + _e97[3]));
        let _e112 = inWeights0_1[3u];
        let _e114 = inJoint0_1[3u];
        let _e116 = ubo.JointIndexOffset;
        let _e121 = r_SkinMatrixBuffer.SkinMat[(_e114 + bitcast<u32>(_e116))];
        let _e122 = (_e121 * _e112);
        SkinMat = mat4x4<f32>((_e110[0] + _e122[0]), (_e110[1] + _e122[1]), (_e110[2] + _e122[2]), (_e110[3] + _e122[3]));
        let _e136 = SkinMat;
        let _e137 = inPosition_1;
        WorldPos = (_e136 * vec4<f32>(_e137.x, _e137.y, _e137.z, 1.0));
        let _e143 = SkinMat;
        let _e144 = inNormal_1;
        WorldNormal = normalize((_e143 * vec4<f32>(_e144.x, _e144.y, _e144.z, 0.0)).xyz);
        let _e152 = SkinMat;
        let _e153 = inTangent_1;
        WorldTangent = normalize((_e152 * _e153).xyz);
        let _e157 = SkinMat;
        let _e158 = BioTangent;
        WorldBioTangent = normalize((_e157 * vec4<f32>(_e158.x, _e158.y, _e158.z, 0.0)).xyz);
    } else {
        let _e167 = ubo.model;
        let _e168 = inPosition_1;
        WorldPos = (_e167 * vec4<f32>(_e168.x, _e168.y, _e168.z, 1.0));
        let _e175 = ubo.model;
        let _e176 = inNormal_1;
        WorldNormal = normalize((_e175 * vec4<f32>(_e176.x, _e176.y, _e176.z, 0.0)).xyz);
        let _e185 = ubo.model;
        let _e186 = inTangent_1;
        WorldTangent = normalize((_e185 * _e186).xyz);
        let _e191 = ubo.model;
        let _e192 = BioTangent;
        WorldBioTangent = normalize((_e191 * vec4<f32>(_e192.x, _e192.y, _e192.z, 0.0)).xyz);
    }
    let _e201 = ubo.view;
    let _e202 = WorldNormal;
    VNormal = (_e201 * vec4<f32>(_e202.x, _e202.y, _e202.z, 0.0));
    let _e208 = VNormal;
    SphereUV = ((_e208.xy * 0.5) + vec2<f32>(0.5));
    let _e214 = ubo.proj;
    let _e216 = ubo.view;
    let _e218 = WorldPos;
    perVertexStruct.gl_Position = ((_e214 * _e216) * _e218);
    let _e221 = WorldNormal;
    f_WorldNormal = _e221;
    let _e222 = inTexcoord_1;
    f_Texcoord = _e222;
    let _e223 = WorldPos;
    f_WorldPos = _e223;
    let _e224 = WorldTangent;
    f_WorldTangent = _e224;
    let _e225 = WorldBioTangent;
    f_WorldBioTangent = _e225;
    let _e227 = ubo.lightVPMat;
    let _e228 = WorldPos;
    f_LightSpacePos = (_e227 * _e228);
    let _e230 = SphereUV;
    f_SphereUV = _e230;
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
