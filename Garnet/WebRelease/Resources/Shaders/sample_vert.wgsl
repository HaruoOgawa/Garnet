struct TestData {
    offset: vec4<f32>,
    color: vec4<f32>,
}

struct TestBufferObject {
    data: array<TestData>,
}

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
    @location(0) member: vec2<f32>,
    @location(1) member_1: vec4<f32>,
}

var<private> gl_InstanceIndex_1: i32;
@group(0) @binding(1) 
var<storage, read_write> rw_TBO: TestBufferObject;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> fragTexCoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> fragColor: vec4<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
var<private> inBioTangent_1: vec4<f32>;

fn main_1() {
    var id: i32;
    var f_id: f32;
    var w: f32;
    var offset: vec3<f32>;

    let _e25 = gl_InstanceIndex_1;
    id = _e25;
    let _e26 = id;
    f_id = f32(_e26);
    w = 10.0;
    let _e28 = id;
    let _e32 = rw_TBO.data[_e28].offset;
    offset = _e32.xyz;
    let _e35 = ubo.proj;
    let _e37 = ubo.view;
    let _e40 = ubo.model;
    let _e42 = inPosition_1;
    let _e43 = offset;
    let _e44 = (_e42 + _e43);
    perVertexStruct.gl_Position = (((_e35 * _e37) * _e40) * vec4<f32>(_e44.x, _e44.y, _e44.z, 1.0));
    let _e51 = inTexcoord_1;
    fragTexCoord = _e51;
    let _e52 = id;
    let _e56 = rw_TBO.data[_e52].color;
    fragColor = _e56;
    return;
}

@vertex 
fn main(@builtin(instance_index) gl_InstanceIndex: u32, @location(0) inPosition: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(4) inBioTangent: vec4<f32>) -> VertexOutput {
    gl_InstanceIndex_1 = i32(gl_InstanceIndex);
    inPosition_1 = inPosition;
    inTexcoord_1 = inTexcoord;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inBioTangent_1 = inBioTangent;
    main_1();
    let _e18 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e18);
    let _e20 = perVertexStruct.gl_Position;
    let _e21 = fragTexCoord;
    let _e22 = fragColor;
    return VertexOutput(_e20, _e21, _e22);
}
