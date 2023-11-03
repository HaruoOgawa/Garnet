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
    cameraPos: vec4<f32>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
    @location(1) member_1: vec2<f32>,
    @location(2) member_2: vec3<f32>,
}

var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> fWolrdNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> fUV: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> fViewDir: vec3<f32>;
var<private> inTangent_1: vec4<f32>;

fn main_1() {
    let _e21 = ubo.proj;
    let _e23 = ubo.view;
    let _e26 = ubo.model;
    let _e28 = inPosition_1;
    perVertexStruct.gl_Position = (((_e21 * _e23) * _e26) * vec4<f32>(_e28.x, _e28.y, _e28.z, 1.0));
    let _e36 = ubo.model;
    let _e37 = inNormal_1;
    fWolrdNormal = (_e36 * vec4<f32>(_e37.x, _e37.y, _e37.z, 0.0)).xyz;
    let _e44 = inTexcoord_1;
    fUV = _e44;
    let _e46 = ubo.model;
    let _e47 = inPosition_1;
    let _e55 = ubo.cameraPos;
    fViewDir = normalize(((_e46 * vec4<f32>(_e47.x, _e47.y, _e47.z, 1.0)).xyz - _e55.xyz));
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(3) inTangent: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    inTangent_1 = inTangent;
    main_1();
    let _e14 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e14);
    let _e16 = perVertexStruct.gl_Position;
    let _e17 = fWolrdNormal;
    let _e18 = fUV;
    let _e19 = fViewDir;
    return VertexOutput(_e16, _e17, _e18, _e19);
}
