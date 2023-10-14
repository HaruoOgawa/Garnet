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
var<private> fWolrdNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> fUV: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> inTangent_1: vec4<f32>;

fn main_1() {
    let _e19 = ubo.proj;
    let _e21 = ubo.view;
    let _e24 = ubo.model;
    let _e26 = inPosition_1;
    perVertexStruct.gl_Position = (((_e19 * _e21) * _e24) * vec4<f32>(_e26.x, _e26.y, _e26.z, 1.0));
    let _e34 = ubo.model;
    let _e35 = inNormal_1;
    fWolrdNormal = (_e34 * vec4<f32>(_e35.x, _e35.y, _e35.z, 0.0)).xyz;
    let _e42 = inTexcoord_1;
    fUV = _e42;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(3) inTangent: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    inTangent_1 = inTangent;
    main_1();
    let _e13 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e13);
    let _e15 = perVertexStruct.gl_Position;
    let _e16 = fWolrdNormal;
    let _e17 = fUV;
    return VertexOutput(_e15, _e16, _e17);
}
