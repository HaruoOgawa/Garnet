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
@group(0) @binding(8) 
var shadowmapTexture: texture_2d<f32>;
@group(0) @binding(9) 
var shadowmapTextureSampler: sampler;
@group(0) @binding(1) 
var<uniform> testUBO: TestBuffer;
var<private> outColor: vec4<f32>;
var<private> fragColor_1: vec3<f32>;
@group(0) @binding(4) 
var u_NormalTexture: texture_2d<f32>;
@group(0) @binding(5) 
var u_NormalSampler: sampler;
@group(0) @binding(6) 
var cubemapTexture: texture_cube<f32>;
@group(0) @binding(7) 
var cubemapTextureSampler: sampler;

fn main_1() {
    var col: vec3<f32>;

    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e24 = fragTexCoord_1;
    col[0u] = _e24.x;
    col[1u] = _e24.y;
    let _e29 = fragTexCoord_1;
    let _e30 = textureSample(u_texture, u_sampler, _e29);
    let _e32 = fragTexCoord_1;
    let _e33 = textureSample(shadowmapTexture, shadowmapTextureSampler, _e32);
    let _e36 = testUBO.time;
    col = mix(_e30.xyz, _e33.xyz, vec3<f32>(((sin(_e36) * 0.5) + 0.5)));
    let _e42 = col;
    outColor = vec4<f32>(_e42.x, _e42.y, _e42.z, 1.0);
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
