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
    lightVPMat: mat4x4<f32>,
    lightDir: vec4<f32>,
    lightColor: vec4<f32>,
    cameraPos: vec4<f32>,
    baseColorFactor: vec4<f32>,
    emissiveFactor: vec4<f32>,
    time: f32,
    metallicFactor: f32,
    roughnessFactor: f32,
    normalMapScale: f32,
    occlusionStrength: f32,
    mipCount: f32,
    s_pad1_: f32,
    s_pad2_: f32,
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
var<private> f_WorldTangent_1: vec3<f32>;
var<private> f_WorldBioTangent_1: vec3<f32>;
var<private> f_WorldNormal_1: vec3<f32>;
@group(0) @binding(7) 
var normalTexture: texture_2d<f32>;
@group(0) @binding(8) 
var normalTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
@group(0) @binding(13) 
var shadowmapTexture: texture_2d<f32>;
@group(0) @binding(14) 
var shadowmapTextureSampler: sampler;
@group(0) @binding(3) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(4) 
var metallicRoughnessTextureSampler: sampler;
@group(0) @binding(1) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(2) 
var baseColorTextureSampler: sampler;
var<private> f_WorldPos_1: vec4<f32>;
@group(0) @binding(11) 
var cubemapTexture: texture_cube<f32>;
@group(0) @binding(12) 
var cubemapTextureSampler: sampler;
@group(0) @binding(9) 
var occlusionTexture: texture_2d<f32>;
@group(0) @binding(10) 
var occlusionTextureSampler: sampler;
@group(0) @binding(5) 
var emissiveTexture: texture_2d<f32>;
@group(0) @binding(6) 
var emissiveTextureSampler: sampler;
var<private> f_LightSpacePos_1: vec4<f32>;
var<private> outColor: vec4<f32>;

fn CalcShadowvf3_(lsp: ptr<function, vec3<f32>>) -> f32 {
    var moments: vec2<f32>;

    let _e69 = (*lsp);
    let _e71 = textureSample(shadowmapTexture, shadowmapTextureSampler, _e69.xy);
    moments = _e71.xy;
    let _e74 = (*lsp)[2u];
    let _e76 = moments[0u];
    if (_e74 <= _e76) {
        return 1.0;
    } else {
        return 0.0;
    }
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e68 = (*srgbIn);
    let _e70 = pow(_e68.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e72 = (*srgbIn)[3u];
    return vec4<f32>(_e70.x, _e70.y, _e70.z, _e72);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e68 = (*srgbIn_1);
    let _e70 = pow(_e68.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e72 = (*srgbIn_1)[3u];
    return vec4<f32>(_e70.x, _e70.y, _e70.z, _e72);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e69 = (*param).diffuseColor;
    return (_e69 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e69 = (*param_1).reflectance0_;
    let _e71 = (*param_1).reflectance90_;
    let _e73 = (*param_1).reflectance0_;
    let _e76 = (*param_1).VdotH;
    return (_e69 + ((_e71 - _e73) * pow(clamp((1.0 - _e76), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e74 = (*param_2).NdotL;
    NdotL = _e74;
    let _e76 = (*param_2).NdotV;
    NdotV = _e76;
    let _e78 = (*param_2).alphaRoughness;
    r = _e78;
    let _e79 = NdotL;
    let _e81 = NdotL;
    let _e82 = r;
    let _e83 = r;
    let _e85 = r;
    let _e86 = r;
    let _e89 = NdotL;
    let _e90 = NdotL;
    attenuationL = ((2.0 * _e79) / (_e81 + sqrt(((_e82 * _e83) + ((1.0 - (_e85 * _e86)) * (_e89 * _e90))))));
    let _e97 = NdotV;
    let _e99 = NdotV;
    let _e100 = r;
    let _e101 = r;
    let _e103 = r;
    let _e104 = r;
    let _e107 = NdotV;
    let _e108 = NdotV;
    attenuationV = ((2.0 * _e97) / (_e99 + sqrt(((_e100 * _e101) + ((1.0 - (_e103 * _e104)) * (_e107 * _e108))))));
    let _e115 = attenuationL;
    let _e116 = attenuationV;
    return (_e115 * _e116);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e71 = (*param_3).alphaRoughness;
    let _e73 = (*param_3).alphaRoughness;
    roughness2_ = (_e71 * _e73);
    let _e76 = (*param_3).NdotH;
    let _e77 = roughness2_;
    let _e80 = (*param_3).NdotH;
    let _e83 = (*param_3).NdotH;
    f = ((((_e76 * _e77) - _e80) * _e83) + 1.0);
    let _e86 = roughness2_;
    let _e87 = f;
    let _e89 = f;
    return (_e86 / ((3.1415927410125732 * _e87) * _e89));
}

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e73 = ubo.useNormalTexture;
    if (_e73 != 0) {
        let _e75 = f_WorldTangent_1;
        t = normalize(_e75);
        let _e77 = f_WorldBioTangent_1;
        b = normalize(_e77);
        let _e79 = f_WorldNormal_1;
        n = normalize(_e79);
        let _e81 = t;
        let _e82 = b;
        let _e83 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e81.x, _e81.y, _e81.z), vec3<f32>(_e82.x, _e82.y, _e82.z), vec3<f32>(_e83.x, _e83.y, _e83.z));
        let _e97 = f_Texcoord_1;
        let _e98 = textureSample(normalTexture, normalTextureSampler, _e97);
        nomral = _e98.xyz;
        let _e100 = tbn;
        let _e101 = nomral;
        let _e106 = ubo.normalMapScale;
        let _e108 = ubo.normalMapScale;
        nomral = normalize((_e100 * (((_e101 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e106, _e108, 1.0))));
    } else {
        let _e113 = f_WorldNormal_1;
        nomral = _e113;
    }
    let _e114 = nomral;
    return _e114;
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
    var mipCount: f32;
    var lod: f32;
    var reflectColor: vec3<f32>;
    var param_8: vec4<f32>;
    var ao: f32;
    var emissive: vec3<f32>;
    var param_9: vec4<f32>;
    var lsp_1: vec3<f32>;
    var shadowCol: f32;
    var outSide: bool;
    var param_10: vec3<f32>;
    var phi_631_: bool;
    var phi_632_: bool;
    var phi_645_: bool;
    var phi_646_: bool;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e112 = ubo.roughnessFactor;
    perceptualRoughness = _e112;
    let _e114 = ubo.metallicFactor;
    metallic = _e114;
    let _e116 = ubo.useMetallicRoughnessTexture;
    if (_e116 != 0) {
        let _e118 = f_Texcoord_1;
        let _e119 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e118);
        metallicRoughnessColor = _e119;
        let _e120 = perceptualRoughness;
        let _e122 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e120 * _e122);
        let _e124 = metallic;
        let _e126 = metallicRoughnessColor[2u];
        metallic = (_e124 * _e126);
    }
    let _e128 = perceptualRoughness;
    perceptualRoughness = clamp(_e128, 0.03999999910593033, 1.0);
    let _e130 = metallic;
    metallic = clamp(_e130, 0.0, 1.0);
    let _e132 = perceptualRoughness;
    let _e133 = perceptualRoughness;
    alphaRoughness = (_e132 * _e133);
    let _e136 = ubo.useBaseColorTexture;
    if (_e136 != 0) {
        let _e138 = f_Texcoord_1;
        let _e139 = textureSample(baseColorTexture, baseColorTextureSampler, _e138);
        baseColor = _e139;
    } else {
        let _e141 = ubo.baseColorFactor;
        baseColor = _e141;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e142 = baseColor;
    let _e144 = f0_;
    diffuseColor = (_e142.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e144));
    let _e147 = metallic;
    let _e149 = diffuseColor;
    diffuseColor = (_e149 * (1.0 - _e147));
    let _e151 = f0_;
    let _e152 = baseColor;
    let _e154 = metallic;
    specularColor = mix(_e151, _e152.xyz, vec3<f32>(_e154));
    let _e158 = specularColor[0u];
    let _e160 = specularColor[1u];
    let _e163 = specularColor[2u];
    reflectance = max(max(_e158, _e160), _e163);
    let _e165 = reflectance;
    reflectance90_ = clamp((_e165 * 25.0), 0.0, 1.0);
    let _e168 = specularColor;
    specularEnvironmentR0_ = _e168;
    let _e169 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e169);
    let _e171 = getNormal();
    n_1 = _e171;
    let _e173 = ubo.cameraPos;
    let _e175 = f_WorldPos_1;
    v = normalize((_e173.xyz - _e175.xyz));
    let _e180 = ubo.lightDir;
    l = normalize(_e180.xyz);
    let _e183 = v;
    let _e184 = l;
    h = normalize((_e183 + _e184));
    let _e187 = v;
    let _e188 = n_1;
    reflection = -(normalize(reflect(_e187, _e188)));
    let _e192 = n_1;
    let _e193 = l;
    NdotL_1 = clamp(dot(_e192, _e193), 0.0010000000474974513, 1.0);
    let _e196 = n_1;
    let _e197 = v;
    NdotV_1 = clamp(abs(dot(_e196, _e197)), 0.0010000000474974513, 1.0);
    let _e201 = n_1;
    let _e202 = h;
    NdotH = clamp(dot(_e201, _e202), 0.0, 1.0);
    let _e205 = l;
    let _e206 = h;
    LdotH = clamp(dot(_e205, _e206), 0.0, 1.0);
    let _e209 = v;
    let _e210 = h;
    VdotH = clamp(dot(_e209, _e210), 0.0, 1.0);
    let _e213 = NdotL_1;
    let _e214 = NdotV_1;
    let _e215 = NdotH;
    let _e216 = LdotH;
    let _e217 = VdotH;
    let _e218 = perceptualRoughness;
    let _e219 = metallic;
    let _e220 = specularEnvironmentR0_;
    let _e221 = specularEnvironmentR90_;
    let _e222 = alphaRoughness;
    let _e223 = diffuseColor;
    let _e224 = specularColor;
    pbrParam = PBRParam(_e213, _e214, _e215, _e216, _e217, _e218, _e219, _e220, _e221, _e222, _e223, _e224);
    let _e226 = pbrParam;
    param_4 = _e226;
    let _e227 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e227;
    let _e228 = pbrParam;
    param_5 = _e228;
    let _e229 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e229;
    let _e230 = pbrParam;
    param_6 = _e230;
    let _e231 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e231;
    let _e232 = D;
    let _e233 = G;
    let _e235 = F;
    let _e237 = NdotL_1;
    let _e239 = NdotV_1;
    specularBRDF = ((_e235 * (_e232 * _e233)) / vec3<f32>(((4.0 * _e237) * _e239)));
    let _e243 = F;
    let _e246 = pbrParam;
    param_7 = _e246;
    let _e247 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e243) * _e247);
    let _e250 = ubo.mipCount;
    mipCount = _e250;
    let _e251 = mipCount;
    let _e252 = perceptualRoughness;
    lod = (_e251 * _e252);
    let _e254 = v;
    let _e255 = n_1;
    let _e257 = lod;
    let _e258 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e254, _e255), _e257);
    param_8 = _e258;
    let _e259 = LINEARtoSRGBvf4_((&param_8));
    reflectColor = _e259.xyz;
    let _e261 = NdotL_1;
    let _e263 = ubo.lightColor;
    let _e266 = specularBRDF;
    let _e267 = diffuseBRDF;
    let _e270 = reflectColor;
    let _e271 = specularColor;
    let _e273 = (((_e263.xyz * _e261) * (_e266 + _e267)) + (_e270 * _e271));
    col[0u] = _e273.x;
    col[1u] = _e273.y;
    col[2u] = _e273.z;
    let _e281 = ubo.useOcclusionTexture;
    if (_e281 != 0) {
        let _e283 = f_Texcoord_1;
        let _e284 = textureSample(occlusionTexture, occlusionTextureSampler, _e283);
        ao = _e284.x;
        let _e286 = col;
        let _e288 = col;
        let _e290 = ao;
        let _e293 = ubo.occlusionStrength;
        let _e295 = mix(_e286.xyz, (_e288.xyz * _e290), vec3<f32>(_e293));
        col[0u] = _e295.x;
        col[1u] = _e295.y;
        col[2u] = _e295.z;
    }
    let _e303 = ubo.useEmissiveTexture;
    if (_e303 != 0) {
        let _e305 = f_Texcoord_1;
        let _e306 = textureSample(emissiveTexture, emissiveTextureSampler, _e305);
        param_9 = _e306;
        let _e307 = SRGBtoLINEARvf4_((&param_9));
        let _e310 = ubo.emissiveFactor;
        emissive = (_e307.xyz * _e310.xyz);
        let _e313 = emissive;
        let _e314 = col;
        let _e316 = (_e314.xyz + _e313);
        col[0u] = _e316.x;
        col[1u] = _e316.y;
        col[2u] = _e316.z;
    }
    let _e323 = f_LightSpacePos_1;
    let _e326 = f_LightSpacePos_1[3u];
    lsp_1 = (_e323.xyz / vec3<f32>(_e326));
    let _e329 = lsp_1;
    lsp_1 = ((_e329 * 0.5) + vec3<f32>(0.5));
    shadowCol = 1.0;
    let _e334 = f_LightSpacePos_1[2u];
    let _e335 = (_e334 <= 0.0);
    phi_632_ = _e335;
    if !(_e335) {
        let _e338 = lsp_1[0u];
        let _e339 = (_e338 < 0.0);
        phi_631_ = _e339;
        if !(_e339) {
            let _e342 = lsp_1[1u];
            phi_631_ = (_e342 < 0.0);
        }
        let _e345 = phi_631_;
        phi_632_ = _e345;
    }
    let _e347 = phi_632_;
    phi_646_ = _e347;
    if !(_e347) {
        let _e350 = lsp_1[0u];
        let _e351 = (_e350 >= 1.0);
        phi_645_ = _e351;
        if !(_e351) {
            let _e354 = lsp_1[1u];
            phi_645_ = (_e354 >= 1.0);
        }
        let _e357 = phi_645_;
        phi_646_ = _e357;
    }
    let _e359 = phi_646_;
    outSide = _e359;
    let _e360 = outSide;
    if !(_e360) {
        let _e362 = lsp_1;
        param_10 = _e362;
        let _e363 = CalcShadowvf3_((&param_10));
        shadowCol = _e363;
    }
    let _e364 = shadowCol;
    let _e365 = col;
    let _e367 = (_e365.xyz * _e364);
    col[0u] = _e367.x;
    col[1u] = _e367.y;
    col[2u] = _e367.z;
    let _e374 = col;
    let _e376 = pow(_e374.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e376.x;
    col[1u] = _e376.y;
    col[2u] = _e376.z;
    let _e384 = baseColor[3u];
    col[3u] = _e384;
    let _e386 = col;
    outColor = _e386;
    return;
}

@fragment 
fn main(@location(3) f_WorldTangent: vec3<f32>, @location(4) f_WorldBioTangent: vec3<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(1) f_Texcoord: vec2<f32>, @location(2) f_WorldPos: vec4<f32>, @location(5) f_LightSpacePos: vec4<f32>) -> @location(0) vec4<f32> {
    f_WorldTangent_1 = f_WorldTangent;
    f_WorldBioTangent_1 = f_WorldBioTangent;
    f_WorldNormal_1 = f_WorldNormal;
    f_Texcoord_1 = f_Texcoord;
    f_WorldPos_1 = f_WorldPos;
    f_LightSpacePos_1 = f_LightSpacePos;
    main_1();
    let _e13 = outColor;
    return _e13;
}
