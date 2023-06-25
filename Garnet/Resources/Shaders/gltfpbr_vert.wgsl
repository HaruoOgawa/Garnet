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

fn main_1() {
    var pos: vec4<f32>;

    let _e23 = inPosition_1;
    pos = vec4<f32>(_e23.x, _e23.y, _e23.z, 1.0);
    let _e29 = ubo.proj;
    let _e31 = ubo.view;
    let _e34 = ubo.model;
    let _e36 = pos;
    perVertexStruct.gl_Position = (((_e29 * _e31) * _e34) * _e36);
    let _e40 = ubo.model;
    let _e41 = inNormal_1;
    f_WorldNormal = normalize((_e40 * vec4<f32>(_e41.x, _e41.y, _e41.z, 0.0)).xyz);
    let _e49 = inTexcoord_1;
    f_Texcoord = _e49;
    let _e51 = ubo.model;
    let _e52 = inPosition_1;
    f_WorldPos = (_e51 * vec4<f32>(_e52.x, _e52.y, _e52.z, 1.0));
    let _e59 = ubo.model;
    let _e60 = inTangent_1;
    f_WorldTangent = normalize((_e59 * _e60).xyz);
    let _e65 = ubo.model;
    let _e66 = inBioTangent_1;
    f_WorldBioTangent = normalize((_e65 * _e66).xyz);
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
    let _e18 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e18);
    let _e20 = perVertexStruct.gl_Position;
    let _e21 = f_WorldNormal;
    let _e22 = f_Texcoord;
    let _e23 = f_WorldPos;
    let _e24 = f_WorldTangent;
    let _e25 = f_WorldBioTangent;
    return VertexOutput(_e20, _e21, _e22, _e23, _e24, _e25);
}
