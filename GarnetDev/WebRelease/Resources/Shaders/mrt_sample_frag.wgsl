var<private> fUV_1: vec2<f32>;
@group(0) @binding(1) 
var texGPosition: texture_2d<f32>;
@group(0) @binding(2) 
var texGPositionSampler: sampler;
@group(0) @binding(3) 
var texGNormal: texture_2d<f32>;
@group(0) @binding(4) 
var texGNormalSampler: sampler;
@group(0) @binding(5) 
var texGAlbedo: texture_2d<f32>;
@group(0) @binding(6) 
var texGAlbedoSampler: sampler;
var<private> outColor: vec4<f32>;

fn main_1() {
    var col: vec4<f32>;
    var st: vec2<f32>;
    var id: vec2<f32>;
    var GPositionCol: vec4<f32>;
    var GNormalCol: vec4<f32>;
    var GAlbedoCol: vec4<f32>;
    var phi_69_: bool;
    var phi_82_: bool;
    var phi_95_: bool;
    var phi_108_: bool;

    col = vec4<f32>(0.0, 0.0, 0.0, 0.0);
    let _e24 = fUV_1;
    let _e25 = (_e24 * 2.0);
    let _e26 = vec2<f32>(1.0);
    st = (_e25 - (floor((_e25 / _e26)) * _e26));
    let _e31 = fUV_1;
    id = floor((_e31 * 2.0));
    let _e34 = st;
    let _e35 = textureSample(texGPosition, texGPositionSampler, _e34);
    GPositionCol = _e35;
    let _e36 = st;
    let _e37 = textureSample(texGNormal, texGNormalSampler, _e36);
    GNormalCol = _e37;
    let _e38 = st;
    let _e39 = textureSample(texGAlbedo, texGAlbedoSampler, _e38);
    GAlbedoCol = _e39;
    let _e41 = id[0u];
    let _e42 = (_e41 == 0.0);
    phi_69_ = _e42;
    if _e42 {
        let _e44 = id[1u];
        phi_69_ = (_e44 == 0.0);
    }
    let _e47 = phi_69_;
    if _e47 {
        let _e48 = GPositionCol;
        col = _e48;
    } else {
        let _e50 = id[0u];
        let _e51 = (_e50 == 0.0);
        phi_82_ = _e51;
        if _e51 {
            let _e53 = id[1u];
            phi_82_ = (_e53 == 1.0);
        }
        let _e56 = phi_82_;
        if _e56 {
            let _e57 = GNormalCol;
            col = _e57;
        } else {
            let _e59 = id[0u];
            let _e60 = (_e59 == 1.0);
            phi_95_ = _e60;
            if _e60 {
                let _e62 = id[1u];
                phi_95_ = (_e62 == 1.0);
            }
            let _e65 = phi_95_;
            if _e65 {
                let _e66 = GAlbedoCol;
                col = _e66;
            } else {
                let _e68 = id[0u];
                let _e69 = (_e68 == 1.0);
                phi_108_ = _e69;
                if _e69 {
                    let _e71 = id[1u];
                    phi_108_ = (_e71 == 0.0);
                }
                let _e74 = phi_108_;
                if _e74 {
                }
            }
        }
    }
    let _e75 = col;
    outColor = _e75;
    return;
}

@fragment 
fn main(@location(0) fUV: vec2<f32>) -> @location(0) vec4<f32> {
    fUV_1 = fUV;
    main_1();
    let _e3 = outColor;
    return _e3;
}
