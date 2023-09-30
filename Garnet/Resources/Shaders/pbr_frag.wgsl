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
    let _e71 = (*uv);
    let _e72 = textureSample(shadowmapTexture, shadowmapTextureSampler, _e71);
    moments = _e72.xy;
    let _e74 = moments;
    return _e74;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e79 = (*lsp);
    param = _e79.xy;
    let _e81 = ComputePCFvf2_((&param));
    moments_1 = _e81;
    let _e82 = (*nomral);
    let _e83 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e82, _e83))));
    let _e89 = (*lsp)[2u];
    let _e90 = ShadowBias;
    distance = (_e89 - _e90);
    let _e92 = distance;
    let _e94 = moments_1[0u];
    if (_e92 <= _e94) {
        return 1.0;
    }
    let _e97 = moments_1[1u];
    let _e99 = moments_1[0u];
    let _e101 = moments_1[0u];
    variance = (_e97 - (_e99 * _e101));
    let _e104 = variance;
    variance = max(0.004999999888241291, _e104);
    let _e106 = distance;
    let _e108 = moments_1[0u];
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

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e71 = (*param_1).diffuseColor;
    return (_e71 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> vec3<f32> {
    let _e71 = (*param_2).reflectance0_;
    let _e73 = (*param_2).reflectance90_;
    let _e75 = (*param_2).reflectance0_;
    let _e78 = (*param_2).VdotH;
    return (_e71 + ((_e73 - _e75) * pow(clamp((1.0 - _e78), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e76 = (*param_3).NdotL;
    NdotL = _e76;
    let _e78 = (*param_3).NdotV;
    NdotV = _e78;
    let _e80 = (*param_3).alphaRoughness;
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

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_4: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e73 = (*param_4).alphaRoughness;
    let _e75 = (*param_4).alphaRoughness;
    roughness2_ = (_e73 * _e75);
    let _e78 = (*param_4).NdotH;
    let _e79 = roughness2_;
    let _e82 = (*param_4).NdotH;
    let _e85 = (*param_4).NdotH;
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
    var specular: vec3<f32>;
    var diffuse: vec3<f32>;
    var D: f32;
    var param_5: PBRParam;
    var G: f32;
    var param_6: PBRParam;
    var F: vec3<f32>;
    var param_7: PBRParam;
    var param_8: PBRParam;
    var reflectColor: vec3<f32>;
    var mipCount: f32;
    var lod: f32;
    var param_9: vec4<f32>;
    var gi_diffuse: vec3<f32>;
    var ao: f32;
    var emissive: vec3<f32>;
    var param_10: vec4<f32>;
    var lsp_1: vec3<f32>;
    var shadowCol: f32;
    var outSide: bool;
    var param_11: vec3<f32>;
    var param_12: vec3<f32>;
    var param_13: vec3<f32>;
    var phi_711_: bool;
    var phi_712_: bool;
    var phi_725_: bool;
    var phi_726_: bool;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e117 = ubo.roughnessFactor;
    perceptualRoughness = _e117;
    let _e119 = ubo.metallicFactor;
    metallic = _e119;
    let _e121 = ubo.useMetallicRoughnessTexture;
    if (_e121 != 0) {
        let _e123 = f_Texcoord_1;
        let _e124 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e123);
        metallicRoughnessColor = _e124;
        let _e125 = perceptualRoughness;
        let _e127 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e125 * _e127);
        let _e129 = metallic;
        let _e131 = metallicRoughnessColor[2u];
        metallic = (_e129 * _e131);
    }
    let _e133 = perceptualRoughness;
    perceptualRoughness = clamp(_e133, 0.03999999910593033, 1.0);
    let _e135 = metallic;
    metallic = clamp(_e135, 0.0, 1.0);
    let _e137 = perceptualRoughness;
    let _e138 = perceptualRoughness;
    alphaRoughness = (_e137 * _e138);
    let _e141 = ubo.useBaseColorTexture;
    if (_e141 != 0) {
        let _e143 = f_Texcoord_1;
        let _e144 = textureSample(baseColorTexture, baseColorTextureSampler, _e143);
        baseColor = _e144;
    } else {
        let _e146 = ubo.baseColorFactor;
        baseColor = _e146;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e147 = baseColor;
    let _e149 = f0_;
    diffuseColor = (_e147.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e149));
    let _e152 = metallic;
    let _e154 = diffuseColor;
    diffuseColor = (_e154 * (1.0 - _e152));
    let _e156 = f0_;
    let _e157 = baseColor;
    let _e159 = metallic;
    specularColor = mix(_e156, _e157.xyz, vec3<f32>(_e159));
    let _e163 = specularColor[0u];
    let _e165 = specularColor[1u];
    let _e168 = specularColor[2u];
    reflectance = max(max(_e163, _e165), _e168);
    let _e170 = reflectance;
    reflectance90_ = clamp((_e170 * 25.0), 0.0, 1.0);
    let _e173 = specularColor;
    specularEnvironmentR0_ = _e173;
    let _e174 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e174);
    let _e176 = getNormal();
    n_1 = _e176;
    let _e178 = ubo.cameraPos;
    let _e180 = f_WorldPos_1;
    v = normalize((_e178.xyz - _e180.xyz));
    let _e185 = ubo.lightDir;
    l = normalize(_e185.xyz);
    let _e188 = v;
    let _e189 = l;
    h = normalize((_e188 + _e189));
    let _e192 = v;
    let _e193 = n_1;
    reflection = -(normalize(reflect(_e192, _e193)));
    let _e197 = n_1;
    let _e198 = l;
    NdotL_1 = clamp(dot(_e197, _e198), 0.0, 1.0);
    let _e201 = n_1;
    let _e202 = v;
    NdotV_1 = clamp(abs(dot(_e201, _e202)), 0.0, 1.0);
    let _e206 = n_1;
    let _e207 = h;
    NdotH = clamp(dot(_e206, _e207), 0.0, 1.0);
    let _e210 = l;
    let _e211 = h;
    LdotH = clamp(dot(_e210, _e211), 0.0, 1.0);
    let _e214 = v;
    let _e215 = h;
    VdotH = clamp(dot(_e214, _e215), 0.0, 1.0);
    let _e218 = NdotL_1;
    let _e219 = NdotV_1;
    let _e220 = NdotH;
    let _e221 = LdotH;
    let _e222 = VdotH;
    let _e223 = perceptualRoughness;
    let _e224 = metallic;
    let _e225 = specularEnvironmentR0_;
    let _e226 = specularEnvironmentR90_;
    let _e227 = alphaRoughness;
    let _e228 = diffuseColor;
    let _e229 = specularColor;
    pbrParam = PBRParam(_e218, _e219, _e220, _e221, _e222, _e223, _e224, _e225, _e226, _e227, _e228, _e229);
    specular = vec3<f32>(0.0, 0.0, 0.0);
    diffuse = vec3<f32>(0.0, 0.0, 0.0);
    let _e231 = NdotL_1;
    let _e233 = NdotV_1;
    if ((_e231 > 0.0) || (_e233 > 0.0)) {
        let _e236 = pbrParam;
        param_5 = _e236;
        let _e237 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
        D = _e237;
        let _e238 = pbrParam;
        param_6 = _e238;
        let _e239 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
        G = _e239;
        let _e240 = pbrParam;
        param_7 = _e240;
        let _e241 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
        F = _e241;
        let _e242 = D;
        let _e243 = G;
        let _e245 = F;
        let _e247 = specular;
        specular = (_e247 + (_e245 * (_e242 * _e243)));
        let _e249 = specular;
        specular = max(_e249, vec3<f32>(0.0, 0.0, 0.0));
        let _e251 = F;
        let _e254 = pbrParam;
        param_8 = _e254;
        let _e255 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_8));
        let _e257 = diffuse;
        diffuse = (_e257 + ((vec3<f32>(1.0) - _e251) * _e255));
        reflectColor = vec3<f32>(0.0, 0.0, 0.0);
        let _e260 = ubo.useCubeMap;
        if (_e260 != 0) {
            let _e263 = ubo.mipCount;
            mipCount = _e263;
            let _e264 = mipCount;
            let _e265 = perceptualRoughness;
            lod = (_e264 * _e265);
            let _e267 = v;
            let _e268 = n_1;
            let _e270 = lod;
            let _e271 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e267, _e268), _e270);
            param_9 = _e271;
            let _e272 = LINEARtoSRGBvf4_((&param_9));
            reflectColor = _e272.xyz;
        }
        let _e274 = NdotL_1;
        let _e275 = specular;
        let _e276 = diffuse;
        let _e279 = reflectColor;
        let _e280 = F;
        let _e282 = (((_e275 + _e276) * _e274) + (_e279 * _e280));
        col[0u] = _e282.x;
        col[1u] = _e282.y;
        col[2u] = _e282.z;
        let _e289 = specular;
        gi_diffuse = clamp(_e289, vec3<f32>(0.03999999910593033), vec3<f32>(1.0));
        let _e293 = gi_diffuse;
        let _e294 = diffuse;
        let _e296 = col;
        let _e298 = (_e296.xyz + (_e293 * _e294));
        col[0u] = _e298.x;
        col[1u] = _e298.y;
        col[2u] = _e298.z;
    }
    let _e306 = ubo.useOcclusionTexture;
    if (_e306 != 0) {
        let _e308 = f_Texcoord_1;
        let _e309 = textureSample(occlusionTexture, occlusionTextureSampler, _e308);
        ao = _e309.x;
        let _e311 = col;
        let _e313 = col;
        let _e315 = ao;
        let _e318 = ubo.occlusionStrength;
        let _e320 = mix(_e311.xyz, (_e313.xyz * _e315), vec3<f32>(_e318));
        col[0u] = _e320.x;
        col[1u] = _e320.y;
        col[2u] = _e320.z;
    }
    let _e328 = ubo.useEmissiveTexture;
    if (_e328 != 0) {
        let _e330 = f_Texcoord_1;
        let _e331 = textureSample(emissiveTexture, emissiveTextureSampler, _e330);
        param_10 = _e331;
        let _e332 = SRGBtoLINEARvf4_((&param_10));
        let _e335 = ubo.emissiveFactor;
        emissive = (_e332.xyz * _e335.xyz);
        let _e338 = emissive;
        let _e339 = col;
        let _e341 = (_e339.xyz + _e338);
        col[0u] = _e341.x;
        col[1u] = _e341.y;
        col[2u] = _e341.z;
    }
    let _e349 = ubo.useShadowMap;
    if (_e349 != 0) {
        let _e351 = f_LightSpacePos_1;
        let _e354 = f_LightSpacePos_1[3u];
        lsp_1 = (_e351.xyz / vec3<f32>(_e354));
        let _e357 = lsp_1;
        lsp_1 = ((_e357 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e362 = f_LightSpacePos_1[2u];
        let _e363 = (_e362 <= 0.0);
        phi_712_ = _e363;
        if !(_e363) {
            let _e366 = lsp_1[0u];
            let _e367 = (_e366 < 0.0);
            phi_711_ = _e367;
            if !(_e367) {
                let _e370 = lsp_1[1u];
                phi_711_ = (_e370 < 0.0);
            }
            let _e373 = phi_711_;
            phi_712_ = _e373;
        }
        let _e375 = phi_712_;
        phi_726_ = _e375;
        if !(_e375) {
            let _e378 = lsp_1[0u];
            let _e379 = (_e378 > 1.0);
            phi_725_ = _e379;
            if !(_e379) {
                let _e382 = lsp_1[1u];
                phi_725_ = (_e382 > 1.0);
            }
            let _e385 = phi_725_;
            phi_726_ = _e385;
        }
        let _e387 = phi_726_;
        outSide = _e387;
        let _e388 = outSide;
        if !(_e388) {
            let _e390 = lsp_1;
            param_11 = _e390;
            let _e391 = n_1;
            param_12 = _e391;
            let _e392 = l;
            param_13 = _e392;
            let _e393 = CalcShadowvf3vf3vf3_((&param_11), (&param_12), (&param_13));
            shadowCol = _e393;
        }
        let _e394 = shadowCol;
        let _e395 = col;
        let _e397 = (_e395.xyz * _e394);
        col[0u] = _e397.x;
        col[1u] = _e397.y;
        col[2u] = _e397.z;
    }
    let _e404 = col;
    let _e406 = pow(_e404.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e406.x;
    col[1u] = _e406.y;
    col[2u] = _e406.z;
    let _e414 = baseColor[3u];
    col[3u] = _e414;
    let _e416 = col;
    outColor = _e416;
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
