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
    diffuseColor: vec3<f32>,
    specularColor: vec3<f32>,
}

struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    lightDir: vec4<f32>,
    lightColor: vec4<f32>,
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
var<private> outColor: vec4<f32>;
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

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e49 = (*param).diffuseColor;
    return (_e49 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e49 = (*param_1).reflectance0_;
    let _e51 = (*param_1).reflectance90_;
    let _e53 = (*param_1).reflectance0_;
    let _e56 = (*param_1).VdotH;
    return (_e49 + ((_e51 - _e53) * pow(clamp((1.0 - _e56), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e54 = (*param_2).NdotL;
    NdotL = _e54;
    let _e56 = (*param_2).NdotV;
    NdotV = _e56;
    let _e58 = (*param_2).alphaRoughness;
    r = _e58;
    let _e59 = NdotL;
    let _e61 = NdotL;
    let _e62 = r;
    let _e63 = r;
    let _e65 = r;
    let _e66 = r;
    let _e69 = NdotL;
    let _e70 = NdotL;
    attenuationL = ((2.0 * _e59) / (_e61 + sqrt(((_e62 * _e63) + ((1.0 - (_e65 * _e66)) * (_e69 * _e70))))));
    let _e77 = NdotV;
    let _e79 = NdotV;
    let _e80 = r;
    let _e81 = r;
    let _e83 = r;
    let _e84 = r;
    let _e87 = NdotV;
    let _e88 = NdotV;
    attenuationV = ((2.0 * _e77) / (_e79 + sqrt(((_e80 * _e81) + ((1.0 - (_e83 * _e84)) * (_e87 * _e88))))));
    let _e95 = attenuationL;
    let _e96 = attenuationV;
    return (_e95 * _e96);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e51 = (*param_3).alphaRoughness;
    let _e53 = (*param_3).alphaRoughness;
    roughness2_ = (_e51 * _e53);
    let _e56 = (*param_3).NdotH;
    let _e57 = roughness2_;
    let _e60 = (*param_3).NdotH;
    let _e63 = (*param_3).NdotH;
    f = ((((_e56 * _e57) - _e60) * _e63) + 1.0);
    let _e66 = roughness2_;
    let _e67 = f;
    let _e69 = f;
    return (_e66 / ((3.1415927410125732 * _e67) * _e69));
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
    var NdotL_1: f32;
    var NdotV_1: f32;
    var NdotH: f32;
    var LdotH: f32;
    var VdotH: f32;
    var pbrParam: PBRParam;
    var D: f32;
    var param_4: PBRParam;
    var G: f32;
    var param_5: PBRParam;
    var F: vec3<f32>;
    var param_6: PBRParam;
    var specularBRDF: vec3<f32>;
    var diffuseBRDF: vec3<f32>;
    var param_7: PBRParam;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e81 = ubo.roughnessFactor;
    perceptualRoughness = _e81;
    let _e83 = ubo.metallicFactor;
    metallic = _e83;
    let _e84 = f_Texcoord_1;
    let _e85 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e84);
    metallicRoughnessColor = _e85;
    let _e86 = perceptualRoughness;
    let _e88 = metallicRoughnessColor[1u];
    perceptualRoughness = (_e86 * _e88);
    let _e90 = metallic;
    let _e92 = metallicRoughnessColor[2u];
    metallic = (_e90 * _e92);
    let _e94 = perceptualRoughness;
    perceptualRoughness = clamp(_e94, 0.03999999910593033, 1.0);
    let _e96 = metallic;
    metallic = clamp(_e96, 0.0, 1.0);
    let _e98 = perceptualRoughness;
    let _e99 = perceptualRoughness;
    alphaRoughness = (_e98 * _e99);
    let _e101 = f_Texcoord_1;
    let _e102 = textureSample(baseColorTexture, baseColorTextureSampler, _e101);
    baseColor = _e102;
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e103 = baseColor;
    let _e105 = f0_;
    diffuseColor = (_e103.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e105));
    let _e108 = metallic;
    let _e110 = diffuseColor;
    diffuseColor = (_e110 * (1.0 - _e108));
    let _e112 = f0_;
    let _e113 = baseColor;
    let _e115 = metallic;
    specularColor = mix(_e112, _e113.xyz, vec3<f32>(_e115));
    let _e119 = specularColor[0u];
    let _e121 = specularColor[1u];
    let _e124 = specularColor[2u];
    reflectance = max(max(_e119, _e121), _e124);
    let _e126 = reflectance;
    reflectance90_ = clamp((_e126 * 25.0), 0.0, 1.0);
    let _e129 = specularColor;
    specularEnvironmentR0_ = _e129;
    let _e130 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e130);
    let _e132 = f_WorldNormal_1;
    n = _e132;
    let _e134 = ubo.cameraPos;
    let _e136 = f_WorldPos_1;
    v = normalize((_e134.xyz - _e136.xyz));
    let _e141 = ubo.lightDir;
    l = normalize(_e141.xyz);
    let _e144 = v;
    let _e145 = l;
    h = normalize((_e144 + _e145));
    let _e148 = v;
    let _e149 = n;
    reflection = -(normalize(reflect(_e148, _e149)));
    let _e153 = n;
    let _e154 = l;
    NdotL_1 = clamp(dot(_e153, _e154), 0.0010000000474974513, 1.0);
    let _e157 = n;
    let _e158 = v;
    NdotV_1 = clamp(abs(dot(_e157, _e158)), 0.0010000000474974513, 1.0);
    let _e162 = n;
    let _e163 = h;
    NdotH = clamp(dot(_e162, _e163), 0.0, 1.0);
    let _e166 = l;
    let _e167 = h;
    LdotH = clamp(dot(_e166, _e167), 0.0, 1.0);
    let _e170 = v;
    let _e171 = h;
    VdotH = clamp(dot(_e170, _e171), 0.0, 1.0);
    let _e174 = NdotL_1;
    let _e175 = NdotV_1;
    let _e176 = NdotH;
    let _e177 = LdotH;
    let _e178 = VdotH;
    let _e179 = perceptualRoughness;
    let _e180 = metallic;
    let _e181 = specularEnvironmentR0_;
    let _e182 = specularEnvironmentR90_;
    let _e183 = alphaRoughness;
    let _e184 = diffuseColor;
    let _e185 = specularColor;
    pbrParam = PBRParam(_e174, _e175, _e176, _e177, _e178, _e179, _e180, _e181, _e182, _e183, _e184, _e185);
    let _e187 = pbrParam;
    param_4 = _e187;
    let _e188 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e188;
    let _e189 = pbrParam;
    param_5 = _e189;
    let _e190 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e190;
    let _e191 = pbrParam;
    param_6 = _e191;
    let _e192 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e192;
    let _e193 = D;
    let _e194 = G;
    let _e196 = F;
    let _e198 = NdotL_1;
    let _e200 = NdotV_1;
    specularBRDF = ((_e196 * (_e193 * _e194)) / vec3<f32>(((4.0 * _e198) * _e200)));
    let _e204 = F;
    let _e207 = pbrParam;
    param_7 = _e207;
    let _e208 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e204) * _e208);
    let _e210 = NdotL_1;
    let _e212 = ubo.lightColor;
    let _e215 = specularBRDF;
    let _e216 = diffuseBRDF;
    let _e218 = ((_e212.xyz * _e210) * (_e215 + _e216));
    col[0u] = _e218.x;
    col[1u] = _e218.y;
    col[2u] = _e218.z;
    let _e225 = col;
    let _e227 = pow(_e225.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e227.x;
    col[1u] = _e227.y;
    col[2u] = _e227.z;
    let _e235 = baseColor[3u];
    col[3u] = _e235;
    let _e237 = col;
    outColor = _e237;
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
