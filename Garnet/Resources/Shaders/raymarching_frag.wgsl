struct FragUniformBufferObject {
    mPad0_: mat4x4<f32>,
    mPad1_: mat4x4<f32>,
    mPad2_: mat4x4<f32>,
    mPad3_: mat4x4<f32>,
    resolution: vec2<f32>,
    time: f32,
    deltaTime: f32,
}

@group(0) @binding(0) 
var<uniform> fragUbo: FragUniformBufferObject;
var<private> v2f_UV_1: vec2<f32>;
var<private> outColor: vec4<f32>;

fn sdCrossvf3_(p: ptr<function, vec3<f32>>) -> f32 {
    var dxy: f32;
    var dyz: f32;
    var dzx: f32;

    let _e34 = (*p);
    (*p) = abs(_e34);
    let _e37 = (*p)[0u];
    let _e39 = (*p)[1u];
    dxy = max(_e37, _e39);
    let _e42 = (*p)[1u];
    let _e44 = (*p)[2u];
    dyz = max(_e42, _e44);
    let _e47 = (*p)[2u];
    let _e49 = (*p)[0u];
    dzx = max(_e47, _e49);
    let _e51 = dxy;
    let _e52 = dyz;
    let _e53 = dzx;
    return (min(_e51, min(_e52, _e53)) - 1.0);
}

fn Cubevf3f1_(p_1: ptr<function, vec3<f32>>, s: ptr<function, f32>) -> f32 {
    let _e32 = (*p_1);
    (*p_1) = abs(_e32);
    let _e34 = (*p_1);
    let _e35 = (*s);
    return length(max((_e34 - vec3<f32>(_e35)), vec3<f32>(0.0)));
}

fn pmodvf2f1_(p_2: ptr<function, vec2<f32>>, n: ptr<function, f32>) -> vec2<f32> {
    var a: f32;
    var t: f32;

    let _e34 = (*n);
    a = (6.2831854820251465 / _e34);
    let _e37 = (*p_2)[0u];
    let _e39 = (*p_2)[1u];
    let _e41 = (*n);
    t = (atan2(_e37, _e39) - (3.1415927410125732 / _e41));
    let _e44 = t;
    let _e45 = a;
    let _e50 = (*n);
    t = ((_e44 - (floor((_e44 / _e45)) * _e45)) - (3.1415927410125732 / _e50));
    let _e53 = (*p_2);
    let _e55 = t;
    let _e58 = (*p_2);
    let _e60 = t;
    return vec2<f32>((length(_e53) * cos(_e55)), (length(_e58) * sin(_e60)));
}

fn mapvf3_(p_3: ptr<function, vec3<f32>>) -> f32 {
    var param: vec2<f32>;
    var param_1: f32;
    var k: f32;
    var s_1: f32;
    var d: f32;
    var param_2: vec3<f32>;
    var param_3: f32;
    var scale: f32;
    var i: i32;
    var param_4: vec3<f32>;

    let _e42 = fragUbo.time;
    let _e44 = (*p_3)[2u];
    (*p_3)[2u] = (_e44 + _e42);
    let _e47 = (*p_3);
    param = _e47.xy;
    param_1 = 6.0;
    let _e49 = pmodvf2f1_((&param), (&param_1));
    (*p_3)[0u] = _e49.x;
    (*p_3)[1u] = _e49.y;
    k = 4.0;
    let _e54 = (*p_3);
    let _e55 = k;
    let _e56 = vec3<f32>(_e55);
    let _e61 = k;
    (*p_3) = ((_e54 - (floor((_e54 / _e56)) * _e56)) - vec3<f32>((0.5 * _e61)));
    s_1 = 2.0;
    let _e65 = (*p_3);
    param_2 = _e65;
    let _e66 = s_1;
    param_3 = _e66;
    let _e67 = Cubevf3f1_((&param_2), (&param_3));
    d = _e67;
    scale = 6.0;
    i = 0;
    loop {
        let _e68 = i;
        if (_e68 < 6) {
            let _e70 = (*p_3);
            let _e71 = vec3<f32>(2.0);
            (*p_3) = ((_e70 - (floor((_e70 / _e71)) * _e71)) - vec3<f32>(1.0));
            let _e78 = scale;
            let _e79 = s_1;
            s_1 = (_e79 * _e78);
            let _e81 = scale;
            let _e82 = (*p_3);
            (*p_3) = (vec3<f32>(1.0) - (abs(_e82) * _e81));
            let _e87 = d;
            let _e88 = (*p_3);
            param_4 = _e88;
            let _e89 = sdCrossvf3_((&param_4));
            let _e90 = s_1;
            d = max(_e87, (_e89 / _e90));
            continue;
        } else {
            break;
        }
        continuing {
            let _e93 = i;
            i = (_e93 + 1);
        }
    }
    let _e95 = d;
    return _e95;
}

fn gnvf3_(p_4: ptr<function, vec3<f32>>) -> vec3<f32> {
    var e: vec2<f32>;
    var param_5: vec3<f32>;
    var param_6: vec3<f32>;
    var param_7: vec3<f32>;
    var param_8: vec3<f32>;
    var param_9: vec3<f32>;
    var param_10: vec3<f32>;

    e = vec2<f32>(0.0010000000474974513, 0.0);
    let _e38 = (*p_4);
    let _e39 = e;
    param_5 = (_e38 + _e39.xyy);
    let _e42 = mapvf3_((&param_5));
    let _e43 = (*p_4);
    let _e44 = e;
    param_6 = (_e43 - _e44.xyy);
    let _e47 = mapvf3_((&param_6));
    let _e49 = (*p_4);
    let _e50 = e;
    param_7 = (_e49 + _e50.yxy);
    let _e53 = mapvf3_((&param_7));
    let _e54 = (*p_4);
    let _e55 = e;
    param_8 = (_e54 - _e55.yxy);
    let _e58 = mapvf3_((&param_8));
    let _e60 = (*p_4);
    let _e61 = e;
    param_9 = (_e60 + _e61.yyx);
    let _e64 = mapvf3_((&param_9));
    let _e65 = (*p_4);
    let _e66 = e;
    param_10 = (_e65 - _e66.yyx);
    let _e69 = mapvf3_((&param_10));
    return normalize(vec3<f32>((_e42 - _e47), (_e53 - _e58), (_e64 - _e69)));
}

fn main_1() {
    var col: vec3<f32>;
    var uv: vec2<f32>;
    var st: vec2<f32>;
    var ro: vec3<f32>;
    var rd: vec3<f32>;
    var d_1: f32;
    var t_1: f32;
    var i_1: i32;
    var param_11: vec3<f32>;
    var n_1: vec3<f32>;
    var param_12: vec3<f32>;

    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e42 = v2f_UV_1[0u];
    let _e44 = v2f_UV_1[1u];
    uv = vec2<f32>(_e42, (1.0 - _e44));
    let _e47 = uv;
    st = ((_e47 * 2.0) - vec2<f32>(1.0));
    let _e53 = fragUbo.resolution[0u];
    let _e56 = fragUbo.resolution[1u];
    let _e59 = st[0u];
    st[0u] = (_e59 * (_e53 / _e56));
    ro = vec3<f32>(0.0, 0.0, 1.0);
    let _e62 = st;
    rd = normalize(vec3<f32>(_e62.x, _e62.y, -1.0));
    d_1 = 1.0;
    t_1 = 0.0;
    i_1 = 0;
    loop {
        let _e67 = i_1;
        if (_e67 < 64) {
            let _e69 = ro;
            let _e70 = rd;
            let _e71 = t_1;
            param_11 = (_e69 + (_e70 * _e71));
            let _e74 = mapvf3_((&param_11));
            d_1 = _e74;
            let _e75 = d_1;
            if (_e75 < 0.0010000000474974513) {
                break;
            }
            let _e77 = d_1;
            let _e78 = t_1;
            t_1 = (_e78 + _e77);
            continue;
        } else {
            break;
        }
        continuing {
            let _e80 = i_1;
            i_1 = (_e80 + 1);
        }
    }
    let _e82 = t_1;
    col = (vec3<f32>(1.0, 1.0, 1.0) * exp((-1.0 * _e82)));
    let _e86 = ro;
    let _e87 = rd;
    let _e88 = t_1;
    param_12 = (_e86 + (_e87 * _e88));
    let _e91 = gnvf3_((&param_12));
    n_1 = _e91;
    let _e92 = d_1;
    if (_e92 < 0.0010000000474974513) {
        let _e94 = n_1;
        col = ((_e94 * 0.5) + vec3<f32>(0.5));
    }
    let _e98 = col;
    outColor = vec4<f32>(_e98.x, _e98.y, _e98.z, 1.0);
    return;
}

@fragment 
fn main(@location(0) v2f_UV: vec2<f32>) -> @location(0) vec4<f32> {
    v2f_UV_1 = v2f_UV;
    main_1();
    let _e3 = outColor;
    return _e3;
}
