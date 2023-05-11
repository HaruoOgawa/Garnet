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
var<private> inPosition_1: vec3<f32>;
var<private> fragColor: vec3<f32>;
var<private> inColor_1: vec3<f32>;
var<private> fragTexCoord: vec2<f32>;
var<private> inTexCoord_1: vec2<f32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
@group(0) @binding(1) 
var<uniform> testUBO: TestBuffer;

fn main_1() {
    let _e15 = inPosition_1;
    perVertexStruct.gl_Position = vec4<f32>(_e15.x, _e15.y, _e15.z, 1.0);
    let _e21 = inColor_1;
    fragColor = _e21;
    let _e22 = inTexCoord_1;
    fragTexCoord = _e22;
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
