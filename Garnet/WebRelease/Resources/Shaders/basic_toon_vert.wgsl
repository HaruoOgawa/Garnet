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

    let _e49 = inNormal_1;
    let _e50 = inTangent_1;
    BioTangent = cross(_e49, _e50.xyz);
    let _e54 = ubo.useSkinMeshAnimation;
    if (_e54 != 0) {
        StartSkinMatIndex = 0;
        let _e57 = inWeights0_1[0u];
        let _e59 = inJoint0_1[0u];
        let _e61 = ubo.JointIndexOffset;
        let _e66 = r_SkinMatrixBuffer.SkinMat[(_e59 + bitcast<u32>(_e61))];
        let _e67 = (_e66 * _e57);
        let _e69 = inWeights0_1[1u];
        let _e71 = inJoint0_1[1u];
        let _e73 = ubo.JointIndexOffset;
        let _e78 = r_SkinMatrixBuffer.SkinMat[(_e71 + bitcast<u32>(_e73))];
        let _e79 = (_e78 * _e69);
        let _e92 = mat4x4<f32>((_e67[0] + _e79[0]), (_e67[1] + _e79[1]), (_e67[2] + _e79[2]), (_e67[3] + _e79[3]));
        let _e94 = inWeights0_1[2u];
        let _e96 = inJoint0_1[2u];
        let _e98 = ubo.JointIndexOffset;
        let _e103 = r_SkinMatrixBuffer.SkinMat[(_e96 + bitcast<u32>(_e98))];
        let _e104 = (_e103 * _e94);
        let _e117 = mat4x4<f32>((_e92[0] + _e104[0]), (_e92[1] + _e104[1]), (_e92[2] + _e104[2]), (_e92[3] + _e104[3]));
        let _e119 = inWeights0_1[3u];
        let _e121 = inJoint0_1[3u];
        let _e123 = ubo.JointIndexOffset;
        let _e128 = r_SkinMatrixBuffer.SkinMat[(_e121 + bitcast<u32>(_e123))];
        let _e129 = (_e128 * _e119);
        SkinMat = mat4x4<f32>((_e117[0] + _e129[0]), (_e117[1] + _e129[1]), (_e117[2] + _e129[2]), (_e117[3] + _e129[3]));
        let _e143 = SkinMat;
        let _e144 = inPosition_1;
        WorldPos = (_e143 * vec4<f32>(_e144.x, _e144.y, _e144.z, 1.0));
        let _e150 = SkinMat;
        let _e151 = inNormal_1;
        WorldNormal = normalize((_e150 * vec4<f32>(_e151.x, _e151.y, _e151.z, 0.0)).xyz);
        let _e159 = SkinMat;
        let _e160 = inTangent_1;
        WorldTangent = normalize((_e159 * _e160).xyz);
        let _e164 = SkinMat;
        let _e165 = BioTangent;
        WorldBioTangent = normalize((_e164 * vec4<f32>(_e165.x, _e165.y, _e165.z, 0.0)).xyz);
    } else {
        let _e174 = ubo.model;
        let _e175 = inPosition_1;
        WorldPos = (_e174 * vec4<f32>(_e175.x, _e175.y, _e175.z, 1.0));
        let _e182 = ubo.model;
        let _e183 = inNormal_1;
        WorldNormal = normalize((_e182 * vec4<f32>(_e183.x, _e183.y, _e183.z, 0.0)).xyz);
        let _e192 = ubo.model;
        let _e193 = inTangent_1;
        WorldTangent = normalize((_e192 * _e193).xyz);
        let _e198 = ubo.model;
        let _e199 = BioTangent;
        WorldBioTangent = normalize((_e198 * vec4<f32>(_e199.x, _e199.y, _e199.z, 0.0)).xyz);
    }
    let _e208 = ubo.view;
    let _e209 = WorldNormal;
    VNormal = (_e208 * vec4<f32>(_e209.x, _e209.y, _e209.z, 0.0));
    let _e215 = VNormal;
    SphereUV = ((_e215.xy * 0.5) + vec2<f32>(0.5));
    let _e221 = ubo.drawPathIndex;
    if (_e221 == 2) {
        ViewSpaceOutline = false;
        let _e223 = ViewSpaceOutline;
        if _e223 {
            let _e225 = ubo.view;
            let _e226 = WorldPos;
            CameraPos = (_e225 * _e226);
            let _e229 = ubo.view;
            let _e230 = WorldNormal;
            CameraNormal = (_e229 * vec4<f32>(_e230.x, _e230.y, _e230.z, 0.0)).xyz;
            let _e237 = CameraNormal;
            let _e241 = ubo.edgeSize;
            let _e244 = CameraPos;
            let _e246 = (_e244.xy + ((normalize(_e237).xy * _e241) * 0.0010000000474974513));
            CameraPos[0u] = _e246.x;
            CameraPos[1u] = _e246.y;
            let _e252 = ubo.proj;
            let _e253 = CameraPos;
            perVertexStruct.gl_Position = (_e252 * _e253);
        } else {
            let _e256 = WorldNormal;
            let _e259 = ubo.edgeSize;
            let _e262 = WorldPos;
            let _e264 = (_e262.xyz + ((normalize(_e256) * _e259) * 0.0010000000474974513));
            WorldPos[0u] = _e264.x;
            WorldPos[1u] = _e264.y;
            WorldPos[2u] = _e264.z;
            let _e272 = ubo.proj;
            let _e274 = ubo.view;
            let _e276 = WorldPos;
            perVertexStruct.gl_Position = ((_e272 * _e274) * _e276);
        }
    } else {
        let _e280 = ubo.proj;
        let _e282 = ubo.view;
        let _e284 = WorldPos;
        perVertexStruct.gl_Position = ((_e280 * _e282) * _e284);
    }
    let _e287 = WorldNormal;
    f_WorldNormal = _e287;
    let _e288 = inTexcoord_1;
    f_Texcoord = _e288;
    let _e289 = WorldPos;
    f_WorldPos = _e289;
    let _e290 = WorldTangent;
    f_WorldTangent = _e290;
    let _e291 = WorldBioTangent;
    f_WorldBioTangent = _e291;
    let _e293 = ubo.lightVPMat;
    let _e294 = WorldPos;
    f_LightSpacePos = (_e293 * _e294);
    let _e296 = SphereUV;
    f_SphereUV = _e296;
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
