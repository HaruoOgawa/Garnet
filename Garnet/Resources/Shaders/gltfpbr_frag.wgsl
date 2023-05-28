var<private> outColor: vec4<f32>;
var<private> f_WorldNormal_1: vec3<f32>;
var<private> f_Texcoord_1: vec2<f32>;

fn main_1() {
    let _e9 = f_WorldNormal_1;
    let _e12 = ((_e9 * 0.5) + vec3<f32>(0.5));
    outColor = vec4<f32>(_e12.x, _e12.y, _e12.z, 1.0);
    return;
}

@fragment 
fn main(@location(0) f_WorldNormal: vec3<f32>, @location(1) f_Texcoord: vec2<f32>) -> @location(0) vec4<f32> {
    f_WorldNormal_1 = f_WorldNormal;
    f_Texcoord_1 = f_Texcoord;
    main_1();
    let _e5 = outColor;
    return _e5;
}
