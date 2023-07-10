var<private> gl_FragCoord_1: vec4<f32>;
var<private> outColor: vec4<f32>;
var<private> fragPos_1: vec4<f32>;

fn main_1() {
    var depth: f32;
    var moment1_: f32;
    var moment2_: f32;
    var dx: f32;
    var dy: f32;

    let _e16 = gl_FragCoord_1[2u];
    depth = _e16;
    let _e17 = depth;
    moment1_ = _e17;
    let _e18 = depth;
    let _e19 = depth;
    moment2_ = (_e18 * _e19);
    let _e21 = depth;
    let _e22 = dpdx(_e21);
    dx = _e22;
    let _e23 = depth;
    let _e24 = dpdy(_e23);
    dy = _e24;
    let _e25 = dx;
    let _e26 = dx;
    let _e28 = dy;
    let _e29 = dy;
    let _e33 = moment2_;
    moment2_ = (_e33 + (0.25 * ((_e25 * _e26) + (_e28 * _e29))));
    let _e35 = moment1_;
    let _e36 = moment2_;
    outColor = vec4<f32>(_e35, _e36, 0.0, 0.0);
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(0) fragPos: vec4<f32>) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    fragPos_1 = fragPos;
    main_1();
    let _e5 = outColor;
    return _e5;
}
