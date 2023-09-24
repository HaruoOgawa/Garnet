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
    @location(0) member: vec4<f32>,
}

var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> fragPos: vec4<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> inTangent_1: vec4<f32>;

fn main_1() {
    let _e16 = ubo.lightVPMat;
    let _e18 = ubo.model;
    let _e20 = inPosition_1;
    perVertexStruct.gl_Position = ((_e16 * _e18) * vec4<f32>(_e20.x, _e20.y, _e20.z, 1.0));
    let _e28 = perVertexStruct.gl_Position;
    fragPos = _e28;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(3) inTangent: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    inTangent_1 = inTangent;
    main_1();
    let _e12 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e12);
    let _e14 = perVertexStruct.gl_Position;
    let _e15 = fragPos;
    return VertexOutput(_e14, _e15);
}
