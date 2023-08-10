var<private> fragTexCoord_1: vec2<f32>;
var<private> outColor: vec4<f32>;

fn main_1() {
    var col: vec3<f32>;

    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e12 = fragTexCoord_1;
    col[0u] = _e12.x;
    col[1u] = _e12.y;
    let _e17 = col;
    outColor = vec4<f32>(_e17.x, _e17.y, _e17.z, 1.0);
    return;
}

@fragment 
fn main(@location(0) fragTexCoord: vec2<f32>) -> @location(0) vec4<f32> {
    fragTexCoord_1 = fragTexCoord;
    main_1();
    let _e3 = outColor;
    return _e3;
}
