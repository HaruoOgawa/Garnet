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
var<private> inTangent_1: vec4<f32>;
var<private> inJoint0_1: vec4<u32>;
var<private> inWeights0_1: vec4<f32>;

fn main_1() {
    let _e22 = ubo.proj;
    let _e24 = ubo.view;
    let _e27 = ubo.model;
    let _e29 = inPosition_1;
    perVertexStruct.gl_Position = (((_e22 * _e24) * _e27) * vec4<f32>(_e29.x, _e29.y, _e29.z, 1.0));
    let _e37 = ubo.model;
    let _e38 = inNormal_1;
    f_WorldNormal = (_e37 * vec4<f32>(_e38.x, _e38.y, _e38.z, 0.0)).xyz;
    let _e45 = inTexcoord_1;
    f_Texcoord = _e45;
    let _e47 = ubo.model;
    let _e48 = inPosition_1;
    f_WorldPos = (_e47 * vec4<f32>(_e48.x, _e48.y, _e48.z, 1.0));
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(3) inTangent: vec4<f32>, @location(4) inJoint0_: vec4<u32>, @location(5) inWeights0_: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    inTangent_1 = inTangent;
    inJoint0_1 = inJoint0_;
    inWeights0_1 = inWeights0_;
    main_1();
    let _e18 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e18);
    let _e20 = perVertexStruct.gl_Position;
    let _e21 = f_WorldNormal;
    let _e22 = f_Texcoord;
    let _e23 = f_WorldPos;
    return VertexOutput(_e20, _e21, _e22, _e23);
}
