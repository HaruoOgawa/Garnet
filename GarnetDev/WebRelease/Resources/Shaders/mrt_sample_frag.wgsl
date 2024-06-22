struct Light {
    Posision: vec3<f32>,
    Color: vec3<f32>,
}

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
    var width: f32;
    var lightList: array<Light,5u>;
    var i: i32;
    var lightDir: vec3<f32>;
    var diffuse: vec3<f32>;
    var phi_69_: bool;
    var phi_82_: bool;
    var phi_95_: bool;
    var phi_108_: bool;

    col = vec4<f32>(0.0, 0.0, 0.0, 1.0);
    let _e45 = fUV_1;
    let _e46 = (_e45 * 2.0);
    let _e47 = vec2<f32>(1.0);
    st = (_e46 - (floor((_e46 / _e47)) * _e47));
    let _e52 = fUV_1;
    id = floor((_e52 * 2.0));
    let _e55 = st;
    let _e56 = textureSample(texGPosition, texGPositionSampler, _e55);
    GPositionCol = _e56;
    let _e57 = st;
    let _e58 = textureSample(texGNormal, texGNormalSampler, _e57);
    GNormalCol = _e58;
    let _e59 = st;
    let _e60 = textureSample(texGAlbedo, texGAlbedoSampler, _e59);
    GAlbedoCol = _e60;
    let _e62 = id[0u];
    let _e63 = (_e62 == 0.0);
    phi_69_ = _e63;
    if _e63 {
        let _e65 = id[1u];
        phi_69_ = (_e65 == 0.0);
    }
    let _e68 = phi_69_;
    if _e68 {
        let _e69 = GPositionCol;
        col = _e69;
    } else {
        let _e71 = id[0u];
        let _e72 = (_e71 == 0.0);
        phi_82_ = _e72;
        if _e72 {
            let _e74 = id[1u];
            phi_82_ = (_e74 == 1.0);
        }
        let _e77 = phi_82_;
        if _e77 {
            let _e78 = GNormalCol;
            col = _e78;
        } else {
            let _e80 = id[0u];
            let _e81 = (_e80 == 1.0);
            phi_95_ = _e81;
            if _e81 {
                let _e83 = id[1u];
                phi_95_ = (_e83 == 1.0);
            }
            let _e86 = phi_95_;
            if _e86 {
                let _e87 = GAlbedoCol;
                col = _e87;
            } else {
                let _e89 = id[0u];
                let _e90 = (_e89 == 1.0);
                phi_108_ = _e90;
                if _e90 {
                    let _e92 = id[1u];
                    phi_108_ = (_e92 == 0.0);
                }
                let _e95 = phi_108_;
                if _e95 {
                    width = 3.0;
                    lightList[0].Posision = vec3<f32>(0.0, 1.0, 0.0);
                    lightList[0].Color = vec3<f32>(1.0, 1.0, 1.0);
                    let _e100 = width;
                    lightList[1].Posision = vec3<f32>((1.0 * _e100), 1.0, 0.0);
                    lightList[1].Color = vec3<f32>(1.0, 0.0, 0.0);
                    let _e107 = width;
                    lightList[2].Posision = vec3<f32>((-1.0 * _e107), 1.0, 0.0);
                    lightList[2].Color = vec3<f32>(0.0, 1.0, 0.0);
                    let _e114 = width;
                    lightList[3].Posision = vec3<f32>((-2.0 * _e114), 1.0, 0.0);
                    lightList[3].Color = vec3<f32>(0.0, 0.0, 1.0);
                    let _e121 = width;
                    lightList[4].Posision = vec3<f32>((-2.0 * _e121), 1.0, 0.0);
                    lightList[4].Color = vec3<f32>(1.0, 0.0, 1.0);
                    i = 0;
                    loop {
                        let _e128 = i;
                        if (_e128 < 5) {
                            let _e130 = i;
                            let _e133 = lightList[_e130].Posision;
                            let _e134 = GPositionCol;
                            lightDir = normalize((_e133 - _e134.xyz));
                            let _e138 = GNormalCol;
                            let _e140 = lightDir;
                            let _e143 = GAlbedoCol;
                            let _e146 = i;
                            let _e149 = lightList[_e146].Color;
                            diffuse = ((_e143.xyz * max(0.0, dot(_e138.xyz, _e140))) * _e149);
                            let _e151 = diffuse;
                            let _e152 = col;
                            let _e154 = (_e152.xyz + _e151);
                            col[0u] = _e154.x;
                            col[1u] = _e154.y;
                            col[2u] = _e154.z;
                            continue;
                        } else {
                            break;
                        }
                        continuing {
                            let _e161 = i;
                            i = (_e161 + 1);
                        }
                    }
                }
            }
        }
    }
    let _e163 = col;
    outColor = _e163;
    return;
}

@fragment 
fn main(@location(0) fUV: vec2<f32>) -> @location(0) vec4<f32> {
    fUV_1 = fUV;
    main_1();
    let _e3 = outColor;
    return _e3;
}
