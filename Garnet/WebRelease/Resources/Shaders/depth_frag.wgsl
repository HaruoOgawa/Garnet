var<private> fragPos_1: vec4<f32>;
var<private> outColor: vec4<f32>;

fn main_1() {
    var depth: f32;
    var moment1_: f32;
    var moment2_: f32;
    var dx: f32;
    var dy: f32;

    let _e17 = fragPos_1[2u];
    let _e19 = fragPos_1[3u];
    depth = (_e17 / _e19);
    let _e21 = depth;
    depth = ((_e21 * 0.5) + 0.5);
    let _e24 = depth;
    moment1_ = _e24;
    let _e25 = depth;
    let _e26 = depth;
    moment2_ = (_e25 * _e26);
    let _e28 = depth;
    let _e29 = dpdx(_e28);
    dx = _e29;
    let _e30 = depth;
    let _e31 = dpdy(_e30);
    dy = _e31;
    let _e32 = dx;
    let _e33 = dx;
    let _e35 = dy;
    let _e36 = dy;
    let _e40 = moment2_;
    moment2_ = (_e40 + (0.25 * ((_e32 * _e33) + (_e35 * _e36))));
    let _e42 = moment1_;
    let _e43 = moment2_;
    outColor = vec4<f32>(_e42, _e43, 0.0, 0.0);
    return;
}

@fragment 
fn main(@location(0) fragPos: vec4<f32>) -> @location(0) vec4<f32> {
    fragPos_1 = fragPos;
    main_1();
    let _e3 = outColor;
    return _e3;
}
