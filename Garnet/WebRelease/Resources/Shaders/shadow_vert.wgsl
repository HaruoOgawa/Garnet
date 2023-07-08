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
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec2<f32>,
}

var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> fragTexCoord: vec2<f32>;
var<private> inTexCoord_1: vec2<f32>;
var<private> inColor_1: vec3<f32>;

fn main_1() {
    let _e16 = ubo.proj;
    let _e18 = ubo.view;
    let _e21 = ubo.model;
    let _e23 = inPosition_1;
    perVertexStruct.gl_Position = (((_e16 * _e18) * _e21) * vec4<f32>(_e23.x, _e23.y, _e23.z, 1.0));
    let _e30 = inTexCoord_1;
    fragTexCoord = _e30;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(2) inTexCoord: vec2<f32>, @location(1) inColor: vec3<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTexCoord_1 = inTexCoord;
    inColor_1 = inColor;
    main_1();
    let _e10 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e10);
    let _e12 = perVertexStruct.gl_Position;
    let _e13 = fragTexCoord;
    return VertexOutput(_e12, _e13);
}
