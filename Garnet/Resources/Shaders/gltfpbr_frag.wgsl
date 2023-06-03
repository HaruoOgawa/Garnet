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

fn CalcDiffusestructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e48 = (*param).diffuseColor;
    return (_e48 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e48 = (*param_1).reflectance0_;
    let _e50 = (*param_1).reflectance90_;
    let _e52 = (*param_1).reflectance0_;
    let _e55 = (*param_1).VdotH;
    return (_e48 + ((_e50 - _e52) * pow(clamp((1.0 - _e55), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e53 = (*param_2).NdotL;
    NdotL = _e53;
    let _e55 = (*param_2).NdotV;
    NdotV = _e55;
    let _e57 = (*param_2).alphaRoughness;
    r = _e57;
    let _e58 = NdotL;
    let _e60 = NdotL;
    let _e61 = r;
    let _e62 = r;
    let _e64 = r;
    let _e65 = r;
    let _e68 = NdotL;
    let _e69 = NdotL;
    attenuationL = ((2.0 * _e58) / (_e60 + sqrt(((_e61 * _e62) + ((1.0 - (_e64 * _e65)) * (_e68 * _e69))))));
    let _e76 = NdotV;
    let _e78 = NdotV;
    let _e79 = r;
    let _e80 = r;
    let _e82 = r;
    let _e83 = r;
    let _e86 = NdotV;
    let _e87 = NdotV;
    attenuationV = ((2.0 * _e76) / (_e78 + sqrt(((_e79 * _e80) + ((1.0 - (_e82 * _e83)) * (_e86 * _e87))))));
    let _e94 = attenuationL;
    let _e95 = attenuationV;
    return (_e94 * _e95);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e50 = (*param_3).alphaRoughness;
    let _e52 = (*param_3).alphaRoughness;
    roughness2_ = (_e50 * _e52);
    let _e55 = (*param_3).NdotH;
    let _e56 = roughness2_;
    let _e59 = (*param_3).NdotH;
    let _e62 = (*param_3).NdotH;
    f = ((((_e55 * _e56) - _e59) * _e62) + 1.0);
    let _e65 = roughness2_;
    let _e66 = f;
    let _e68 = f;
    return (_e65 / ((3.1415927410125732 * _e66) * _e68));
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
    var specBRDF: vec3<f32>;
    var diffuseBRDF: vec3<f32>;
    var param_7: PBRParam;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e80 = ubo.roughnessFactor;
    perceptualRoughness = _e80;
    let _e82 = ubo.metallicFactor;
    metallic = _e82;
    let _e83 = f_Texcoord_1;
    let _e84 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e83);
    metallicRoughnessColor = _e84;
    let _e85 = perceptualRoughness;
    let _e87 = metallicRoughnessColor[1u];
    perceptualRoughness = (_e85 * _e87);
    let _e89 = metallic;
    let _e91 = metallicRoughnessColor[2u];
    metallic = (_e89 * _e91);
    let _e93 = perceptualRoughness;
    perceptualRoughness = clamp(_e93, 0.03999999910593033, 1.0);
    let _e95 = metallic;
    metallic = clamp(_e95, 0.0, 1.0);
    let _e97 = perceptualRoughness;
    let _e98 = perceptualRoughness;
    alphaRoughness = (_e97 * _e98);
    let _e100 = f_Texcoord_1;
    let _e101 = textureSample(baseColorTexture, baseColorTextureSampler, _e100);
    baseColor = _e101;
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e102 = baseColor;
    let _e104 = f0_;
    diffuseColor = (_e102.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e104));
    let _e107 = metallic;
    let _e109 = diffuseColor;
    diffuseColor = (_e109 * (1.0 - _e107));
    let _e111 = f0_;
    let _e112 = baseColor;
    let _e114 = metallic;
    specularColor = mix(_e111, _e112.xyz, vec3<f32>(_e114));
    let _e118 = specularColor[0u];
    let _e120 = specularColor[1u];
    let _e123 = specularColor[2u];
    reflectance = max(max(_e118, _e120), _e123);
    let _e125 = reflectance;
    reflectance90_ = clamp((_e125 * 25.0), 0.0, 1.0);
    let _e128 = specularColor;
    specularEnvironmentR0_ = _e128;
    let _e129 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e129);
    let _e131 = f_WorldNormal_1;
    n = _e131;
    let _e133 = ubo.cameraPos;
    let _e135 = f_WorldPos_1;
    v = normalize((_e133.xyz - _e135.xyz));
    let _e140 = ubo.lightDir;
    l = normalize(_e140.xyz);
    let _e143 = v;
    let _e144 = l;
    h = normalize((_e143 + _e144));
    let _e147 = v;
    let _e148 = n;
    reflection = -(normalize(reflect(_e147, _e148)));
    let _e152 = n;
    let _e153 = l;
    NdotL_1 = clamp(dot(_e152, _e153), 0.0010000000474974513, 1.0);
    let _e156 = n;
    let _e157 = v;
    NdotV_1 = clamp(abs(dot(_e156, _e157)), 0.0010000000474974513, 1.0);
    let _e161 = n;
    let _e162 = h;
    NdotH = clamp(dot(_e161, _e162), 0.0, 1.0);
    let _e165 = l;
    let _e166 = h;
    LdotH = clamp(dot(_e165, _e166), 0.0, 1.0);
    let _e169 = v;
    let _e170 = h;
    VdotH = clamp(dot(_e169, _e170), 0.0, 1.0);
    let _e173 = NdotL_1;
    let _e174 = NdotV_1;
    let _e175 = NdotH;
    let _e176 = LdotH;
    let _e177 = VdotH;
    let _e178 = perceptualRoughness;
    let _e179 = metallic;
    let _e180 = specularEnvironmentR0_;
    let _e181 = specularEnvironmentR90_;
    let _e182 = alphaRoughness;
    let _e183 = diffuseColor;
    let _e184 = specularColor;
    pbrParam = PBRParam(_e173, _e174, _e175, _e176, _e177, _e178, _e179, _e180, _e181, _e182, _e183, _e184);
    let _e186 = pbrParam;
    param_4 = _e186;
    let _e187 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e187;
    let _e188 = pbrParam;
    param_5 = _e188;
    let _e189 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e189;
    let _e190 = pbrParam;
    param_6 = _e190;
    let _e191 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e191;
    let _e192 = D;
    let _e193 = G;
    let _e195 = F;
    let _e197 = NdotL_1;
    let _e199 = NdotV_1;
    specBRDF = ((_e195 * (_e192 * _e193)) / vec3<f32>(((4.0 * _e197) * _e199)));
    let _e203 = F;
    let _e206 = pbrParam;
    param_7 = _e206;
    let _e207 = CalcDiffusestructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e203) * _e207);
    let _e209 = NdotL_1;
    let _e211 = specBRDF;
    let _e212 = diffuseBRDF;
    let _e214 = ((vec3<f32>(1.0, 1.0, 1.0) * _e209) * (_e211 + _e212));
    col[0u] = _e214.x;
    col[1u] = _e214.y;
    col[2u] = _e214.z;
    let _e221 = col;
    let _e223 = pow(_e221.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e223.x;
    col[1u] = _e223.y;
    col[2u] = _e223.z;
    let _e231 = baseColor[3u];
    col[3u] = _e231;
    let _e233 = col;
    outColor = _e233;
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
