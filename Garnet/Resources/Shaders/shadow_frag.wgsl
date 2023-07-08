@group(0) @binding(1) 
var depthMap: texture_2d<f32>;
@group(0) @binding(2) 
var depthSampler: sampler;
var<private> fragTexCoord_1: vec2<f32>;
var<private> outColor: vec4<f32>;

fn main_1() {
    var depth: f32;

    let _e11 = fragTexCoord_1;
    let _e12 = textureSample(depthMap, depthSampler, _e11);
    depth = _e12.x;
    let _e14 = depth;
    let _e15 = vec3<f32>(_e14);
    outColor = vec4<f32>(_e15.x, _e15.y, _e15.z, 1.0);
    return;
}

@fragment 
fn main(@location(0) fragTexCoord: vec2<f32>) -> @location(0) vec4<f32> {
    fragTexCoord_1 = fragTexCoord;
    main_1();
    let _e3 = outColor;
    return _e3;
}
