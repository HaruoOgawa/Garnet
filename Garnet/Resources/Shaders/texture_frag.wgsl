struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightVPMat: mat4x4<f32>,
    cameraPos: vec4<f32>,
    useDirSampling: i32,
    time: f32,
    pad1_: i32,
    pad2_: i32,
}

var<private> fUV_1: vec2<f32>;
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> fViewDir_1: vec3<f32>;
@group(0) @binding(1) 
var texImage: texture_2d<f32>;
@group(0) @binding(2) 
var texSampler: sampler;
var<private> outColor: vec4<f32>;
var<private> fWolrdNormal_1: vec3<f32>;

fn main_1() {
    var col: vec3<f32>;
    var st: vec2<f32>;
    var pi: f32;
    var theta: f32;
    var phi: f32;
    var local: f32;

    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e27 = fUV_1;
    st = _e27;
    let _e29 = ubo.useDirSampling;
    if (_e29 != 0) {
        pi = 3.1414999961853027;
        let _e32 = fViewDir_1[1u];
        theta = acos(_e32);
        let _e35 = fViewDir_1[2u];
        let _e37 = fViewDir_1[0u];
        phi = atan2(_e35, _e37);
        let _e39 = phi;
        if (_e39 < 0.0) {
            let _e41 = phi;
            let _e42 = pi;
            local = (_e41 + (2.0 * _e42));
        } else {
            let _e45 = phi;
            local = _e45;
        }
        let _e46 = local;
        phi = _e46;
        let _e47 = phi;
        let _e48 = pi;
        let _e51 = theta;
        let _e52 = pi;
        st = vec2<f32>((_e47 / (2.0 * _e48)), (_e51 / _e52));
    }
    let _e55 = st;
    let _e56 = textureSample(texImage, texSampler, _e55);
    col = _e56.xyz;
    let _e58 = col;
    outColor = vec4<f32>(_e58.x, _e58.y, _e58.z, 1.0);
    return;
}

@fragment 
fn main(@location(1) fUV: vec2<f32>, @location(2) fViewDir: vec3<f32>, @location(0) fWolrdNormal: vec3<f32>) -> @location(0) vec4<f32> {
    fUV_1 = fUV;
    fViewDir_1 = fViewDir;
    fWolrdNormal_1 = fWolrdNormal;
    main_1();
    let _e7 = outColor;
    return _e7;
}
