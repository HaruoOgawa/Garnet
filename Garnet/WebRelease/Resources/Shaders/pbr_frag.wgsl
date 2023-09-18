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
    ShadowMapX: f32,
    ShadowMapY: f32,
    useBaseColorTexture: i32,
    useMetallicRoughnessTexture: i32,
    useEmissiveTexture: i32,
    useNormalTexture: i32,
    useOcclusionTexture: i32,
    useCubeMap: i32,
    useShadowMap: i32,
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

fn ComputePCFvf2_(uv: ptr<function, vec2<f32>>) -> vec2<f32> {
    var moments: vec2<f32>;

    moments = vec2<f32>(0.0, 0.0);
    let _e74 = (*uv);
    let _e75 = textureSample(shadowmapTexture, shadowmapTextureSampler, _e74);
    moments = _e75.xy;
    let _e77 = moments;
    return _e77;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e82 = (*lsp);
    param = _e82.xy;
    let _e84 = ComputePCFvf2_((&param));
    moments_1 = _e84;
    let _e85 = (*nomral);
    let _e86 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e85, _e86))));
    let _e92 = (*lsp)[2u];
    let _e93 = ShadowBias;
    distance = (_e92 - _e93);
    let _e95 = distance;
    let _e97 = moments_1[0u];
    if (_e95 <= _e97) {
        return 1.0;
    }
    let _e100 = moments_1[1u];
    let _e102 = moments_1[0u];
    let _e104 = moments_1[0u];
    variance = (_e100 - (_e102 * _e104));
    let _e107 = variance;
    variance = max(0.004999999888241291, _e107);
    let _e109 = distance;
    let _e111 = moments_1[0u];
    d = (_e109 - _e111);
    let _e113 = variance;
    let _e114 = variance;
    let _e115 = d;
    let _e116 = d;
    p_max = (_e113 / (_e114 + (_e115 * _e116)));
    let _e120 = p_max;
    return _e120;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e73 = (*srgbIn);
    let _e75 = pow(_e73.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e77 = (*srgbIn)[3u];
    return vec4<f32>(_e75.x, _e75.y, _e75.z, _e77);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e73 = (*srgbIn_1);
    let _e75 = pow(_e73.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e77 = (*srgbIn_1)[3u];
    return vec4<f32>(_e75.x, _e75.y, _e75.z, _e77);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e74 = (*param_1).diffuseColor;
    return (_e74 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> vec3<f32> {
    let _e74 = (*param_2).reflectance0_;
    let _e76 = (*param_2).reflectance90_;
    let _e78 = (*param_2).reflectance0_;
    let _e81 = (*param_2).VdotH;
    return (_e74 + ((_e76 - _e78) * pow(clamp((1.0 - _e81), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e79 = (*param_3).NdotL;
    NdotL = _e79;
    let _e81 = (*param_3).NdotV;
    NdotV = _e81;
    let _e83 = (*param_3).alphaRoughness;
    r = _e83;
    let _e84 = NdotL;
    let _e86 = NdotL;
    let _e87 = r;
    let _e88 = r;
    let _e90 = r;
    let _e91 = r;
    let _e94 = NdotL;
    let _e95 = NdotL;
    attenuationL = ((2.0 * _e84) / (_e86 + sqrt(((_e87 * _e88) + ((1.0 - (_e90 * _e91)) * (_e94 * _e95))))));
    let _e102 = NdotV;
    let _e104 = NdotV;
    let _e105 = r;
    let _e106 = r;
    let _e108 = r;
    let _e109 = r;
    let _e112 = NdotV;
    let _e113 = NdotV;
    attenuationV = ((2.0 * _e102) / (_e104 + sqrt(((_e105 * _e106) + ((1.0 - (_e108 * _e109)) * (_e112 * _e113))))));
    let _e120 = attenuationL;
    let _e121 = attenuationV;
    return (_e120 * _e121);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_4: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e76 = (*param_4).alphaRoughness;
    let _e78 = (*param_4).alphaRoughness;
    roughness2_ = (_e76 * _e78);
    let _e81 = (*param_4).NdotH;
    let _e82 = roughness2_;
    let _e85 = (*param_4).NdotH;
    let _e88 = (*param_4).NdotH;
    f = ((((_e81 * _e82) - _e85) * _e88) + 1.0);
    let _e91 = roughness2_;
    let _e92 = f;
    let _e94 = f;
    return (_e91 / ((3.1415927410125732 * _e92) * _e94));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e78 = ubo.useNormalTexture;
    if (_e78 != 0) {
        let _e80 = f_WorldTangent_1;
        t = normalize(_e80);
        let _e82 = f_WorldBioTangent_1;
        b = normalize(_e82);
        let _e84 = f_WorldNormal_1;
        n = normalize(_e84);
        let _e86 = t;
        let _e87 = b;
        let _e88 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e86.x, _e86.y, _e86.z), vec3<f32>(_e87.x, _e87.y, _e87.z), vec3<f32>(_e88.x, _e88.y, _e88.z));
        let _e102 = f_Texcoord_1;
        let _e103 = textureSample(normalTexture, normalTextureSampler, _e102);
        nomral_1 = _e103.xyz;
        let _e105 = tbn;
        let _e106 = nomral_1;
        let _e111 = ubo.normalMapScale;
        let _e113 = ubo.normalMapScale;
        nomral_1 = normalize((_e105 * (((_e106 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e111, _e113, 1.0))));
    } else {
        let _e118 = f_WorldNormal_1;
        nomral_1 = _e118;
    }
    let _e119 = nomral_1;
    return _e119;
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
    var param_5: PBRParam;
    var G: f32;
    var param_6: PBRParam;
    var F: vec3<f32>;
    var param_7: PBRParam;
    var specularBRDF: vec3<f32>;
    var diffuseBRDF: vec3<f32>;
    var param_8: PBRParam;
    var reflectColor: vec3<f32>;
    var mipCount: f32;
    var lod: f32;
    var param_9: vec4<f32>;
    var ao: f32;
    var emissive: vec3<f32>;
    var param_10: vec4<f32>;
    var lsp_1: vec3<f32>;
    var shadowCol: f32;
    var outSide: bool;
    var param_11: vec3<f32>;
    var param_12: vec3<f32>;
    var param_13: vec3<f32>;
    var phi_694_: bool;
    var phi_695_: bool;
    var phi_708_: bool;
    var phi_709_: bool;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e119 = ubo.roughnessFactor;
    perceptualRoughness = _e119;
    let _e121 = ubo.metallicFactor;
    metallic = _e121;
    let _e123 = ubo.useMetallicRoughnessTexture;
    if (_e123 != 0) {
        let _e125 = f_Texcoord_1;
        let _e126 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e125);
        metallicRoughnessColor = _e126;
        let _e127 = perceptualRoughness;
        let _e129 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e127 * _e129);
        let _e131 = metallic;
        let _e133 = metallicRoughnessColor[2u];
        metallic = (_e131 * _e133);
    }
    let _e135 = perceptualRoughness;
    perceptualRoughness = clamp(_e135, 0.03999999910593033, 1.0);
    let _e137 = metallic;
    metallic = clamp(_e137, 0.0, 1.0);
    let _e139 = perceptualRoughness;
    let _e140 = perceptualRoughness;
    alphaRoughness = (_e139 * _e140);
    let _e143 = ubo.useBaseColorTexture;
    if (_e143 != 0) {
        let _e145 = f_Texcoord_1;
        let _e146 = textureSample(baseColorTexture, baseColorTextureSampler, _e145);
        baseColor = _e146;
    } else {
        let _e148 = ubo.baseColorFactor;
        baseColor = _e148;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e149 = baseColor;
    let _e151 = f0_;
    diffuseColor = (_e149.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e151));
    let _e154 = metallic;
    let _e156 = diffuseColor;
    diffuseColor = (_e156 * (1.0 - _e154));
    let _e158 = f0_;
    let _e159 = baseColor;
    let _e161 = metallic;
    specularColor = mix(_e158, _e159.xyz, vec3<f32>(_e161));
    let _e165 = specularColor[0u];
    let _e167 = specularColor[1u];
    let _e170 = specularColor[2u];
    reflectance = max(max(_e165, _e167), _e170);
    let _e172 = reflectance;
    reflectance90_ = clamp((_e172 * 25.0), 0.0, 1.0);
    let _e175 = specularColor;
    specularEnvironmentR0_ = _e175;
    let _e176 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e176);
    let _e178 = getNormal();
    n_1 = _e178;
    let _e180 = ubo.cameraPos;
    let _e182 = f_WorldPos_1;
    v = normalize((_e180.xyz - _e182.xyz));
    let _e187 = ubo.lightDir;
    l = normalize(_e187.xyz);
    let _e190 = v;
    let _e191 = l;
    h = normalize((_e190 + _e191));
    let _e194 = v;
    let _e195 = n_1;
    reflection = -(normalize(reflect(_e194, _e195)));
    let _e199 = n_1;
    let _e200 = l;
    NdotL_1 = clamp(dot(_e199, _e200), 0.0010000000474974513, 1.0);
    let _e203 = n_1;
    let _e204 = v;
    NdotV_1 = clamp(abs(dot(_e203, _e204)), 0.0010000000474974513, 1.0);
    let _e208 = n_1;
    let _e209 = h;
    NdotH = clamp(dot(_e208, _e209), 0.0, 1.0);
    let _e212 = l;
    let _e213 = h;
    LdotH = clamp(dot(_e212, _e213), 0.0, 1.0);
    let _e216 = v;
    let _e217 = h;
    VdotH = clamp(dot(_e216, _e217), 0.0, 1.0);
    let _e220 = NdotL_1;
    let _e221 = NdotV_1;
    let _e222 = NdotH;
    let _e223 = LdotH;
    let _e224 = VdotH;
    let _e225 = perceptualRoughness;
    let _e226 = metallic;
    let _e227 = specularEnvironmentR0_;
    let _e228 = specularEnvironmentR90_;
    let _e229 = alphaRoughness;
    let _e230 = diffuseColor;
    let _e231 = specularColor;
    pbrParam = PBRParam(_e220, _e221, _e222, _e223, _e224, _e225, _e226, _e227, _e228, _e229, _e230, _e231);
    let _e233 = pbrParam;
    param_5 = _e233;
    let _e234 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    D = _e234;
    let _e235 = pbrParam;
    param_6 = _e235;
    let _e236 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    G = _e236;
    let _e237 = pbrParam;
    param_7 = _e237;
    let _e238 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    F = _e238;
    let _e239 = D;
    let _e240 = G;
    let _e242 = F;
    let _e244 = NdotL_1;
    let _e246 = NdotV_1;
    specularBRDF = ((_e242 * (_e239 * _e240)) / vec3<f32>(((4.0 * _e244) * _e246)));
    let _e250 = F;
    let _e253 = pbrParam;
    param_8 = _e253;
    let _e254 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_8));
    diffuseBRDF = ((vec3<f32>(1.0) - _e250) * _e254);
    reflectColor = vec3<f32>(1.0, 1.0, 1.0);
    let _e257 = ubo.useCubeMap;
    if (_e257 != 0) {
        let _e260 = ubo.mipCount;
        mipCount = _e260;
        let _e261 = mipCount;
        let _e262 = perceptualRoughness;
        lod = (_e261 * _e262);
        let _e264 = v;
        let _e265 = n_1;
        let _e267 = lod;
        let _e268 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e264, _e265), _e267);
        param_9 = _e268;
        let _e269 = LINEARtoSRGBvf4_((&param_9));
        reflectColor = _e269.xyz;
    }
    let _e271 = NdotL_1;
    let _e273 = ubo.lightColor;
    let _e276 = specularBRDF;
    let _e277 = diffuseBRDF;
    let _e280 = reflectColor;
    let _e281 = specularColor;
    let _e283 = (((_e273.xyz * _e271) * (_e276 + _e277)) + (_e280 * _e281));
    col[0u] = _e283.x;
    col[1u] = _e283.y;
    col[2u] = _e283.z;
    let _e291 = ubo.useOcclusionTexture;
    if (_e291 != 0) {
        let _e293 = f_Texcoord_1;
        let _e294 = textureSample(occlusionTexture, occlusionTextureSampler, _e293);
        ao = _e294.x;
        let _e296 = col;
        let _e298 = col;
        let _e300 = ao;
        let _e303 = ubo.occlusionStrength;
        let _e305 = mix(_e296.xyz, (_e298.xyz * _e300), vec3<f32>(_e303));
        col[0u] = _e305.x;
        col[1u] = _e305.y;
        col[2u] = _e305.z;
    }
    let _e313 = ubo.useEmissiveTexture;
    if (_e313 != 0) {
        let _e315 = f_Texcoord_1;
        let _e316 = textureSample(emissiveTexture, emissiveTextureSampler, _e315);
        param_10 = _e316;
        let _e317 = SRGBtoLINEARvf4_((&param_10));
        let _e320 = ubo.emissiveFactor;
        emissive = (_e317.xyz * _e320.xyz);
        let _e323 = emissive;
        let _e324 = col;
        let _e326 = (_e324.xyz + _e323);
        col[0u] = _e326.x;
        col[1u] = _e326.y;
        col[2u] = _e326.z;
    }
    let _e334 = ubo.useShadowMap;
    if (_e334 != 0) {
        let _e336 = f_LightSpacePos_1;
        let _e339 = f_LightSpacePos_1[3u];
        lsp_1 = (_e336.xyz / vec3<f32>(_e339));
        let _e342 = lsp_1;
        lsp_1 = ((_e342 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e347 = f_LightSpacePos_1[2u];
        let _e348 = (_e347 <= 0.0);
        phi_695_ = _e348;
        if !(_e348) {
            let _e351 = lsp_1[0u];
            let _e352 = (_e351 < 0.0);
            phi_694_ = _e352;
            if !(_e352) {
                let _e355 = lsp_1[1u];
                phi_694_ = (_e355 < 0.0);
            }
            let _e358 = phi_694_;
            phi_695_ = _e358;
        }
        let _e360 = phi_695_;
        phi_709_ = _e360;
        if !(_e360) {
            let _e363 = lsp_1[0u];
            let _e364 = (_e363 > 1.0);
            phi_708_ = _e364;
            if !(_e364) {
                let _e367 = lsp_1[1u];
                phi_708_ = (_e367 > 1.0);
            }
            let _e370 = phi_708_;
            phi_709_ = _e370;
        }
        let _e372 = phi_709_;
        outSide = _e372;
        let _e373 = outSide;
        if !(_e373) {
            let _e375 = lsp_1;
            param_11 = _e375;
            let _e376 = n_1;
            param_12 = _e376;
            let _e377 = l;
            param_13 = _e377;
            let _e378 = CalcShadowvf3vf3vf3_((&param_11), (&param_12), (&param_13));
            shadowCol = _e378;
        }
        let _e379 = shadowCol;
        let _e380 = col;
        let _e382 = (_e380.xyz * _e379);
        col[0u] = _e382.x;
        col[1u] = _e382.y;
        col[2u] = _e382.z;
    }
    let _e389 = col;
    let _e391 = pow(_e389.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e391.x;
    col[1u] = _e391.y;
    col[2u] = _e391.z;
    let _e399 = baseColor[3u];
    col[3u] = _e399;
    let _e401 = col;
    outColor = _e401;
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
