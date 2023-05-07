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
    test: mat4x4<f32>,
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
var<private> inColor_1: vec3<f32>;
@group(0) @binding(1) 
var<uniform> testUBO: TestBuffer;
var<private> fragTexCoord: vec2<f32>;
var<private> inTexCoord_1: vec2<f32>;

fn main_1() {
    let _e18 = ubo.proj;
    let _e20 = ubo.view;
    let _e23 = ubo.model;
    let _e25 = inPosition_1;
    perVertexStruct.gl_Position = (((_e18 * _e20) * _e23) * vec4<f32>(_e25.x, _e25.y, _e25.z, 1.0));
    let _e32 = inColor_1;
    let _e34 = testUBO.MulColor;
    fragColor = (_e32 * _e34.xyz);
    let _e37 = inTexCoord_1;
    fragTexCoord = _e37;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inColor: vec3<f32>, @location(2) inTexCoord: vec2<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inColor_1 = inColor;
    inTexCoord_1 = inTexCoord;
    main_1();
    let _e11 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e11);
    let _e13 = perVertexStruct.gl_Position;
    let _e14 = fragColor;
    let _e15 = fragTexCoord;
    return VertexOutput(_e13, _e14, _e15);
}
