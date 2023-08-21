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

fn main_1() {
    var id: u32;

    let _e15 = gl_GlobalInvocationID_1[0u];
    id = _e15;
    let _e16 = id;
    let _e18 = ubo.time;
    let _e24 = w_TBO.data[_e16].offset[1u];
    w_TBO.data[_e16].offset[1u] = (_e24 + sin(_e18));
    return;
}

@compute @workgroup_size(256, 1, 1) 
fn main(@builtin(global_invocation_id) gl_GlobalInvocationID: vec3<u32>) {
    gl_GlobalInvocationID_1 = gl_GlobalInvocationID;
    main_1();
}
