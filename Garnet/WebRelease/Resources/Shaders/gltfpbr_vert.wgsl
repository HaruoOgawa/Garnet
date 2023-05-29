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
    time: f32,
    padding0_: f32,
    padding1_: f32,
    padding2_: f32,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
    @location(1) member_1: vec2<f32>,
}

var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> f_WorldNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;

fn main_1() {
    let _e18 = ubo.proj;
    let _e20 = ubo.view;
    let _e23 = ubo.model;
    let _e25 = inPosition_1;
    perVertexStruct.gl_Position = (((_e18 * _e20) * _e23) * vec4<f32>(_e25.x, _e25.y, _e25.z, 1.0));
    let _e33 = ubo.model;
    let _e34 = inNormal_1;
    f_WorldNormal = (_e33 * vec4<f32>(_e34.x, _e34.y, _e34.z, 0.0)).xyz;
    let _e41 = inTexcoord_1;
    f_Texcoord = _e41;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e11 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e11);
    let _e13 = perVertexStruct.gl_Position;
    let _e14 = f_WorldNormal;
    let _e15 = f_Texcoord;
    return VertexOutput(_e13, _e14, _e15);
}
