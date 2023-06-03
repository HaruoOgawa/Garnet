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
    cameraPos: vec4<f32>,
    baseColorFactor: vec4<f32>,
    emissiveFactor: vec4<f32>,
    time: f32,
    metallicFactor: f32,
    roughnessFactor: f32,
    padding2_: f32,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
    @location(1) member_1: vec2<f32>,
    @location(2) member_2: vec4<f32>,
}

var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> f_WorldNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;

fn main_1() {
    let _e19 = ubo.proj;
    let _e21 = ubo.view;
    let _e24 = ubo.model;
    let _e26 = inPosition_1;
    perVertexStruct.gl_Position = (((_e19 * _e21) * _e24) * vec4<f32>(_e26.x, _e26.y, _e26.z, 1.0));
    let _e34 = ubo.model;
    let _e35 = inNormal_1;
    f_WorldNormal = (_e34 * vec4<f32>(_e35.x, _e35.y, _e35.z, 0.0)).xyz;
    let _e42 = inTexcoord_1;
    f_Texcoord = _e42;
    let _e44 = ubo.model;
    let _e45 = inPosition_1;
    f_WorldPos = (_e44 * vec4<f32>(_e45.x, _e45.y, _e45.z, 1.0));
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e12 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e12);
    let _e14 = perVertexStruct.gl_Position;
    let _e15 = f_WorldNormal;
    let _e16 = f_Texcoord;
    let _e17 = f_WorldPos;
    return VertexOutput(_e14, _e15, _e16, _e17);
}
