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
    var ViewSpaceOutline: bool;
    var CameraPos: vec4<f32>;
    var CameraNormal: vec3<f32>;

    let _e50 = inNormal_1;
    let _e51 = inTangent_1;
    BioTangent = cross(_e50, _e51.xyz);
    let _e55 = ubo.useSkinMeshAnimation;
    if (_e55 != 0) {
        StartSkinMatIndex = 0;
        let _e58 = inWeights0_1[0u];
        let _e60 = inJoint0_1[0u];
        let _e62 = ubo.JointIndexOffset;
        let _e67 = r_SkinMatrixBuffer.SkinMat[(_e60 + bitcast<u32>(_e62))];
        let _e68 = (_e67 * _e58);
        let _e70 = inWeights0_1[1u];
        let _e72 = inJoint0_1[1u];
        let _e74 = ubo.JointIndexOffset;
        let _e79 = r_SkinMatrixBuffer.SkinMat[(_e72 + bitcast<u32>(_e74))];
        let _e80 = (_e79 * _e70);
        let _e93 = mat4x4<f32>((_e68[0] + _e80[0]), (_e68[1] + _e80[1]), (_e68[2] + _e80[2]), (_e68[3] + _e80[3]));
        let _e95 = inWeights0_1[2u];
        let _e97 = inJoint0_1[2u];
        let _e99 = ubo.JointIndexOffset;
        let _e104 = r_SkinMatrixBuffer.SkinMat[(_e97 + bitcast<u32>(_e99))];
        let _e105 = (_e104 * _e95);
        let _e118 = mat4x4<f32>((_e93[0] + _e105[0]), (_e93[1] + _e105[1]), (_e93[2] + _e105[2]), (_e93[3] + _e105[3]));
        let _e120 = inWeights0_1[3u];
        let _e122 = inJoint0_1[3u];
        let _e124 = ubo.JointIndexOffset;
        let _e129 = r_SkinMatrixBuffer.SkinMat[(_e122 + bitcast<u32>(_e124))];
        let _e130 = (_e129 * _e120);
        SkinMat = mat4x4<f32>((_e118[0] + _e130[0]), (_e118[1] + _e130[1]), (_e118[2] + _e130[2]), (_e118[3] + _e130[3]));
        let _e144 = SkinMat;
        let _e145 = inPosition_1;
        WorldPos = (_e144 * vec4<f32>(_e145.x, _e145.y, _e145.z, 1.0));
        let _e151 = SkinMat;
        let _e152 = inNormal_1;
        WorldNormal = normalize((_e151 * vec4<f32>(_e152.x, _e152.y, _e152.z, 0.0)).xyz);
        let _e160 = SkinMat;
        let _e161 = inTangent_1;
        WorldTangent = normalize((_e160 * _e161).xyz);
        let _e165 = SkinMat;
        let _e166 = BioTangent;
        WorldBioTangent = normalize((_e165 * vec4<f32>(_e166.x, _e166.y, _e166.z, 0.0)).xyz);
    } else {
        let _e175 = ubo.model;
        let _e176 = inPosition_1;
        WorldPos = (_e175 * vec4<f32>(_e176.x, _e176.y, _e176.z, 1.0));
        let _e183 = ubo.model;
        let _e184 = inNormal_1;
        WorldNormal = normalize((_e183 * vec4<f32>(_e184.x, _e184.y, _e184.z, 0.0)).xyz);
        let _e193 = ubo.model;
        let _e194 = inTangent_1;
        WorldTangent = normalize((_e193 * _e194).xyz);
        let _e199 = ubo.model;
        let _e200 = BioTangent;
        WorldBioTangent = normalize((_e199 * vec4<f32>(_e200.x, _e200.y, _e200.z, 0.0)).xyz);
    }
    let _e209 = ubo.view;
    let _e210 = WorldNormal;
    VNormal = (_e209 * vec4<f32>(_e210.x, _e210.y, _e210.z, 0.0));
    let _e216 = VNormal;
    SphereUV = ((_e216.xy * 0.5) + vec2<f32>(0.5));
    let _e222 = ubo.drawPathIndex;
    if (_e222 == 2) {
        ViewSpaceOutline = false;
        let _e224 = ViewSpaceOutline;
        if _e224 {
            let _e226 = ubo.view;
            let _e227 = WorldPos;
            CameraPos = (_e226 * _e227);
            let _e230 = ubo.view;
            let _e231 = WorldNormal;
            CameraNormal = (_e230 * vec4<f32>(_e231.x, _e231.y, _e231.z, 0.0)).xyz;
            let _e238 = CameraNormal;
            let _e242 = ubo.edgeSize;
            let _e245 = CameraPos;
            let _e247 = (_e245.xy + ((normalize(_e238).xy * _e242) * 0.009999999776482582));
            CameraPos[0u] = _e247.x;
            CameraPos[1u] = _e247.y;
            let _e253 = ubo.proj;
            let _e254 = CameraPos;
            perVertexStruct.gl_Position = (_e253 * _e254);
        } else {
            let _e257 = WorldNormal;
            let _e260 = ubo.edgeSize;
            let _e263 = WorldPos;
            let _e265 = (_e263.xyz + ((normalize(_e257) * _e260) * 0.004999999888241291));
            WorldPos[0u] = _e265.x;
            WorldPos[1u] = _e265.y;
            WorldPos[2u] = _e265.z;
            let _e273 = ubo.proj;
            let _e275 = ubo.view;
            let _e277 = WorldPos;
            perVertexStruct.gl_Position = ((_e273 * _e275) * _e277);
        }
    } else {
        let _e281 = ubo.proj;
        let _e283 = ubo.view;
        let _e285 = WorldPos;
        perVertexStruct.gl_Position = ((_e281 * _e283) * _e285);
    }
    let _e288 = WorldNormal;
    f_WorldNormal = _e288;
    let _e289 = inTexcoord_1;
    f_Texcoord = _e289;
    let _e290 = WorldPos;
    f_WorldPos = _e290;
    let _e291 = WorldTangent;
    f_WorldTangent = _e291;
    let _e292 = WorldBioTangent;
    f_WorldBioTangent = _e292;
    let _e294 = ubo.lightVPMat;
    let _e295 = WorldPos;
    f_LightSpacePos = (_e294 * _e295);
    let _e297 = SphereUV;
    f_SphereUV = _e297;
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
