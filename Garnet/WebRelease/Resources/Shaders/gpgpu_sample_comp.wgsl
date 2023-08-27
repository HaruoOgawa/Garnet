struct TestData {
    offset: vec4<f32>,
    color: vec4<f32>,
    AccumulateDeltaTime: f32,
    pad0_: f32,
    pad1_: f32,
    pad2_: f32,
}

struct WriteOnlyTestBufferObject {
    data: array<TestData>,
}

struct ParamUBO {
    time: f32,
    deltaTime: f32,
    pad0_: f32,
    pad1_: f32,
}

struct ReadOnlyTestBufferObject {
    data: array<TestData>,
}

var<private> gl_GlobalInvocationID_1: vec3<u32>;
@group(0) @binding(2) 
var<storage, read_write> w_TBO: WriteOnlyTestBufferObject;
@group(0) @binding(0) 
var<uniform> ubo: ParamUBO;
@group(0) @binding(1) 
var<storage> r_TBO: ReadOnlyTestBufferObject;

fn randvf2_(st: ptr<function, vec2<f32>>) -> f32 {
    let _e21 = (*st);
    return fract((sin(dot(_e21, vec2<f32>(12.989800453186035, 78.23300170898438))) * 43758.546875));
}

fn main_1() {
    var id: u32;
    var id_f: f32;
    var param: vec2<f32>;

    let _e24 = gl_GlobalInvocationID_1[0u];
    id = _e24;
    let _e25 = id;
    id_f = f32(_e25);
    let _e27 = id;
    let _e29 = ubo.time;
    let _e30 = id_f;
    let _e31 = id_f;
    param = vec2<f32>(_e30, (_e31 + 12.394000053405762));
    let _e34 = randvf2_((&param));
    let _e43 = w_TBO.data[_e27].offset[1u];
    w_TBO.data[_e27].offset[1u] = (_e43 + (sin((_e29 + (_e34 * 10.0))) * 0.10000000149011612));
    return;
}

@compute @workgroup_size(256, 1, 1) 
fn main(@builtin(global_invocation_id) gl_GlobalInvocationID: vec3<u32>) {
    gl_GlobalInvocationID_1 = gl_GlobalInvocationID;
    main_1();
}
