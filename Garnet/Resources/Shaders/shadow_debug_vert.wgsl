struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightVPMat: mat4x4<f32>,
}

struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32,1u>,
    gl_CullDistance: array<f32,1u>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec2<f32>,
}

@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
var<private> inPosition_1: vec3<f32>;
var<private> fragTexCoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
var<private> inBioTangent_1: vec4<f32>;

fn main_1() {
    var mvmat: mat4x4<f32>;

    let _e23 = ubo.view;
    let _e25 = ubo.model;
    mvmat = (_e23 * _e25);
    mvmat[0][0u] = 5.0;
    mvmat[0][1u] = 0.0;
    mvmat[0][2u] = 0.0;
    mvmat[1][0u] = 0.0;
    mvmat[1][1u] = 5.0;
    mvmat[1][2u] = 0.0;
    mvmat[2][0u] = 0.0;
    mvmat[2][1u] = 0.0;
    mvmat[2][2u] = 5.0;
    let _e46 = ubo.proj;
    let _e47 = mvmat;
    let _e49 = inPosition_1;
    perVertexStruct.gl_Position = ((_e46 * _e47) * vec4<f32>(_e49.x, _e49.y, _e49.z, 1.0));
    let _e56 = inTexcoord_1;
    fragTexCoord = _e56;
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
    let _e14 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e14);
    let _e16 = perVertexStruct.gl_Position;
    let _e17 = fragTexCoord;
    return VertexOutput(_e16, _e17);
}
