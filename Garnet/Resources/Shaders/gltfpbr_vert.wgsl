struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightDir: vec4<f32>,
    lightColor: vec4<f32>,
    cameraPos: vec4<f32>,
    baseColorFactor: vec4<f32>,
    emissiveFactor: vec4<f32>,
    time: f32,
    metallicFactor: f32,
    roughnessFactor: f32,
    padding2_: f32,
}

struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32,1u>,
    gl_CullDistance: array<f32,1u>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
    @location(1) member_1: vec2<f32>,
    @location(2) member_2: vec4<f32>,
}

var<private> inPosition_1: vec3<f32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
var<private> f_WorldNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;

fn main_1() {
    var pos: vec4<f32>;

    let _e23 = inPosition_1;
    pos = vec4<f32>(_e23.x, _e23.y, _e23.z, 1.0);
    let _e29 = ubo.time;
    let _e33 = ubo.time;
    let _e38 = ubo.time;
    let _e42 = ubo.time;
    let _e48 = pos;
    let _e50 = (_e48.xy * mat2x2<f32>(vec2<f32>(cos((_e29 * 0.5)), -(sin((_e33 * 0.5)))), vec2<f32>(sin((_e38 * 0.5)), cos((_e42 * 0.5)))));
    pos[0u] = _e50.x;
    pos[1u] = _e50.y;
    let _e56 = ubo.time;
    let _e60 = ubo.time;
    let _e65 = ubo.time;
    let _e69 = ubo.time;
    let _e75 = pos;
    let _e77 = (_e75.xz * mat2x2<f32>(vec2<f32>(cos((_e56 * 0.5)), -(sin((_e60 * 0.5)))), vec2<f32>(sin((_e65 * 0.5)), cos((_e69 * 0.5)))));
    pos[0u] = _e77.x;
    pos[2u] = _e77.y;
    let _e83 = ubo.time;
    let _e87 = ubo.time;
    let _e92 = ubo.time;
    let _e96 = ubo.time;
    let _e102 = pos;
    let _e104 = (_e102.yz * mat2x2<f32>(vec2<f32>(cos((_e83 * 0.5)), -(sin((_e87 * 0.5)))), vec2<f32>(sin((_e92 * 0.5)), cos((_e96 * 0.5)))));
    pos[1u] = _e104.x;
    pos[2u] = _e104.y;
    let _e110 = ubo.proj;
    let _e112 = ubo.view;
    let _e115 = ubo.model;
    let _e117 = pos;
    perVertexStruct.gl_Position = (((_e110 * _e112) * _e115) * _e117);
    let _e121 = ubo.model;
    let _e122 = inNormal_1;
    f_WorldNormal = (_e121 * vec4<f32>(_e122.x, _e122.y, _e122.z, 0.0)).xyz;
    let _e129 = inTexcoord_1;
    f_Texcoord = _e129;
    let _e131 = ubo.model;
    let _e132 = inPosition_1;
    f_WorldPos = (_e131 * vec4<f32>(_e132.x, _e132.y, _e132.z, 1.0));
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e12 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e12);
    let _e14 = perVertexStruct.gl_Position;
    let _e15 = f_WorldNormal;
    let _e16 = f_Texcoord;
    let _e17 = f_WorldPos;
    return VertexOutput(_e14, _e15, _e16, _e17);
}
