struct UniformBufferObject {
    Kernal: array<f32,32u>,
    IsXBlur: i32,
    KernelSize: i32,
    Direction: vec2<f32>,
}

@group(0) @binding(1) 
var SrcTex: texture_2d<f32>;
@group(0) @binding(2) 
var SamplerSrcTex: sampler;
var<private> f_UV_1: vec2<f32>;
var<private> outColor: vec4<f32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;

fn main_1() {
    var col: vec3<f32>;

    let _e12 = f_UV_1;
    let _e13 = textureSample(SrcTex, SamplerSrcTex, _e12);
    col = _e13.xyz;
    let _e15 = col;
    outColor = vec4<f32>(_e15.x, _e15.y, _e15.z, 1.0);
    return;
}

@fragment 
fn main(@location(0) f_UV: vec2<f32>) -> @location(0) vec4<f32> {
    f_UV_1 = f_UV;
    main_1();
    let _e3 = outColor;
    return _e3;
}
