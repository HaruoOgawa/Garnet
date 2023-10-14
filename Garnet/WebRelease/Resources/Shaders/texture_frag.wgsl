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

    col = vec3<f32>(0.0, 0.0, 0.0);
    let _e26 = fUV_1;
    st = _e26;
    let _e28 = ubo.useDirSampling;
    if (_e28 != 0) {
        pi = 3.1414999961853027;
        let _e31 = fViewDir_1[1u];
        theta = acos(_e31);
        let _e34 = fViewDir_1[2u];
        let _e36 = fViewDir_1[0u];
        phi = atan2(_e34, _e36);
        let _e38 = phi;
        let _e39 = pi;
        let _e42 = theta;
        let _e43 = pi;
        st = vec2<f32>((_e38 / (2.0 * _e39)), (_e42 / _e43));
    }
    let _e46 = st;
    let _e47 = textureSample(texImage, texSampler, _e46);
    col = _e47.xyz;
    let _e49 = col;
    outColor = vec4<f32>(_e49.x, _e49.y, _e49.z, 1.0);
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
