struct TestBuffer {
    UBOColor: vec4<f32>,
    time: f32,
    pad_0_: f32,
    pad_1_: f32,
    pad_2_: f32,
}

var<private> fragTexCoord_1: vec2<f32>;
@group(0) @binding(2) 
var u_texture: texture_2d<f32>;
@group(0) @binding(3) 
var u_sampler: sampler;
@group(0) @binding(4) 
var u_NormalTexture: texture_2d<f32>;
@group(0) @binding(5) 
var u_NormalSampler: sampler;
@group(0) @binding(1) 
var<uniform> testUBO: TestBuffer;
var<private> outColor: vec4<f32>;
var<private> fragColor_1: vec3<f32>;

fn main_1() {
    var col: vec3<f32>;

    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e20 = fragTexCoord_1;
    col[0u] = _e20.x;
    col[1u] = _e20.y;
    let _e25 = fragTexCoord_1;
    let _e26 = textureSample(u_texture, u_sampler, _e25);
    let _e28 = fragTexCoord_1;
    let _e29 = textureSample(u_NormalTexture, u_NormalSampler, _e28);
    let _e32 = testUBO.time;
    col = mix(_e26.xyz, _e29.xyz, vec3<f32>(((sin(_e32) * 0.5) + 0.5)));
    let _e38 = col;
    outColor = vec4<f32>(_e38.x, _e38.y, _e38.z, 1.0);
    return;
}

@fragment 
fn main(@location(1) fragTexCoord: vec2<f32>, @location(0) fragColor: vec3<f32>) -> @location(0) vec4<f32> {
    fragTexCoord_1 = fragTexCoord;
    fragColor_1 = fragColor;
    main_1();
    let _e5 = outColor;
    return _e5;
}
