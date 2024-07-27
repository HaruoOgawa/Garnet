@group(0) @binding(1) 
var MainTexture: texture_2d<f32>;
@group(0) @binding(2) 
var MainTextureSampler: sampler;
var<private> fUV_1: vec2<f32>;
var<private> outCol: vec4<f32>;
var<private> fWolrdNormal_1: vec3<f32>;
var<private> fViewDir_1: vec3<f32>;

fn main_1() {
    var col: vec4<f32>;
    var dist: f32;
    var t: f32;
    var alpha: f32;

    col = vec4<f32>(0.0, 0.0, 0.0, 1.0);
    let _e24 = fUV_1[0u];
    let _e26 = fUV_1[1u];
    let _e29 = textureSample(MainTexture, MainTextureSampler, vec2<f32>(_e24, (1.0 - _e26)));
    dist = _e29.x;
    t = 0.5;
    let _e31 = t;
    let _e33 = t;
    let _e35 = dist;
    alpha = smoothstep((_e31 - 0.009999999776482582), (_e33 + 0.009999999776482582), _e35);
    let _e37 = alpha;
    if (_e37 > 0.5) {
        col[0u] = vec3<f32>(1.0, 1.0, 1.0).x;
        col[1u] = vec3<f32>(1.0, 1.0, 1.0).y;
        col[2u] = vec3<f32>(1.0, 1.0, 1.0).z;
    } else {
        discard;
    }
    let _e45 = col;
    outCol = _e45;
    return;
}

@fragment 
fn main(@location(1) fUV: vec2<f32>, @location(0) fWolrdNormal: vec3<f32>, @location(2) fViewDir: vec3<f32>) -> @location(0) vec4<f32> {
    fUV_1 = fUV;
    fWolrdNormal_1 = fWolrdNormal;
    fViewDir_1 = fViewDir;
    main_1();
    let _e7 = outCol;
    return _e7;
}
