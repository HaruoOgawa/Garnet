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
    lightDir: vec4<f32>,
    lightColor: vec4<f32>,
    cameraPos: vec4<f32>,
    baseColorFactor: vec4<f32>,
    emissiveFactor: vec4<f32>,
    time: f32,
    metallicFactor: f32,
    roughnessFactor: f32,
    useBaseColorTexture: i32,
    useMetallicRoughnessTexture: i32,
    useEmissiveTexture: i32,
    useNormalTexture: i32,
    useOcclusionTexture: i32,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
    @location(1) member_1: vec2<f32>,
    @location(2) member_2: vec4<f32>,
}

var<private> inPosition_1: vec3<f32>;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> f_WorldNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;

fn main_1() {
    var pos: vec4<f32>;

    let _e19 = inPosition_1;
    pos = vec4<f32>(_e19.x, _e19.y, _e19.z, 1.0);
    let _e25 = ubo.proj;
    let _e27 = ubo.view;
    let _e30 = ubo.model;
    let _e32 = pos;
    perVertexStruct.gl_Position = (((_e25 * _e27) * _e30) * _e32);
    let _e36 = ubo.model;
    let _e37 = inNormal_1;
    f_WorldNormal = (_e36 * vec4<f32>(_e37.x, _e37.y, _e37.z, 0.0)).xyz;
    let _e44 = inTexcoord_1;
    f_Texcoord = _e44;
    let _e46 = ubo.model;
    let _e47 = inPosition_1;
    f_WorldPos = (_e46 * vec4<f32>(_e47.x, _e47.y, _e47.z, 1.0));
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
