struct UniformBufferObject {
    rate: f32,
    time: f32,
    alpha: f32,
    pad: f32,
}

var<private> fragTexCoord_1: vec2<f32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> outColor: vec4<f32>;

fn main_1() {
    var uv: vec2<f32>;
    var st: vec2<f32>;
    var pad: vec2<f32>;
    var col: vec3<f32>;
    var w: f32;
    var phi_56_: bool;
    var phi_63_: bool;
    var phi_69_: bool;

    let _e31 = fragTexCoord_1;
    uv = (vec2<f32>(1.0) - _e31);
    let _e34 = fragTexCoord_1;
    st = ((_e34 * 2.0) - vec2<f32>(1.0));
    pad = vec2<f32>(0.20000000298023224, 0.47999998927116394);
    let _e38 = uv;
    let _e39 = pad;
    let _e41 = pad;
    let _e46 = (vec2<f32>(1.0) / (vec2<f32>(1.0) - (_e41 * 2.0)));
    uv = ((_e38 - _e39) * vec2<f32>(_e46.x, _e46.y));
    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e52 = uv[0u];
    let _e53 = (_e52 >= 0.0);
    phi_56_ = _e53;
    if _e53 {
        let _e55 = uv[0u];
        phi_56_ = (_e55 <= 1.0);
    }
    let _e58 = phi_56_;
    phi_63_ = _e58;
    if _e58 {
        let _e60 = uv[1u];
        phi_63_ = (_e60 >= 0.0);
    }
    let _e63 = phi_63_;
    phi_69_ = _e63;
    if _e63 {
        let _e65 = uv[1u];
        phi_69_ = (_e65 <= 1.0);
    }
    let _e68 = phi_69_;
    if _e68 {
        let _e70 = uv[0u];
        let _e72 = ubo.rate;
        if (_e70 <= _e72) {
            let _e75 = uv[0u];
            col = mix(vec3<f32>(0.10000000149011612, 0.10000000149011612, 0.10000000149011612), vec3<f32>(1.0, 1.0, 1.0), vec3<f32>(_e75));
        } else {
            col = vec3<f32>(0.10000000149011612, 0.10000000149011612, 0.10000000149011612);
        }
    } else {
        w = 0.03999999910593033;
        let _e78 = st;
        let _e80 = w;
        let _e83 = w;
        if (length(max(vec2<f32>(0.0, 0.0), (abs(_e78) - vec2<f32>((0.5 + (_e80 * 2.3499999046325684)), _e83)))) <= 0.009999999776482582) {
            let _e89 = col;
            col = (_e89 + vec3<f32>(1.0, 1.0, 1.0));
        }
    }
    let _e91 = col;
    let _e93 = ubo.alpha;
    outColor = vec4<f32>(_e91.x, _e91.y, _e91.z, _e93);
    return;
}

@fragment 
fn main(@location(0) fragTexCoord: vec2<f32>) -> @location(0) vec4<f32> {
    fragTexCoord_1 = fragTexCoord;
    main_1();
    let _e3 = outColor;
    return _e3;
}
