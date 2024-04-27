struct FragUniformBufferObject {
    mPad0_: mat4x4<f32>,
    mPad1_: mat4x4<f32>,
    mPad2_: mat4x4<f32>,
    mPad3_: mat4x4<f32>,
}

var<private> v2f_UV_1: vec2<f32>;
var<private> outColor: vec4<f32>;
@group(0) @binding(0) 
var<uniform> fragUbo: FragUniformBufferObject;

fn main_1() {
    var col: vec3<f32>;
    var uv: vec2<f32>;

    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e15 = v2f_UV_1[0u];
    let _e17 = v2f_UV_1[1u];
    uv = vec2<f32>(_e15, (1.0 - _e17));
    let _e20 = uv;
    col[0u] = _e20.x;
    col[1u] = _e20.y;
    let _e25 = col;
    outColor = vec4<f32>(_e25.x, _e25.y, _e25.z, 1.0);
    return;
}

@fragment 
fn main(@location(0) v2f_UV: vec2<f32>) -> @location(0) vec4<f32> {
    v2f_UV_1 = v2f_UV;
    main_1();
    let _e3 = outColor;
    return _e3;
}
