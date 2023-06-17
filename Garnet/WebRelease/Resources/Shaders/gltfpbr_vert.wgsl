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
    @location(3) member_3: vec4<f32>,
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
var<private> f_WorldTangent: vec4<f32>;
var<private> inTangent_1: vec4<f32>;

fn main_1() {
    var pos: vec4<f32>;

    let _e21 = inPosition_1;
    pos = vec4<f32>(_e21.x, _e21.y, _e21.z, 1.0);
    let _e27 = ubo.proj;
    let _e29 = ubo.view;
    let _e32 = ubo.model;
    let _e34 = pos;
    perVertexStruct.gl_Position = (((_e27 * _e29) * _e32) * _e34);
    let _e38 = ubo.model;
    let _e39 = inNormal_1;
    f_WorldNormal = (_e38 * vec4<f32>(_e39.x, _e39.y, _e39.z, 0.0)).xyz;
    let _e46 = inTexcoord_1;
    f_Texcoord = _e46;
    let _e48 = ubo.model;
    let _e49 = inPosition_1;
    f_WorldPos = (_e48 * vec4<f32>(_e49.x, _e49.y, _e49.z, 1.0));
    let _e56 = ubo.model;
    let _e57 = inTangent_1;
    f_WorldTangent = (_e56 * _e57);
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(3) inTangent: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    inTangent_1 = inTangent;
    main_1();
    let _e15 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e15);
    let _e17 = perVertexStruct.gl_Position;
    let _e18 = f_WorldNormal;
    let _e19 = f_Texcoord;
    let _e20 = f_WorldPos;
    let _e21 = f_WorldTangent;
    return VertexOutput(_e17, _e18, _e19, _e20, _e21);
}
