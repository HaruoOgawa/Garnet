@group(0) @binding(2) 
var u_texture: texture_2d<f32>;
@group(0) @binding(3) 
var u_sampler: sampler;
var<private> fragTexCoord_1: vec2<f32>;
var<private> outColor: vec4<f32>;
var<private> fragColor_1: vec3<f32>;

fn main_1() {
    var texCol: vec4<f32>;

    let _e11 = fragTexCoord_1;
    let _e12 = textureSample(u_texture, u_sampler, _e11);
    texCol = _e12;
    let _e13 = fragColor_1;
    let _e14 = texCol;
    let _e16 = (_e13 * _e14.xyz);
    outColor = vec4<f32>(_e16.x, _e16.y, _e16.z, 1.0);
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
