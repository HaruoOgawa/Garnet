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
}

var<private> gl_InstanceIndex_1: i32;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> fragTexCoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
var<private> inBioTangent_1: vec4<f32>;

fn randvf2_(st: ptr<function, vec2<f32>>) -> f32 {
    let _e29 = (*st);
    return fract((sin(dot(_e29, vec2<f32>(12.989800453186035, 78.23300170898438))) * 43758.546875));
}

fn main_1() {
    var id: i32;
    var f_id: f32;
    var w: f32;
    var offset: vec3<f32>;
    var param: vec2<f32>;
    var param_1: vec2<f32>;
    var param_2: vec2<f32>;

    let _e35 = gl_InstanceIndex_1;
    id = _e35;
    let _e36 = id;
    f_id = f32(_e36);
    w = 10.0;
    let _e38 = w;
    let _e39 = f_id;
    param = vec2<f32>(_e39, 55.5);
    let _e41 = randvf2_((&param));
    let _e45 = w;
    let _e46 = f_id;
    param_1 = vec2<f32>(943.219970703125, _e46);
    let _e48 = randvf2_((&param_1));
    let _e52 = w;
    let _e53 = f_id;
    let _e55 = f_id;
    param_2 = vec2<f32>((_e53 + 11.111000061035156), (_e55 + 456.12298583984375));
    let _e58 = randvf2_((&param_2));
    offset = vec3<f32>((_e38 * ((_e41 * 2.0) - 1.0)), (_e45 * ((_e48 * 2.0) - 1.0)), (_e52 * ((_e58 * 2.0) - 1.0)));
    let _e64 = ubo.proj;
    let _e66 = ubo.view;
    let _e69 = ubo.model;
    let _e71 = inPosition_1;
    let _e72 = offset;
    let _e73 = (_e71 + _e72);
    perVertexStruct.gl_Position = (((_e64 * _e66) * _e69) * vec4<f32>(_e73.x, _e73.y, _e73.z, 1.0));
    let _e80 = inTexcoord_1;
    fragTexCoord = _e80;
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
    let _e17 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e17);
    let _e19 = perVertexStruct.gl_Position;
    let _e20 = fragTexCoord;
    return VertexOutput(_e19, _e20);
}
