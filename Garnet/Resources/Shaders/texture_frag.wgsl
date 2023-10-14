@group(0) @binding(1) 
var texImage: texture_2d<f32>;
@group(0) @binding(2) 
var texSampler: sampler;
var<private> fUV_1: vec2<f32>;
var<private> outColor: vec4<f32>;
var<private> fWolrdNormal_1: vec3<f32>;

fn main_1() {
    var col: vec3<f32>;

    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e13 = fUV_1;
    let _e14 = textureSample(texImage, texSampler, _e13);
    col = _e14.xyz;
    let _e16 = col;
    outColor = vec4<f32>(_e16.x, _e16.y, _e16.z, 1.0);
    return;
}

@fragment 
fn main(@location(1) fUV: vec2<f32>, @location(0) fWolrdNormal: vec3<f32>) -> @location(0) vec4<f32> {
    fUV_1 = fUV;
    fWolrdNormal_1 = fWolrdNormal;
    main_1();
    let _e5 = outColor;
    return _e5;
}
