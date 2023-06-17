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
    normalMapScale: f32,
    useBaseColorTexture: i32,
    useMetallicRoughnessTexture: i32,
    useEmissiveTexture: i32,
    useNormalTexture: i32,
    useOcclusionTexture: i32,
    t_pad_0_: i32,
    t_pad_1_: i32,
    t_pad_2_: i32,
}

@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> f_WorldTangent_1: vec4<f32>;
var<private> f_WorldNormal_1: vec3<f32>;
@group(0) @binding(7) 
var normalTexture: texture_2d<f32>;
@group(0) @binding(8) 
var normalTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
@group(0) @binding(3) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(4) 
var metallicRoughnessTextureSampler: sampler;
@group(0) @binding(1) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(2) 
var baseColorTextureSampler: sampler;
var<private> f_WorldPos_1: vec4<f32>;
var<private> outColor: vec4<f32>;
var<private> f_WorldBioTangent_1: vec4<f32>;
@group(0) @binding(5) 
var emissiveTexture: texture_2d<f32>;
@group(0) @binding(6) 
var emissiveTextureSampler: sampler;
@group(0) @binding(9) 
var occlusionTexture: texture_2d<f32>;
@group(0) @binding(10) 
var occlusionTextureSampler: sampler;

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e57 = (*param).diffuseColor;
    return (_e57 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e57 = (*param_1).reflectance0_;
    let _e59 = (*param_1).reflectance90_;
    let _e61 = (*param_1).reflectance0_;
    let _e64 = (*param_1).VdotH;
    return (_e57 + ((_e59 - _e61) * pow(clamp((1.0 - _e64), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e62 = (*param_2).NdotL;
    NdotL = _e62;
    let _e64 = (*param_2).NdotV;
    NdotV = _e64;
    let _e66 = (*param_2).alphaRoughness;
    r = _e66;
    let _e67 = NdotL;
    let _e69 = NdotL;
    let _e70 = r;
    let _e71 = r;
    let _e73 = r;
    let _e74 = r;
    let _e77 = NdotL;
    let _e78 = NdotL;
    attenuationL = ((2.0 * _e67) / (_e69 + sqrt(((_e70 * _e71) + ((1.0 - (_e73 * _e74)) * (_e77 * _e78))))));
    let _e85 = NdotV;
    let _e87 = NdotV;
    let _e88 = r;
    let _e89 = r;
    let _e91 = r;
    let _e92 = r;
    let _e95 = NdotV;
    let _e96 = NdotV;
    attenuationV = ((2.0 * _e85) / (_e87 + sqrt(((_e88 * _e89) + ((1.0 - (_e91 * _e92)) * (_e95 * _e96))))));
    let _e103 = attenuationL;
    let _e104 = attenuationV;
    return (_e103 * _e104);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e59 = (*param_3).alphaRoughness;
    let _e61 = (*param_3).alphaRoughness;
    roughness2_ = (_e59 * _e61);
    let _e64 = (*param_3).NdotH;
    let _e65 = roughness2_;
    let _e68 = (*param_3).NdotH;
    let _e71 = (*param_3).NdotH;
    f = ((((_e64 * _e65) - _e68) * _e71) + 1.0);
    let _e74 = roughness2_;
    let _e75 = f;
    let _e77 = f;
    return (_e74 / ((3.1415927410125732 * _e75) * _e77));
}

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e61 = ubo.useNormalTexture;
    if (_e61 != 0) {
        let _e63 = f_WorldTangent_1;
        t = normalize(_e63.xyz);
        let _e66 = f_WorldTangent_1;
        b = normalize(_e66.xyz);
        let _e69 = f_WorldNormal_1;
        n = normalize(_e69);
        let _e71 = t;
        let _e72 = b;
        let _e73 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e71.x, _e71.y, _e71.z), vec3<f32>(_e72.x, _e72.y, _e72.z), vec3<f32>(_e73.x, _e73.y, _e73.z));
        let _e87 = f_Texcoord_1;
        let _e88 = textureSample(normalTexture, normalTextureSampler, _e87);
        nomral = _e88.xyz;
        let _e90 = tbn;
        let _e91 = nomral;
        let _e96 = ubo.normalMapScale;
        let _e98 = ubo.normalMapScale;
        nomral = normalize((_e90 * (((_e91 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e96, _e98, 1.0))));
    } else {
        let _e103 = f_WorldNormal_1;
        nomral = _e103;
    }
    let _e104 = nomral;
    return _e104;
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
    var n_1: vec3<f32>;
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
    let _e89 = ubo.roughnessFactor;
    perceptualRoughness = _e89;
    let _e91 = ubo.metallicFactor;
    metallic = _e91;
    let _e93 = ubo.useMetallicRoughnessTexture;
    if (_e93 != 0) {
        let _e95 = f_Texcoord_1;
        let _e96 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e95);
        metallicRoughnessColor = _e96;
        let _e97 = perceptualRoughness;
        let _e99 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e97 * _e99);
        let _e101 = metallic;
        let _e103 = metallicRoughnessColor[2u];
        metallic = (_e101 * _e103);
    }
    let _e105 = perceptualRoughness;
    perceptualRoughness = clamp(_e105, 0.03999999910593033, 1.0);
    let _e107 = metallic;
    metallic = clamp(_e107, 0.0, 1.0);
    let _e109 = perceptualRoughness;
    let _e110 = perceptualRoughness;
    alphaRoughness = (_e109 * _e110);
    let _e113 = ubo.useBaseColorTexture;
    if (_e113 != 0) {
        let _e115 = f_Texcoord_1;
        let _e116 = textureSample(baseColorTexture, baseColorTextureSampler, _e115);
        baseColor = _e116;
    } else {
        let _e118 = ubo.baseColorFactor;
        baseColor = _e118;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e119 = baseColor;
    let _e121 = f0_;
    diffuseColor = (_e119.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e121));
    let _e124 = metallic;
    let _e126 = diffuseColor;
    diffuseColor = (_e126 * (1.0 - _e124));
    let _e128 = f0_;
    let _e129 = baseColor;
    let _e131 = metallic;
    specularColor = mix(_e128, _e129.xyz, vec3<f32>(_e131));
    let _e135 = specularColor[0u];
    let _e137 = specularColor[1u];
    let _e140 = specularColor[2u];
    reflectance = max(max(_e135, _e137), _e140);
    let _e142 = reflectance;
    reflectance90_ = clamp((_e142 * 25.0), 0.0, 1.0);
    let _e145 = specularColor;
    specularEnvironmentR0_ = _e145;
    let _e146 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e146);
    let _e148 = getNormal();
    n_1 = _e148;
    let _e150 = ubo.cameraPos;
    let _e152 = f_WorldPos_1;
    v = normalize((_e150.xyz - _e152.xyz));
    let _e157 = ubo.lightDir;
    l = normalize(_e157.xyz);
    let _e160 = v;
    let _e161 = l;
    h = normalize((_e160 + _e161));
    let _e164 = v;
    let _e165 = n_1;
    reflection = -(normalize(reflect(_e164, _e165)));
    let _e169 = n_1;
    let _e170 = l;
    NdotL_1 = clamp(dot(_e169, _e170), 0.0010000000474974513, 1.0);
    let _e173 = n_1;
    let _e174 = v;
    NdotV_1 = clamp(abs(dot(_e173, _e174)), 0.0010000000474974513, 1.0);
    let _e178 = n_1;
    let _e179 = h;
    NdotH = clamp(dot(_e178, _e179), 0.0, 1.0);
    let _e182 = l;
    let _e183 = h;
    LdotH = clamp(dot(_e182, _e183), 0.0, 1.0);
    let _e186 = v;
    let _e187 = h;
    VdotH = clamp(dot(_e186, _e187), 0.0, 1.0);
    let _e190 = NdotL_1;
    let _e191 = NdotV_1;
    let _e192 = NdotH;
    let _e193 = LdotH;
    let _e194 = VdotH;
    let _e195 = perceptualRoughness;
    let _e196 = metallic;
    let _e197 = specularEnvironmentR0_;
    let _e198 = specularEnvironmentR90_;
    let _e199 = alphaRoughness;
    let _e200 = diffuseColor;
    let _e201 = specularColor;
    pbrParam = PBRParam(_e190, _e191, _e192, _e193, _e194, _e195, _e196, _e197, _e198, _e199, _e200, _e201);
    let _e203 = pbrParam;
    param_4 = _e203;
    let _e204 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e204;
    let _e205 = pbrParam;
    param_5 = _e205;
    let _e206 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e206;
    let _e207 = pbrParam;
    param_6 = _e207;
    let _e208 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e208;
    let _e209 = D;
    let _e210 = G;
    let _e212 = F;
    let _e214 = NdotL_1;
    let _e216 = NdotV_1;
    specularBRDF = ((_e212 * (_e209 * _e210)) / vec3<f32>(((4.0 * _e214) * _e216)));
    let _e220 = F;
    let _e223 = pbrParam;
    param_7 = _e223;
    let _e224 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e220) * _e224);
    let _e226 = NdotL_1;
    let _e228 = ubo.lightColor;
    let _e231 = specularBRDF;
    let _e232 = diffuseBRDF;
    let _e234 = ((_e228.xyz * _e226) * (_e231 + _e232));
    col[0u] = _e234.x;
    col[1u] = _e234.y;
    col[2u] = _e234.z;
    let _e241 = col;
    let _e243 = pow(_e241.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e243.x;
    col[1u] = _e243.y;
    col[2u] = _e243.z;
    let _e251 = baseColor[3u];
    col[3u] = _e251;
    let _e253 = col;
    outColor = _e253;
    return;
}

@fragment 
fn main(@location(3) f_WorldTangent: vec4<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(1) f_Texcoord: vec2<f32>, @location(2) f_WorldPos: vec4<f32>, @location(4) f_WorldBioTangent: vec4<f32>) -> @location(0) vec4<f32> {
    f_WorldTangent_1 = f_WorldTangent;
    f_WorldNormal_1 = f_WorldNormal;
    f_Texcoord_1 = f_Texcoord;
    f_WorldPos_1 = f_WorldPos;
    f_WorldBioTangent_1 = f_WorldBioTangent;
    main_1();
    let _e11 = outColor;
    return _e11;
}
