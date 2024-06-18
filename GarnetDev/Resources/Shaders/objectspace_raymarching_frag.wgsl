struct FragmentUniformBuffer {
    invModel: mat4x4<f32>,
    mPad0_: mat4x4<f32>,
    mPad1_: mat4x4<f32>,
    mPad2_: mat4x4<f32>,
    cameraPos: vec4<f32>,
    v4Pad0_: vec4<f32>,
    v4Pad1_: vec4<f32>,
    v4Pad2_: vec4<f32>,
    time: f32,
    fPad0_: f32,
    fPad1_: f32,
    fPad2_: f32,
}

struct FragmentOutput {
    @location(0) member: vec4<f32>,
    @location(1) member_1: vec4<f32>,
    @location(2) member_2: vec4<f32>,
}

@group(0) @binding(1) 
var<uniform> f_ubo: FragmentUniformBuffer;
var<private> v2f_ObjectPos_1: vec4<f32>;
var<private> gPosition: vec4<f32>;
var<private> gNormal: vec4<f32>;
var<private> gAlbedo: vec4<f32>;

fn mapvf3_(p: ptr<function, vec3<f32>>) -> f32 {
    let _e21 = (*p);
    return (length(_e21) - 0.5);
}

fn gnvf3_(p_1: ptr<function, vec3<f32>>) -> vec3<f32> {
    var e: vec2<f32>;
    var param: vec3<f32>;
    var param_1: vec3<f32>;
    var param_2: vec3<f32>;
    var param_3: vec3<f32>;
    var param_4: vec3<f32>;
    var param_5: vec3<f32>;

    e = vec2<f32>(9.999999747378752e-5, 0.0);
    let _e28 = (*p_1);
    let _e29 = e;
    param = (_e28 + _e29.xyy);
    let _e32 = mapvf3_((&param));
    let _e33 = (*p_1);
    let _e34 = e;
    param_1 = (_e33 - _e34.xyy);
    let _e37 = mapvf3_((&param_1));
    let _e39 = (*p_1);
    let _e40 = e;
    param_2 = (_e39 + _e40.yxy);
    let _e43 = mapvf3_((&param_2));
    let _e44 = (*p_1);
    let _e45 = e;
    param_3 = (_e44 - _e45.yxy);
    let _e48 = mapvf3_((&param_3));
    let _e50 = (*p_1);
    let _e51 = e;
    param_4 = (_e50 + _e51.yyx);
    let _e54 = mapvf3_((&param_4));
    let _e55 = (*p_1);
    let _e56 = e;
    param_5 = (_e55 - _e56.yyx);
    let _e59 = mapvf3_((&param_5));
    return normalize(vec3<f32>((_e32 - _e37), (_e43 - _e48), (_e54 - _e59)));
}

fn main_1() {
    var ro: vec3<f32>;
    var rd: vec3<f32>;
    var d: f32;
    var t: f32;
    var i: i32;
    var param_6: vec3<f32>;
    var p_2: vec3<f32>;
    var n: vec3<f32>;
    var param_7: vec3<f32>;

    let _e30 = f_ubo.invModel;
    let _e32 = f_ubo.cameraPos;
    ro = (_e30 * _e32).xyz;
    let _e35 = v2f_ObjectPos_1;
    let _e37 = ro;
    rd = normalize((_e35.xyz - _e37));
    d = 1.0;
    t = 0.0;
    i = 0;
    loop {
        let _e40 = i;
        if (_e40 < 64) {
            let _e42 = ro;
            let _e43 = rd;
            let _e44 = t;
            param_6 = (_e42 + (_e43 * _e44));
            let _e47 = mapvf3_((&param_6));
            d = _e47;
            let _e48 = d;
            if (_e48 < 9.999999747378752e-5) {
                break;
            }
            let _e50 = d;
            let _e51 = t;
            t = (_e51 + _e50);
            continue;
        } else {
            break;
        }
        continuing {
            let _e53 = i;
            i = (_e53 + 1);
        }
    }
    let _e55 = d;
    let _e57 = t;
    if ((_e55 < 9.999999747378752e-5) && (_e57 < 100.0)) {
        let _e60 = ro;
        let _e61 = rd;
        let _e62 = t;
        p_2 = (_e60 + (_e61 * _e62));
        let _e65 = p_2;
        param_7 = _e65;
        let _e66 = gnvf3_((&param_7));
        n = _e66;
        let _e67 = p_2;
        gPosition = vec4<f32>(_e67.x, _e67.y, _e67.z, 1.0);
        let _e72 = n;
        gNormal = vec4<f32>(_e72.x, _e72.y, _e72.z, 1.0);
        gAlbedo = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    } else {
        discard;
    }
    return;
}

@fragment 
fn main(@location(0) v2f_ObjectPos: vec4<f32>) -> FragmentOutput {
    v2f_ObjectPos_1 = v2f_ObjectPos;
    main_1();
    let _e5 = gPosition;
    let _e6 = gNormal;
    let _e7 = gAlbedo;
    return FragmentOutput(_e5, _e6, _e7);
}
