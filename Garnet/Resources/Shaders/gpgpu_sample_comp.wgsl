struct TestData {
    offset: vec4<f32>,
    color: vec4<f32>,
}

struct WriteOnlyTestBufferObject {
    data: array<TestData>,
}

struct ParamUBO {
    time: f32,
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
    let _e20 = (*st);
    return fract((sin(dot(_e20, vec2<f32>(12.989800453186035, 78.23300170898438))) * 43758.546875));
}

fn main_1() {
    var id: u32;
    var id_f: f32;
    var param: vec2<f32>;

    let _e23 = gl_GlobalInvocationID_1[0u];
    id = _e23;
    let _e24 = id;
    id_f = f32(_e24);
    let _e26 = id;
    let _e28 = ubo.time;
    let _e29 = id_f;
    let _e30 = id_f;
    param = vec2<f32>(_e29, (_e30 + 12.394000053405762));
    let _e33 = randvf2_((&param));
    let _e41 = w_TBO.data[_e26].offset[1u];
    w_TBO.data[_e26].offset[1u] = (_e41 + sin((_e28 + (_e33 * 10.0))));
    return;
}

@compute @workgroup_size(256, 1, 1) 
fn main(@builtin(global_invocation_id) gl_GlobalInvocationID: vec3<u32>) {
    gl_GlobalInvocationID_1 = gl_GlobalInvocationID;
    main_1();
}
