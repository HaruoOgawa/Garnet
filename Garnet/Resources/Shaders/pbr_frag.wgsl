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
    let _e72 = (*uv);
    let _e73 = textureSample(shadowmapTexture, shadowmapTextureSampler, _e72);
    moments = _e73.xy;
    let _e75 = moments;
    return _e75;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e80 = (*lsp);
    param = _e80.xy;
    let _e82 = ComputePCFvf2_((&param));
    moments_1 = _e82;
    let _e83 = (*nomral);
    let _e84 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e83, _e84))));
    let _e90 = (*lsp)[2u];
    let _e91 = ShadowBias;
    distance = (_e90 - _e91);
    let _e93 = distance;
    let _e95 = moments_1[0u];
    if (_e93 <= _e95) {
        return 1.0;
    }
    let _e98 = moments_1[1u];
    let _e100 = moments_1[0u];
    let _e102 = moments_1[0u];
    variance = (_e98 - (_e100 * _e102));
    let _e105 = variance;
    variance = max(0.004999999888241291, _e105);
    let _e107 = distance;
    let _e109 = moments_1[0u];
    d = (_e107 - _e109);
    let _e111 = variance;
    let _e112 = variance;
    let _e113 = d;
    let _e114 = d;
    p_max = (_e111 / (_e112 + (_e113 * _e114)));
    let _e118 = p_max;
    return _e118;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e71 = (*srgbIn);
    let _e73 = pow(_e71.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e75 = (*srgbIn)[3u];
    return vec4<f32>(_e73.x, _e73.y, _e73.z, _e75);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e71 = (*srgbIn_1);
    let _e73 = pow(_e71.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e75 = (*srgbIn_1)[3u];
    return vec4<f32>(_e73.x, _e73.y, _e73.z, _e75);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e72 = (*param_1).diffuseColor;
    return (_e72 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> vec3<f32> {
    let _e72 = (*param_2).reflectance0_;
    let _e74 = (*param_2).reflectance90_;
    let _e76 = (*param_2).reflectance0_;
    let _e79 = (*param_2).VdotH;
    return (_e72 + ((_e74 - _e76) * pow(clamp((1.0 - _e79), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e77 = (*param_3).NdotL;
    NdotL = _e77;
    let _e79 = (*param_3).NdotV;
    NdotV = _e79;
    let _e81 = (*param_3).alphaRoughness;
    r = _e81;
    let _e82 = NdotL;
    let _e84 = NdotL;
    let _e85 = r;
    let _e86 = r;
    let _e88 = r;
    let _e89 = r;
    let _e92 = NdotL;
    let _e93 = NdotL;
    attenuationL = ((2.0 * _e82) / (_e84 + sqrt(((_e85 * _e86) + ((1.0 - (_e88 * _e89)) * (_e92 * _e93))))));
    let _e100 = NdotV;
    let _e102 = NdotV;
    let _e103 = r;
    let _e104 = r;
    let _e106 = r;
    let _e107 = r;
    let _e110 = NdotV;
    let _e111 = NdotV;
    attenuationV = ((2.0 * _e100) / (_e102 + sqrt(((_e103 * _e104) + ((1.0 - (_e106 * _e107)) * (_e110 * _e111))))));
    let _e118 = attenuationL;
    let _e119 = attenuationV;
    return (_e118 * _e119);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_4: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e74 = (*param_4).alphaRoughness;
    let _e76 = (*param_4).alphaRoughness;
    roughness2_ = (_e74 * _e76);
    let _e79 = (*param_4).NdotH;
    let _e80 = roughness2_;
    let _e83 = (*param_4).NdotH;
    let _e86 = (*param_4).NdotH;
    f = ((((_e79 * _e80) - _e83) * _e86) + 1.0);
    let _e89 = roughness2_;
    let _e90 = f;
    let _e92 = f;
    return (_e89 / ((3.1415927410125732 * _e90) * _e92));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e76 = ubo.useNormalTexture;
    if (_e76 != 0) {
        let _e78 = f_WorldTangent_1;
        t = normalize(_e78);
        let _e80 = f_WorldBioTangent_1;
        b = normalize(_e80);
        let _e82 = f_WorldNormal_1;
        n = normalize(_e82);
        let _e84 = t;
        let _e85 = b;
        let _e86 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e84.x, _e84.y, _e84.z), vec3<f32>(_e85.x, _e85.y, _e85.z), vec3<f32>(_e86.x, _e86.y, _e86.z));
        let _e100 = f_Texcoord_1;
        let _e101 = textureSample(normalTexture, normalTextureSampler, _e100);
        nomral_1 = _e101.xyz;
        let _e103 = tbn;
        let _e104 = nomral_1;
        let _e109 = ubo.normalMapScale;
        let _e111 = ubo.normalMapScale;
        nomral_1 = normalize((_e103 * (((_e104 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e109, _e111, 1.0))));
    } else {
        let _e116 = f_WorldNormal_1;
        nomral_1 = _e116;
    }
    let _e117 = nomral_1;
    return _e117;
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
    var phi_718_: bool;
    var phi_719_: bool;
    var phi_732_: bool;
    var phi_733_: bool;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e118 = ubo.roughnessFactor;
    perceptualRoughness = _e118;
    let _e120 = ubo.metallicFactor;
    metallic = _e120;
    let _e122 = ubo.useMetallicRoughnessTexture;
    if (_e122 != 0) {
        let _e124 = f_Texcoord_1;
        let _e125 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e124);
        metallicRoughnessColor = _e125;
        let _e126 = perceptualRoughness;
        let _e128 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e126 * _e128);
        let _e130 = metallic;
        let _e132 = metallicRoughnessColor[2u];
        metallic = (_e130 * _e132);
    }
    let _e134 = perceptualRoughness;
    perceptualRoughness = clamp(_e134, 0.03999999910593033, 1.0);
    let _e136 = metallic;
    metallic = clamp(_e136, 0.0, 1.0);
    let _e138 = perceptualRoughness;
    let _e139 = perceptualRoughness;
    alphaRoughness = (_e138 * _e139);
    let _e142 = ubo.useBaseColorTexture;
    if (_e142 != 0) {
        let _e144 = f_Texcoord_1;
        let _e145 = textureSample(baseColorTexture, baseColorTextureSampler, _e144);
        baseColor = _e145;
    } else {
        let _e147 = ubo.baseColorFactor;
        baseColor = _e147;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e148 = baseColor;
    let _e150 = f0_;
    diffuseColor = (_e148.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e150));
    let _e153 = metallic;
    let _e155 = diffuseColor;
    diffuseColor = (_e155 * (1.0 - _e153));
    let _e157 = f0_;
    let _e158 = baseColor;
    let _e160 = metallic;
    specularColor = mix(_e157, _e158.xyz, vec3<f32>(_e160));
    let _e164 = specularColor[0u];
    let _e166 = specularColor[1u];
    let _e169 = specularColor[2u];
    reflectance = max(max(_e164, _e166), _e169);
    let _e171 = reflectance;
    reflectance90_ = clamp((_e171 * 25.0), 0.0, 1.0);
    let _e174 = specularColor;
    specularEnvironmentR0_ = _e174;
    let _e175 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e175);
    let _e177 = getNormal();
    n_1 = _e177;
    let _e179 = ubo.cameraPos;
    let _e181 = f_WorldPos_1;
    v = normalize((_e179.xyz - _e181.xyz));
    let _e186 = ubo.lightDir;
    l = normalize(_e186.xyz);
    let _e189 = v;
    let _e190 = l;
    h = normalize((_e189 + _e190));
    let _e193 = v;
    let _e194 = n_1;
    reflection = -(normalize(reflect(_e193, _e194)));
    let _e198 = n_1;
    let _e199 = l;
    NdotL_1 = clamp(dot(_e198, _e199), 0.0, 1.0);
    let _e202 = n_1;
    let _e203 = v;
    NdotV_1 = clamp(abs(dot(_e202, _e203)), 0.0, 1.0);
    let _e207 = n_1;
    let _e208 = h;
    NdotH = clamp(dot(_e207, _e208), 0.0, 1.0);
    let _e211 = l;
    let _e212 = h;
    LdotH = clamp(dot(_e211, _e212), 0.0, 1.0);
    let _e215 = v;
    let _e216 = h;
    VdotH = clamp(dot(_e215, _e216), 0.0, 1.0);
    let _e219 = NdotL_1;
    let _e220 = NdotV_1;
    let _e221 = NdotH;
    let _e222 = LdotH;
    let _e223 = VdotH;
    let _e224 = perceptualRoughness;
    let _e225 = metallic;
    let _e226 = specularEnvironmentR0_;
    let _e227 = specularEnvironmentR90_;
    let _e228 = alphaRoughness;
    let _e229 = diffuseColor;
    let _e230 = specularColor;
    pbrParam = PBRParam(_e219, _e220, _e221, _e222, _e223, _e224, _e225, _e226, _e227, _e228, _e229, _e230);
    specular = vec3<f32>(0.0, 0.0, 0.0);
    diffuse = vec3<f32>(0.0, 0.0, 0.0);
    let _e232 = NdotL_1;
    let _e234 = NdotV_1;
    if ((_e232 > 0.0) || (_e234 > 0.0)) {
        let _e237 = pbrParam;
        param_5 = _e237;
        let _e238 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
        D = _e238;
        let _e239 = pbrParam;
        param_6 = _e239;
        let _e240 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
        G = _e240;
        let _e241 = pbrParam;
        param_7 = _e241;
        let _e242 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
        F = _e242;
        let _e243 = D;
        let _e244 = G;
        let _e246 = F;
        let _e248 = NdotL_1;
        let _e250 = NdotV_1;
        let _e254 = specular;
        specular = (_e254 + ((_e246 * (_e243 * _e244)) / vec3<f32>(((4.0 * _e248) * _e250))));
        let _e256 = specular;
        specular = max(_e256, vec3<f32>(0.0, 0.0, 0.0));
        let _e258 = F;
        let _e261 = pbrParam;
        param_8 = _e261;
        let _e262 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_8));
        let _e264 = diffuse;
        diffuse = (_e264 + ((vec3<f32>(1.0) - _e258) * _e262));
        reflectColor = vec3<f32>(0.0, 0.0, 0.0);
        let _e267 = ubo.useCubeMap;
        if (_e267 != 0) {
            let _e270 = ubo.mipCount;
            mipCount = _e270;
            let _e271 = mipCount;
            let _e272 = perceptualRoughness;
            lod = (_e271 * _e272);
            let _e274 = v;
            let _e275 = n_1;
            let _e277 = lod;
            let _e278 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e274, _e275), _e277);
            param_9 = _e278;
            let _e279 = LINEARtoSRGBvf4_((&param_9));
            reflectColor = _e279.xyz;
        }
        let _e281 = NdotL_1;
        let _e282 = specular;
        let _e283 = diffuse;
        let _e286 = reflectColor;
        let _e287 = F;
        let _e289 = (((_e282 + _e283) * _e281) + (_e286 * _e287));
        col[0u] = _e289.x;
        col[1u] = _e289.y;
        col[2u] = _e289.z;
        let _e296 = specular;
        gi_diffuse = clamp(_e296, vec3<f32>(0.03999999910593033), vec3<f32>(1.0));
        let _e300 = gi_diffuse;
        let _e301 = diffuse;
        let _e303 = col;
        let _e305 = (_e303.xyz + (_e300 * _e301));
        col[0u] = _e305.x;
        col[1u] = _e305.y;
        col[2u] = _e305.z;
    }
    let _e313 = ubo.useOcclusionTexture;
    if (_e313 != 0) {
        let _e315 = f_Texcoord_1;
        let _e316 = textureSample(occlusionTexture, occlusionTextureSampler, _e315);
        ao = _e316.x;
        let _e318 = col;
        let _e320 = col;
        let _e322 = ao;
        let _e325 = ubo.occlusionStrength;
        let _e327 = mix(_e318.xyz, (_e320.xyz * _e322), vec3<f32>(_e325));
        col[0u] = _e327.x;
        col[1u] = _e327.y;
        col[2u] = _e327.z;
    }
    let _e335 = ubo.useEmissiveTexture;
    if (_e335 != 0) {
        let _e337 = f_Texcoord_1;
        let _e338 = textureSample(emissiveTexture, emissiveTextureSampler, _e337);
        param_10 = _e338;
        let _e339 = SRGBtoLINEARvf4_((&param_10));
        let _e342 = ubo.emissiveFactor;
        emissive = (_e339.xyz * _e342.xyz);
        let _e345 = emissive;
        let _e346 = col;
        let _e348 = (_e346.xyz + _e345);
        col[0u] = _e348.x;
        col[1u] = _e348.y;
        col[2u] = _e348.z;
    }
    let _e356 = ubo.useShadowMap;
    if (_e356 != 0) {
        let _e358 = f_LightSpacePos_1;
        let _e361 = f_LightSpacePos_1[3u];
        lsp_1 = (_e358.xyz / vec3<f32>(_e361));
        let _e364 = lsp_1;
        lsp_1 = ((_e364 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e369 = f_LightSpacePos_1[2u];
        let _e370 = (_e369 <= 0.0);
        phi_719_ = _e370;
        if !(_e370) {
            let _e373 = lsp_1[0u];
            let _e374 = (_e373 < 0.0);
            phi_718_ = _e374;
            if !(_e374) {
                let _e377 = lsp_1[1u];
                phi_718_ = (_e377 < 0.0);
            }
            let _e380 = phi_718_;
            phi_719_ = _e380;
        }
        let _e382 = phi_719_;
        phi_733_ = _e382;
        if !(_e382) {
            let _e385 = lsp_1[0u];
            let _e386 = (_e385 > 1.0);
            phi_732_ = _e386;
            if !(_e386) {
                let _e389 = lsp_1[1u];
                phi_732_ = (_e389 > 1.0);
            }
            let _e392 = phi_732_;
            phi_733_ = _e392;
        }
        let _e394 = phi_733_;
        outSide = _e394;
        let _e395 = outSide;
        if !(_e395) {
            let _e397 = lsp_1;
            param_11 = _e397;
            let _e398 = n_1;
            param_12 = _e398;
            let _e399 = l;
            param_13 = _e399;
            let _e400 = CalcShadowvf3vf3vf3_((&param_11), (&param_12), (&param_13));
            shadowCol = _e400;
        }
        let _e401 = shadowCol;
        let _e402 = col;
        let _e404 = (_e402.xyz * _e401);
        col[0u] = _e404.x;
        col[1u] = _e404.y;
        col[2u] = _e404.z;
    }
    let _e411 = col;
    let _e413 = pow(_e411.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e413.x;
    col[1u] = _e413.y;
    col[2u] = _e413.z;
    let _e421 = baseColor[3u];
    col[3u] = _e421;
    let _e423 = col;
    outColor = _e423;
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
