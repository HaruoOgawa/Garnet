struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightVPMat: mat4x4<f32>,
    lightDir: vec4<f32>,
    lightColor: vec4<f32>,
    cameraPos: vec4<f32>,
    diffuseFactor: vec4<f32>,
    useSkinMeshAnimation: i32,
    JointIndexOffset: i32,
    UseMainTexture: i32,
    UseToonTexture: i32,
}

@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
@group(0) @binding(2) 
var MainTexture: texture_2d<f32>;
@group(0) @binding(3) 
var MainTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
var<private> f_WorldNormal_1: vec3<f32>;
@group(0) @binding(4) 
var ToonTexture: texture_2d<f32>;
@group(0) @binding(5) 
var ToonTextureSampler: sampler;
var<private> outColor: vec4<f32>;
var<private> f_WorldPos_1: vec4<f32>;
var<private> f_WorldTangent_1: vec3<f32>;
var<private> f_WorldBioTangent_1: vec3<f32>;
var<private> f_LightSpacePos_1: vec4<f32>;

fn main_1() {
    var col: vec3<f32>;
    var alpha: f32;
    var diffuseColor: vec4<f32>;
    var MainColor: vec4<f32>;
    var NdotL: f32;
    var ToonFactor: f32;

    col = vec3<f32>(1.0, 1.0, 1.0);
    alpha = 1.0;
    let _e34 = ubo.diffuseFactor;
    diffuseColor = _e34;
    let _e36 = ubo.UseMainTexture;
    if (_e36 != 0) {
        let _e38 = f_Texcoord_1;
        let _e39 = textureSample(MainTexture, MainTextureSampler, _e38);
        MainColor = _e39;
        let _e40 = MainColor;
        let _e42 = diffuseColor;
        let _e44 = (_e42.xyz * _e40.xyz);
        diffuseColor[0u] = _e44.x;
        diffuseColor[1u] = _e44.y;
        diffuseColor[2u] = _e44.z;
    }
    let _e51 = f_WorldNormal_1;
    let _e53 = ubo.lightDir;
    NdotL = max(0.0, dot(_e51, _e53.xyz));
    let _e58 = ubo.UseToonTexture;
    if (_e58 != 0) {
        let _e60 = f_Texcoord_1;
        let _e61 = textureSample(ToonTexture, ToonTextureSampler, _e60);
        ToonFactor = _e61.x;
    }
    let _e63 = diffuseColor;
    col = _e63.xyz;
    let _e65 = col;
    let _e66 = alpha;
    outColor = vec4<f32>(_e65.x, _e65.y, _e65.z, _e66);
    return;
}

@fragment 
fn main(@location(1) f_Texcoord: vec2<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(2) f_WorldPos: vec4<f32>, @location(3) f_WorldTangent: vec3<f32>, @location(4) f_WorldBioTangent: vec3<f32>, @location(5) f_LightSpacePos: vec4<f32>) -> @location(0) vec4<f32> {
    f_Texcoord_1 = f_Texcoord;
    f_WorldNormal_1 = f_WorldNormal;
    f_WorldPos_1 = f_WorldPos;
    f_WorldTangent_1 = f_WorldTangent;
    f_WorldBioTangent_1 = f_WorldBioTangent;
    f_LightSpacePos_1 = f_LightSpacePos;
    main_1();
    let _e13 = outColor;
    return _e13;
}
