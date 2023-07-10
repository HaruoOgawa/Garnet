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
    lightView: mat4x4<f32>,
}

struct TestBuffer {
    MulColor: vec4<f32>,
    val0_: vec4<f32>,
    val1_: vec4<f32>,
    val2_: vec4<f32>,
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
var<private> fragColor: vec3<f32>;
var<private> fragTexCoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
var<private> inBioTangent_1: vec4<f32>;
@group(0) @binding(1) 
var<uniform> testUBO: TestBuffer;

fn main_1() {
    let _e21 = ubo.proj;
    let _e23 = ubo.view;
    let _e26 = ubo.model;
    let _e28 = inPosition_1;
    perVertexStruct.gl_Position = (((_e21 * _e23) * _e26) * vec4<f32>(_e28.x, _e28.y, _e28.z, 1.0));
    fragColor = vec3<f32>(1.0, 1.0, 1.0);
    let _e35 = inTexcoord_1;
    fragTexCoord = _e35;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(4) inBioTangent: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTexcoord_1 = inTexcoord;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inBioTangent_1 = inBioTangent;
    main_1();
    let _e15 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e15);
    let _e17 = perVertexStruct.gl_Position;
    let _e18 = fragColor;
    let _e19 = fragTexCoord;
    return VertexOutput(_e17, _e18, _e19);
}
