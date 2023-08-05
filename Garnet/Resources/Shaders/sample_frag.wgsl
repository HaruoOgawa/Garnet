var<private> outColor: vec4<f32>;
var<private> fragColor_1: vec3<f32>;
var<private> fragTexCoord_1: vec2<f32>;

fn main_1() {
    outColor = vec4<f32>(1.0, 0.0, 0.0, 1.0);
    return;
}

@fragment 
fn main(@location(0) fragColor: vec3<f32>, @location(1) fragTexCoord: vec2<f32>) -> @location(0) vec4<f32> {
    fragColor_1 = fragColor;
    fragTexCoord_1 = fragTexCoord;
    main_1();
    let _e5 = outColor;
    return _e5;
}
