struct PBRParam {
    NdotL: f32,
    NdotV: f32,
    NdotH: f32,
    LdotH: f32,
    VdotH: f32,
    perceptualRoughness: f32,
    metallic: f32,
    reflectance0_: vec3<f32>,
    reflectance90_: vec3<f32>,
    alphaRoughness: f32,
    duffuseColor: vec3<f32>,
    specularColor: vec3<f32>,
}

struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightDir: vec4<f32>,
    cameraPos: vec4<f32>,
    baseColorFactor: vec4<f32>,
    emissiveFactor: vec4<f32>,
    time: f32,
    metallicFactor: f32,
    roughnessFactor: f32,
    padding2_: f32,
}

@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
@group(0) @binding(3) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(4) 
var metallicRoughnessTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
@group(0) @binding(1) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(2) 
var baseColorTextureSampler: sampler;
var<private> f_WorldNormal_1: vec3<f32>;
var<private> f_WorldPos_1: vec4<f32>;
@group(0) @binding(5) 
var emissiveTexture: texture_2d<f32>;
@group(0) @binding(6) 
var emissiveTextureSampler: sampler;
@group(0) @binding(7) 
var normalTexture: texture_2d<f32>;
@group(0) @binding(8) 
var normalTextureSampler: sampler;
@group(0) @binding(9) 
var occlusionTexture: texture_2d<f32>;
@group(0) @binding(10) 
var occlusionTextureSampler: sampler;
var<private> outColor: vec4<f32>;

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e38 = (*param).reflectance0_;
    let _e40 = (*param).reflectance90_;
    let _e42 = (*param).reflectance0_;
    let _e45 = (*param).VdotH;
    return (_e38 + ((_e40 - _e42) * pow(clamp((1.0 - _e45), 0.0, 1.0), 5.0)));
}

fn main_1() {
    var col: vec4<f32>;
    var perceptualRoughness: f32;
    var metallic: f32;
    var metallicRoughnessColor: vec4<f32>;
    var alphaRoughness: f32;
    var baseColor: vec4<f32>;
    var f0_: vec3<f32>;
    var diffuseColor: vec3<f32>;
    var specularColor: vec3<f32>;
    var reflectance: f32;
    var reflectance90_: f32;
    var specularEnvironmentR0_: vec3<f32>;
    var specularEnvironmentR90_: vec3<f32>;
    var n: vec3<f32>;
    var v: vec3<f32>;
    var l: vec3<f32>;
    var h: vec3<f32>;
    var reflection: vec3<f32>;
    var NdotL: f32;
    var NdotV: f32;
    var NdotH: f32;
    var LdotH: f32;
    var VdotH: f32;
    var pbrParam: PBRParam;
    var F: vec3<f32>;
    var param_1: PBRParam;
    var emissiveColor: vec4<f32>;
    var normalColor: vec4<f32>;
    var occlusionColor: vec4<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e66 = ubo.roughnessFactor;
    perceptualRoughness = _e66;
    let _e68 = ubo.metallicFactor;
    metallic = _e68;
    let _e69 = f_Texcoord_1;
    let _e70 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e69);
    metallicRoughnessColor = _e70;
    let _e71 = perceptualRoughness;
    let _e73 = metallicRoughnessColor[1u];
    perceptualRoughness = (_e71 * _e73);
    let _e75 = metallic;
    let _e77 = metallicRoughnessColor[2u];
    metallic = (_e75 * _e77);
    let _e79 = perceptualRoughness;
    perceptualRoughness = clamp(_e79, 0.03999999910593033, 1.0);
    let _e81 = metallic;
    metallic = clamp(_e81, 0.0, 1.0);
    let _e83 = perceptualRoughness;
    let _e84 = perceptualRoughness;
    alphaRoughness = (_e83 * _e84);
    let _e86 = f_Texcoord_1;
    let _e87 = textureSample(baseColorTexture, baseColorTextureSampler, _e86);
    baseColor = _e87;
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e88 = baseColor;
    let _e90 = f0_;
    diffuseColor = (_e88.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e90));
    let _e93 = metallic;
    let _e95 = diffuseColor;
    diffuseColor = (_e95 * (1.0 - _e93));
    let _e97 = f0_;
    let _e98 = baseColor;
    let _e100 = metallic;
    specularColor = mix(_e97, _e98.xyz, vec3<f32>(_e100));
    let _e104 = specularColor[0u];
    let _e106 = specularColor[1u];
    let _e109 = specularColor[2u];
    reflectance = max(max(_e104, _e106), _e109);
    let _e111 = reflectance;
    reflectance90_ = clamp((_e111 * 25.0), 0.0, 1.0);
    let _e114 = specularColor;
    specularEnvironmentR0_ = _e114;
    let _e115 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e115);
    let _e117 = f_WorldNormal_1;
    n = _e117;
    let _e119 = ubo.cameraPos;
    let _e121 = f_WorldPos_1;
    v = normalize((_e119.xyz - _e121.xyz));
    let _e126 = ubo.lightDir;
    l = normalize(_e126.xyz);
    let _e129 = v;
    let _e130 = l;
    h = normalize((_e129 + _e130));
    let _e133 = v;
    let _e134 = n;
    reflection = -(normalize(reflect(_e133, _e134)));
    let _e138 = n;
    let _e139 = l;
    NdotL = clamp(dot(_e138, _e139), 0.0010000000474974513, 1.0);
    let _e142 = n;
    let _e143 = v;
    NdotV = clamp(abs(dot(_e142, _e143)), 0.0010000000474974513, 1.0);
    let _e147 = n;
    let _e148 = h;
    NdotH = clamp(dot(_e147, _e148), 0.0, 1.0);
    let _e151 = l;
    let _e152 = h;
    LdotH = clamp(dot(_e151, _e152), 0.0, 1.0);
    let _e155 = v;
    let _e156 = h;
    VdotH = clamp(dot(_e155, _e156), 0.0, 1.0);
    let _e159 = NdotL;
    let _e160 = NdotV;
    let _e161 = NdotH;
    let _e162 = LdotH;
    let _e163 = VdotH;
    let _e164 = perceptualRoughness;
    let _e165 = metallic;
    let _e166 = specularEnvironmentR0_;
    let _e167 = specularEnvironmentR90_;
    let _e168 = alphaRoughness;
    let _e169 = diffuseColor;
    let _e170 = specularColor;
    pbrParam = PBRParam(_e159, _e160, _e161, _e162, _e163, _e164, _e165, _e166, _e167, _e168, _e169, _e170);
    let _e172 = pbrParam;
    param_1 = _e172;
    let _e173 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_1));
    F = _e173;
    let _e174 = f_Texcoord_1;
    let _e175 = textureSample(emissiveTexture, emissiveTextureSampler, _e174);
    emissiveColor = _e175;
    let _e176 = f_Texcoord_1;
    let _e177 = textureSample(normalTexture, normalTextureSampler, _e176);
    normalColor = _e177;
    let _e178 = f_Texcoord_1;
    let _e179 = textureSample(occlusionTexture, occlusionTextureSampler, _e178);
    occlusionColor = _e179;
    let _e180 = baseColor;
    let _e181 = _e180.xyz;
    col[0u] = _e181.x;
    col[1u] = _e181.y;
    col[2u] = _e181.z;
    let _e188 = col;
    outColor = _e188;
    return;
}

@fragment 
fn main(@location(1) f_Texcoord: vec2<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(2) f_WorldPos: vec4<f32>) -> @location(0) vec4<f32> {
    f_Texcoord_1 = f_Texcoord;
    f_WorldNormal_1 = f_WorldNormal;
    f_WorldPos_1 = f_WorldPos;
    main_1();
    let _e7 = outColor;
    return _e7;
}
