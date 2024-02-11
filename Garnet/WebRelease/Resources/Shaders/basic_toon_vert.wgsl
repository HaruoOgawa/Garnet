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
    pad0_: i32,
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
var<private> inBone0_1: vec4<u32>;
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

    let _e47 = inNormal_1;
    let _e48 = inTangent_1;
    BioTangent = cross(_e47, _e48.xyz);
    let _e52 = ubo.useSkinMeshAnimation;
    if (_e52 != 0) {
        let _e55 = inWeights0_1[0u];
        let _e57 = inBone0_1[0u];
        let _e60 = r_SkinMatrixBuffer.SkinMat[_e57];
        let _e61 = (_e60 * _e55);
        let _e63 = inWeights0_1[1u];
        let _e65 = inBone0_1[1u];
        let _e68 = r_SkinMatrixBuffer.SkinMat[_e65];
        let _e69 = (_e68 * _e63);
        let _e82 = mat4x4<f32>((_e61[0] + _e69[0]), (_e61[1] + _e69[1]), (_e61[2] + _e69[2]), (_e61[3] + _e69[3]));
        let _e84 = inWeights0_1[2u];
        let _e86 = inBone0_1[2u];
        let _e89 = r_SkinMatrixBuffer.SkinMat[_e86];
        let _e90 = (_e89 * _e84);
        let _e103 = mat4x4<f32>((_e82[0] + _e90[0]), (_e82[1] + _e90[1]), (_e82[2] + _e90[2]), (_e82[3] + _e90[3]));
        let _e105 = inWeights0_1[3u];
        let _e107 = inBone0_1[3u];
        let _e110 = r_SkinMatrixBuffer.SkinMat[_e107];
        let _e111 = (_e110 * _e105);
        SkinMat = mat4x4<f32>((_e103[0] + _e111[0]), (_e103[1] + _e111[1]), (_e103[2] + _e111[2]), (_e103[3] + _e111[3]));
        let _e125 = SkinMat;
        let _e126 = inPosition_1;
        WorldPos = (_e125 * vec4<f32>(_e126.x, _e126.y, _e126.z, 1.0));
        let _e132 = SkinMat;
        let _e133 = inNormal_1;
        WorldNormal = normalize((_e132 * vec4<f32>(_e133.x, _e133.y, _e133.z, 0.0)).xyz);
        let _e141 = SkinMat;
        let _e142 = inTangent_1;
        WorldTangent = normalize((_e141 * _e142).xyz);
        let _e146 = SkinMat;
        let _e147 = BioTangent;
        WorldBioTangent = normalize((_e146 * vec4<f32>(_e147.x, _e147.y, _e147.z, 0.0)).xyz);
    } else {
        let _e156 = ubo.model;
        let _e157 = inPosition_1;
        WorldPos = (_e156 * vec4<f32>(_e157.x, _e157.y, _e157.z, 1.0));
        let _e164 = ubo.model;
        let _e165 = inNormal_1;
        WorldNormal = normalize((_e164 * vec4<f32>(_e165.x, _e165.y, _e165.z, 0.0)).xyz);
        let _e174 = ubo.model;
        let _e175 = inTangent_1;
        WorldTangent = normalize((_e174 * _e175).xyz);
        let _e180 = ubo.model;
        let _e181 = BioTangent;
        WorldBioTangent = normalize((_e180 * vec4<f32>(_e181.x, _e181.y, _e181.z, 0.0)).xyz);
    }
    let _e190 = ubo.view;
    let _e191 = WorldNormal;
    VNormal = (_e190 * vec4<f32>(_e191.x, _e191.y, _e191.z, 0.0));
    let _e197 = VNormal;
    SphereUV = ((_e197.xy * 0.5) + vec2<f32>(0.5));
    let _e203 = ubo.drawPathIndex;
    if (_e203 == 2) {
        ViewSpaceOutline = false;
        let _e205 = ViewSpaceOutline;
        if _e205 {
            let _e207 = ubo.view;
            let _e208 = WorldPos;
            CameraPos = (_e207 * _e208);
            let _e211 = ubo.view;
            let _e212 = WorldNormal;
            CameraNormal = (_e211 * vec4<f32>(_e212.x, _e212.y, _e212.z, 0.0)).xyz;
            let _e219 = CameraNormal;
            let _e223 = ubo.edgeSize;
            let _e226 = CameraPos;
            let _e228 = (_e226.xy + ((normalize(_e219).xy * _e223) * 0.0010000000474974513));
            CameraPos[0u] = _e228.x;
            CameraPos[1u] = _e228.y;
            let _e234 = ubo.proj;
            let _e235 = CameraPos;
            perVertexStruct.gl_Position = (_e234 * _e235);
        } else {
            let _e238 = WorldNormal;
            let _e241 = ubo.edgeSize;
            let _e244 = WorldPos;
            let _e246 = (_e244.xyz + ((normalize(_e238) * _e241) * 0.0010000000474974513));
            WorldPos[0u] = _e246.x;
            WorldPos[1u] = _e246.y;
            WorldPos[2u] = _e246.z;
            let _e254 = ubo.proj;
            let _e256 = ubo.view;
            let _e258 = WorldPos;
            perVertexStruct.gl_Position = ((_e254 * _e256) * _e258);
        }
    } else {
        let _e262 = ubo.proj;
        let _e264 = ubo.view;
        let _e266 = WorldPos;
        perVertexStruct.gl_Position = ((_e262 * _e264) * _e266);
    }
    let _e269 = WorldNormal;
    f_WorldNormal = _e269;
    let _e270 = inTexcoord_1;
    f_Texcoord = _e270;
    let _e271 = WorldPos;
    f_WorldPos = _e271;
    let _e272 = WorldTangent;
    f_WorldTangent = _e272;
    let _e273 = WorldBioTangent;
    f_WorldBioTangent = _e273;
    let _e275 = ubo.lightVPMat;
    let _e276 = WorldPos;
    f_LightSpacePos = (_e275 * _e276);
    let _e278 = SphereUV;
    f_SphereUV = _e278;
    return;
}

@vertex 
fn main(@location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(5) inWeights0_: vec4<f32>, @location(4) inBone0_: vec4<u32>, @location(0) inPosition: vec3<f32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inWeights0_1 = inWeights0_;
    inBone0_1 = inBone0_;
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
