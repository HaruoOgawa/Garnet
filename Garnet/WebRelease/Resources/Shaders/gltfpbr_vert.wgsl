struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32,1u>,
    gl_CullDistance: array<f32,1u>,
}

struct UniformBufferObject {
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
    s_pad1_: f32,
    s_pad2_: f32,
    useBaseColorTexture: i32,
    useMetallicRoughnessTexture: i32,
    useEmissiveTexture: i32,
    useNormalTexture: i32,
    useOcclusionTexture: i32,
    t_pad_0_: i32,
    t_pad_1_: i32,
    t_pad_2_: i32,
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

var<private> inPosition_1: vec3<f32>;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> f_WorldNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;
var<private> f_WorldTangent: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
var<private> f_WorldBioTangent: vec3<f32>;
var<private> inBioTangent_1: vec4<f32>;
var<private> f_LightSpacePos: vec4<f32>;

fn main_1() {
    var pos: vec4<f32>;

    let _e25 = inPosition_1;
    pos = vec4<f32>(_e25.x, _e25.y, _e25.z, 1.0);
    let _e31 = ubo.proj;
    let _e33 = ubo.view;
    let _e36 = ubo.model;
    let _e38 = pos;
    perVertexStruct.gl_Position = (((_e31 * _e33) * _e36) * _e38);
    let _e42 = ubo.model;
    let _e43 = inNormal_1;
    f_WorldNormal = normalize((_e42 * vec4<f32>(_e43.x, _e43.y, _e43.z, 0.0)).xyz);
    let _e51 = inTexcoord_1;
    f_Texcoord = _e51;
    let _e53 = ubo.model;
    let _e54 = inPosition_1;
    f_WorldPos = (_e53 * vec4<f32>(_e54.x, _e54.y, _e54.z, 1.0));
    let _e61 = ubo.model;
    let _e62 = inTangent_1;
    f_WorldTangent = normalize((_e61 * _e62).xyz);
    let _e67 = ubo.model;
    let _e68 = inBioTangent_1;
    f_WorldBioTangent = normalize((_e67 * _e68).xyz);
    let _e73 = ubo.lightVPMat;
    let _e75 = ubo.model;
    let _e77 = pos;
    f_LightSpacePos = ((_e73 * _e75) * _e77);
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(3) inTangent: vec4<f32>, @location(4) inBioTangent: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    inTangent_1 = inTangent;
    inBioTangent_1 = inBioTangent;
    main_1();
    let _e19 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e19);
    let _e21 = perVertexStruct.gl_Position;
    let _e22 = f_WorldNormal;
    let _e23 = f_Texcoord;
    let _e24 = f_WorldPos;
    let _e25 = f_WorldTangent;
    let _e26 = f_WorldBioTangent;
    let _e27 = f_LightSpacePos;
    return VertexOutput(_e21, _e22, _e23, _e24, _e25, _e26, _e27);
}
