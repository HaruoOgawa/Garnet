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

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e78 = (*lsp);
    let _e80 = textureSample(shadowmapTexture, shadowmapTextureSampler, _e78.xy);
    moments = _e80.xy;
    let _e82 = (*nomral);
    let _e83 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e82, _e83))));
    let _e89 = (*lsp)[2u];
    let _e90 = ShadowBias;
    distance = (_e89 - _e90);
    let _e92 = distance;
    let _e94 = moments[0u];
    if (_e92 <= _e94) {
        return 1.0;
    }
    let _e97 = moments[1u];
    let _e99 = moments[0u];
    let _e101 = moments[0u];
    variance = (_e97 - (_e99 * _e101));
    let _e104 = variance;
    variance = max(0.004999999888241291, _e104);
    let _e106 = distance;
    let _e108 = moments[0u];
    d = (_e106 - _e108);
    let _e110 = variance;
    let _e111 = variance;
    let _e112 = d;
    let _e113 = d;
    p_max = (_e110 / (_e111 + (_e112 * _e113)));
    let _e117 = p_max;
    return _e117;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e70 = (*srgbIn);
    let _e72 = pow(_e70.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e74 = (*srgbIn)[3u];
    return vec4<f32>(_e72.x, _e72.y, _e72.z, _e74);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e70 = (*srgbIn_1);
    let _e72 = pow(_e70.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e74 = (*srgbIn_1)[3u];
    return vec4<f32>(_e72.x, _e72.y, _e72.z, _e74);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e71 = (*param).diffuseColor;
    return (_e71 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e71 = (*param_1).reflectance0_;
    let _e73 = (*param_1).reflectance90_;
    let _e75 = (*param_1).reflectance0_;
    let _e78 = (*param_1).VdotH;
    return (_e71 + ((_e73 - _e75) * pow(clamp((1.0 - _e78), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e76 = (*param_2).NdotL;
    NdotL = _e76;
    let _e78 = (*param_2).NdotV;
    NdotV = _e78;
    let _e80 = (*param_2).alphaRoughness;
    r = _e80;
    let _e81 = NdotL;
    let _e83 = NdotL;
    let _e84 = r;
    let _e85 = r;
    let _e87 = r;
    let _e88 = r;
    let _e91 = NdotL;
    let _e92 = NdotL;
    attenuationL = ((2.0 * _e81) / (_e83 + sqrt(((_e84 * _e85) + ((1.0 - (_e87 * _e88)) * (_e91 * _e92))))));
    let _e99 = NdotV;
    let _e101 = NdotV;
    let _e102 = r;
    let _e103 = r;
    let _e105 = r;
    let _e106 = r;
    let _e109 = NdotV;
    let _e110 = NdotV;
    attenuationV = ((2.0 * _e99) / (_e101 + sqrt(((_e102 * _e103) + ((1.0 - (_e105 * _e106)) * (_e109 * _e110))))));
    let _e117 = attenuationL;
    let _e118 = attenuationV;
    return (_e117 * _e118);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e73 = (*param_3).alphaRoughness;
    let _e75 = (*param_3).alphaRoughness;
    roughness2_ = (_e73 * _e75);
    let _e78 = (*param_3).NdotH;
    let _e79 = roughness2_;
    let _e82 = (*param_3).NdotH;
    let _e85 = (*param_3).NdotH;
    f = ((((_e78 * _e79) - _e82) * _e85) + 1.0);
    let _e88 = roughness2_;
    let _e89 = f;
    let _e91 = f;
    return (_e88 / ((3.1415927410125732 * _e89) * _e91));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e75 = ubo.useNormalTexture;
    if (_e75 != 0) {
        let _e77 = f_WorldTangent_1;
        t = normalize(_e77);
        let _e79 = f_WorldBioTangent_1;
        b = normalize(_e79);
        let _e81 = f_WorldNormal_1;
        n = normalize(_e81);
        let _e83 = t;
        let _e84 = b;
        let _e85 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e83.x, _e83.y, _e83.z), vec3<f32>(_e84.x, _e84.y, _e84.z), vec3<f32>(_e85.x, _e85.y, _e85.z));
        let _e99 = f_Texcoord_1;
        let _e100 = textureSample(normalTexture, normalTextureSampler, _e99);
        nomral_1 = _e100.xyz;
        let _e102 = tbn;
        let _e103 = nomral_1;
        let _e108 = ubo.normalMapScale;
        let _e110 = ubo.normalMapScale;
        nomral_1 = normalize((_e102 * (((_e103 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e108, _e110, 1.0))));
    } else {
        let _e115 = f_WorldNormal_1;
        nomral_1 = _e115;
    }
    let _e116 = nomral_1;
    return _e116;
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
    var param_11: vec3<f32>;
    var param_12: vec3<f32>;
    var phi_670_: bool;
    var phi_671_: bool;
    var phi_684_: bool;
    var phi_685_: bool;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e116 = ubo.roughnessFactor;
    perceptualRoughness = _e116;
    let _e118 = ubo.metallicFactor;
    metallic = _e118;
    let _e120 = ubo.useMetallicRoughnessTexture;
    if (_e120 != 0) {
        let _e122 = f_Texcoord_1;
        let _e123 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e122);
        metallicRoughnessColor = _e123;
        let _e124 = perceptualRoughness;
        let _e126 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e124 * _e126);
        let _e128 = metallic;
        let _e130 = metallicRoughnessColor[2u];
        metallic = (_e128 * _e130);
    }
    let _e132 = perceptualRoughness;
    perceptualRoughness = clamp(_e132, 0.03999999910593033, 1.0);
    let _e134 = metallic;
    metallic = clamp(_e134, 0.0, 1.0);
    let _e136 = perceptualRoughness;
    let _e137 = perceptualRoughness;
    alphaRoughness = (_e136 * _e137);
    let _e140 = ubo.useBaseColorTexture;
    if (_e140 != 0) {
        let _e142 = f_Texcoord_1;
        let _e143 = textureSample(baseColorTexture, baseColorTextureSampler, _e142);
        baseColor = _e143;
    } else {
        let _e145 = ubo.baseColorFactor;
        baseColor = _e145;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e146 = baseColor;
    let _e148 = f0_;
    diffuseColor = (_e146.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e148));
    let _e151 = metallic;
    let _e153 = diffuseColor;
    diffuseColor = (_e153 * (1.0 - _e151));
    let _e155 = f0_;
    let _e156 = baseColor;
    let _e158 = metallic;
    specularColor = mix(_e155, _e156.xyz, vec3<f32>(_e158));
    let _e162 = specularColor[0u];
    let _e164 = specularColor[1u];
    let _e167 = specularColor[2u];
    reflectance = max(max(_e162, _e164), _e167);
    let _e169 = reflectance;
    reflectance90_ = clamp((_e169 * 25.0), 0.0, 1.0);
    let _e172 = specularColor;
    specularEnvironmentR0_ = _e172;
    let _e173 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e173);
    let _e175 = getNormal();
    n_1 = _e175;
    let _e177 = ubo.cameraPos;
    let _e179 = f_WorldPos_1;
    v = normalize((_e177.xyz - _e179.xyz));
    let _e184 = ubo.lightDir;
    l = normalize(_e184.xyz);
    let _e187 = v;
    let _e188 = l;
    h = normalize((_e187 + _e188));
    let _e191 = v;
    let _e192 = n_1;
    reflection = -(normalize(reflect(_e191, _e192)));
    let _e196 = n_1;
    let _e197 = l;
    NdotL_1 = clamp(dot(_e196, _e197), 0.0010000000474974513, 1.0);
    let _e200 = n_1;
    let _e201 = v;
    NdotV_1 = clamp(abs(dot(_e200, _e201)), 0.0010000000474974513, 1.0);
    let _e205 = n_1;
    let _e206 = h;
    NdotH = clamp(dot(_e205, _e206), 0.0, 1.0);
    let _e209 = l;
    let _e210 = h;
    LdotH = clamp(dot(_e209, _e210), 0.0, 1.0);
    let _e213 = v;
    let _e214 = h;
    VdotH = clamp(dot(_e213, _e214), 0.0, 1.0);
    let _e217 = NdotL_1;
    let _e218 = NdotV_1;
    let _e219 = NdotH;
    let _e220 = LdotH;
    let _e221 = VdotH;
    let _e222 = perceptualRoughness;
    let _e223 = metallic;
    let _e224 = specularEnvironmentR0_;
    let _e225 = specularEnvironmentR90_;
    let _e226 = alphaRoughness;
    let _e227 = diffuseColor;
    let _e228 = specularColor;
    pbrParam = PBRParam(_e217, _e218, _e219, _e220, _e221, _e222, _e223, _e224, _e225, _e226, _e227, _e228);
    let _e230 = pbrParam;
    param_4 = _e230;
    let _e231 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e231;
    let _e232 = pbrParam;
    param_5 = _e232;
    let _e233 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e233;
    let _e234 = pbrParam;
    param_6 = _e234;
    let _e235 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e235;
    let _e236 = D;
    let _e237 = G;
    let _e239 = F;
    let _e241 = NdotL_1;
    let _e243 = NdotV_1;
    specularBRDF = ((_e239 * (_e236 * _e237)) / vec3<f32>(((4.0 * _e241) * _e243)));
    let _e247 = F;
    let _e250 = pbrParam;
    param_7 = _e250;
    let _e251 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e247) * _e251);
    let _e254 = ubo.mipCount;
    mipCount = _e254;
    let _e255 = mipCount;
    let _e256 = perceptualRoughness;
    lod = (_e255 * _e256);
    let _e258 = v;
    let _e259 = n_1;
    let _e261 = lod;
    let _e262 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e258, _e259), _e261);
    param_8 = _e262;
    let _e263 = LINEARtoSRGBvf4_((&param_8));
    reflectColor = _e263.xyz;
    let _e265 = NdotL_1;
    let _e267 = ubo.lightColor;
    let _e270 = specularBRDF;
    let _e271 = diffuseBRDF;
    let _e274 = reflectColor;
    let _e275 = specularColor;
    let _e277 = (((_e267.xyz * _e265) * (_e270 + _e271)) + (_e274 * _e275));
    col[0u] = _e277.x;
    col[1u] = _e277.y;
    col[2u] = _e277.z;
    let _e285 = ubo.useOcclusionTexture;
    if (_e285 != 0) {
        let _e287 = f_Texcoord_1;
        let _e288 = textureSample(occlusionTexture, occlusionTextureSampler, _e287);
        ao = _e288.x;
        let _e290 = col;
        let _e292 = col;
        let _e294 = ao;
        let _e297 = ubo.occlusionStrength;
        let _e299 = mix(_e290.xyz, (_e292.xyz * _e294), vec3<f32>(_e297));
        col[0u] = _e299.x;
        col[1u] = _e299.y;
        col[2u] = _e299.z;
    }
    let _e307 = ubo.useEmissiveTexture;
    if (_e307 != 0) {
        let _e309 = f_Texcoord_1;
        let _e310 = textureSample(emissiveTexture, emissiveTextureSampler, _e309);
        param_9 = _e310;
        let _e311 = SRGBtoLINEARvf4_((&param_9));
        let _e314 = ubo.emissiveFactor;
        emissive = (_e311.xyz * _e314.xyz);
        let _e317 = emissive;
        let _e318 = col;
        let _e320 = (_e318.xyz + _e317);
        col[0u] = _e320.x;
        col[1u] = _e320.y;
        col[2u] = _e320.z;
    }
    let _e327 = f_LightSpacePos_1;
    let _e330 = f_LightSpacePos_1[3u];
    lsp_1 = (_e327.xyz / vec3<f32>(_e330));
    let _e333 = lsp_1;
    lsp_1 = ((_e333 * 0.5) + vec3<f32>(0.5));
    shadowCol = 1.0;
    let _e338 = f_LightSpacePos_1[2u];
    let _e339 = (_e338 <= 0.0);
    phi_671_ = _e339;
    if !(_e339) {
        let _e342 = lsp_1[0u];
        let _e343 = (_e342 < 0.0);
        phi_670_ = _e343;
        if !(_e343) {
            let _e346 = lsp_1[1u];
            phi_670_ = (_e346 < 0.0);
        }
        let _e349 = phi_670_;
        phi_671_ = _e349;
    }
    let _e351 = phi_671_;
    phi_685_ = _e351;
    if !(_e351) {
        let _e354 = lsp_1[0u];
        let _e355 = (_e354 > 1.0);
        phi_684_ = _e355;
        if !(_e355) {
            let _e358 = lsp_1[1u];
            phi_684_ = (_e358 > 1.0);
        }
        let _e361 = phi_684_;
        phi_685_ = _e361;
    }
    let _e363 = phi_685_;
    outSide = _e363;
    let _e364 = outSide;
    if !(_e364) {
        let _e366 = lsp_1;
        param_10 = _e366;
        let _e367 = n_1;
        param_11 = _e367;
        let _e368 = l;
        param_12 = _e368;
        let _e369 = CalcShadowvf3vf3vf3_((&param_10), (&param_11), (&param_12));
        shadowCol = _e369;
    }
    let _e370 = shadowCol;
    let _e371 = col;
    let _e373 = (_e371.xyz * _e370);
    col[0u] = _e373.x;
    col[1u] = _e373.y;
    col[2u] = _e373.z;
    let _e380 = col;
    let _e382 = pow(_e380.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e382.x;
    col[1u] = _e382.y;
    col[2u] = _e382.z;
    let _e390 = baseColor[3u];
    col[3u] = _e390;
    let _e392 = col;
    outColor = _e392;
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
