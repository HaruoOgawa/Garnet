var<private> fragTexCoord_1: vec2<f32>;
var<private> outColor: vec4<f32>;

fn main_1() {
    var uv: vec2<f32>;
    var pad: vec2<f32>;
    var col: vec3<f32>;
    var phi_42_: bool;
    var phi_49_: bool;
    var phi_55_: bool;

    let _e20 = fragTexCoord_1;
    uv = _e20;
    pad = vec2<f32>(0.20000000298023224, 0.4749999940395355);
    let _e21 = uv;
    let _e22 = pad;
    uv = ((_e21 - _e22) * vec2<f32>(1.6660000085830688, 20.0));
    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e26 = uv[0u];
    let _e27 = (_e26 >= 0.0);
    phi_42_ = _e27;
    if _e27 {
        let _e29 = uv[0u];
        phi_42_ = (_e29 <= 1.0);
    }
    let _e32 = phi_42_;
    phi_49_ = _e32;
    if _e32 {
        let _e34 = uv[1u];
        phi_49_ = (_e34 >= 0.0);
    }
    let _e37 = phi_49_;
    phi_55_ = _e37;
    if _e37 {
        let _e39 = uv[1u];
        phi_55_ = (_e39 <= 1.0);
    }
    let _e42 = phi_55_;
    if _e42 {
        let _e43 = uv;
        col[0u] = _e43.x;
        col[1u] = _e43.y;
    }
    let _e48 = col;
    outColor = vec4<f32>(_e48.x, _e48.y, _e48.z, 1.0);
    return;
}

@fragment 
fn main(@location(0) fragTexCoord: vec2<f32>) -> @location(0) vec4<f32> {
    fragTexCoord_1 = fragTexCoord;
    main_1();
    let _e3 = outColor;
    return _e3;
}
