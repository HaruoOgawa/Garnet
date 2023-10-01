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
    var texelSize: vec2<f32>;

    moments = vec2<f32>(0.0, 0.0);
    let _e86 = ubo.ShadowMapX;
    let _e89 = ubo.ShadowMapY;
    texelSize = vec2<f32>((1.0 / _e86), (1.0 / _e89));
    let _e92 = (*uv);
    let _e93 = texelSize;
    let _e96 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e92 + (vec2<f32>(-1.0, -1.0) * _e93)));
    let _e98 = moments;
    moments = (_e98 + _e96.xy);
    let _e100 = (*uv);
    let _e101 = texelSize;
    let _e104 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e100 + (vec2<f32>(-1.0, 0.0) * _e101)));
    let _e106 = moments;
    moments = (_e106 + _e104.xy);
    let _e108 = (*uv);
    let _e109 = texelSize;
    let _e112 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e108 + (vec2<f32>(-1.0, 1.0) * _e109)));
    let _e114 = moments;
    moments = (_e114 + _e112.xy);
    let _e116 = (*uv);
    let _e117 = texelSize;
    let _e120 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e116 + (vec2<f32>(0.0, -1.0) * _e117)));
    let _e122 = moments;
    moments = (_e122 + _e120.xy);
    let _e124 = (*uv);
    let _e125 = texelSize;
    let _e128 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e124 + (vec2<f32>(0.0, 0.0) * _e125)));
    let _e130 = moments;
    moments = (_e130 + _e128.xy);
    let _e132 = (*uv);
    let _e133 = texelSize;
    let _e136 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e132 + (vec2<f32>(0.0, 1.0) * _e133)));
    let _e138 = moments;
    moments = (_e138 + _e136.xy);
    let _e140 = (*uv);
    let _e141 = texelSize;
    let _e144 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e140 + (vec2<f32>(1.0, -1.0) * _e141)));
    let _e146 = moments;
    moments = (_e146 + _e144.xy);
    let _e148 = (*uv);
    let _e149 = texelSize;
    let _e152 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e148 + (vec2<f32>(1.0, 0.0) * _e149)));
    let _e154 = moments;
    moments = (_e154 + _e152.xy);
    let _e156 = (*uv);
    let _e157 = texelSize;
    let _e160 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e156 + (vec2<f32>(1.0, 1.0) * _e157)));
    let _e162 = moments;
    moments = (_e162 + _e160.xy);
    let _e164 = moments;
    moments = (_e164 / vec2<f32>(9.0));
    let _e167 = moments;
    return _e167;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e92 = (*lsp);
    param = _e92.xy;
    let _e94 = ComputePCFvf2_((&param));
    moments_1 = _e94;
    let _e95 = (*nomral);
    let _e96 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e95, _e96))));
    let _e102 = (*lsp)[2u];
    let _e103 = ShadowBias;
    distance = (_e102 - _e103);
    let _e105 = distance;
    let _e107 = moments_1[0u];
    if (_e105 <= _e107) {
        return 1.0;
    }
    let _e110 = moments_1[1u];
    let _e112 = moments_1[0u];
    let _e114 = moments_1[0u];
    variance = (_e110 - (_e112 * _e114));
    let _e117 = variance;
    variance = max(0.004999999888241291, _e117);
    let _e119 = distance;
    let _e121 = moments_1[0u];
    d = (_e119 - _e121);
    let _e123 = variance;
    let _e124 = variance;
    let _e125 = d;
    let _e126 = d;
    p_max = (_e123 / (_e124 + (_e125 * _e126)));
    let _e130 = p_max;
    return _e130;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e83 = (*srgbIn);
    let _e85 = pow(_e83.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e87 = (*srgbIn)[3u];
    return vec4<f32>(_e85.x, _e85.y, _e85.z, _e87);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e83 = (*srgbIn_1);
    let _e85 = pow(_e83.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e87 = (*srgbIn_1)[3u];
    return vec4<f32>(_e85.x, _e85.y, _e85.z, _e87);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e84 = (*param_1).diffuseColor;
    return (_e84 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> vec3<f32> {
    let _e84 = (*param_2).reflectance0_;
    let _e86 = (*param_2).reflectance90_;
    let _e88 = (*param_2).reflectance0_;
    let _e91 = (*param_2).VdotH;
    return (_e84 + ((_e86 - _e88) * pow(clamp((1.0 - _e91), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e89 = (*param_3).NdotL;
    NdotL = _e89;
    let _e91 = (*param_3).NdotV;
    NdotV = _e91;
    let _e93 = (*param_3).alphaRoughness;
    r = _e93;
    let _e94 = NdotL;
    let _e96 = NdotL;
    let _e97 = r;
    let _e98 = r;
    let _e100 = r;
    let _e101 = r;
    let _e104 = NdotL;
    let _e105 = NdotL;
    attenuationL = ((2.0 * _e94) / (_e96 + sqrt(((_e97 * _e98) + ((1.0 - (_e100 * _e101)) * (_e104 * _e105))))));
    let _e112 = NdotV;
    let _e114 = NdotV;
    let _e115 = r;
    let _e116 = r;
    let _e118 = r;
    let _e119 = r;
    let _e122 = NdotV;
    let _e123 = NdotV;
    attenuationV = ((2.0 * _e112) / (_e114 + sqrt(((_e115 * _e116) + ((1.0 - (_e118 * _e119)) * (_e122 * _e123))))));
    let _e130 = attenuationL;
    let _e131 = attenuationV;
    return (_e130 * _e131);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_4: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e86 = (*param_4).alphaRoughness;
    let _e88 = (*param_4).alphaRoughness;
    roughness2_ = (_e86 * _e88);
    let _e91 = (*param_4).NdotH;
    let _e92 = roughness2_;
    let _e95 = (*param_4).NdotH;
    let _e98 = (*param_4).NdotH;
    f = ((((_e91 * _e92) - _e95) * _e98) + 1.0);
    let _e101 = roughness2_;
    let _e102 = f;
    let _e104 = f;
    return (_e101 / ((3.1415927410125732 * _e102) * _e104));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e88 = ubo.useNormalTexture;
    if (_e88 != 0) {
        let _e90 = f_WorldTangent_1;
        t = normalize(_e90);
        let _e92 = f_WorldBioTangent_1;
        b = normalize(_e92);
        let _e94 = f_WorldNormal_1;
        n = normalize(_e94);
        let _e96 = t;
        let _e97 = b;
        let _e98 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e96.x, _e96.y, _e96.z), vec3<f32>(_e97.x, _e97.y, _e97.z), vec3<f32>(_e98.x, _e98.y, _e98.z));
        let _e112 = f_Texcoord_1;
        let _e113 = textureSample(normalTexture, normalTextureSampler, _e112);
        nomral_1 = _e113.xyz;
        let _e115 = tbn;
        let _e116 = nomral_1;
        let _e121 = ubo.normalMapScale;
        let _e123 = ubo.normalMapScale;
        nomral_1 = normalize((_e115 * (((_e116 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e121, _e123, 1.0))));
    } else {
        let _e128 = f_WorldNormal_1;
        nomral_1 = _e128;
    }
    let _e129 = nomral_1;
    return _e129;
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
    var param_11: vec3<f32>;
    var param_12: vec3<f32>;
    var param_13: vec3<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e129 = ubo.roughnessFactor;
    perceptualRoughness = _e129;
    let _e131 = ubo.metallicFactor;
    metallic = _e131;
    let _e133 = ubo.useMetallicRoughnessTexture;
    if (_e133 != 0) {
        let _e135 = f_Texcoord_1;
        let _e136 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e135);
        metallicRoughnessColor = _e136;
        let _e137 = perceptualRoughness;
        let _e139 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e137 * _e139);
        let _e141 = metallic;
        let _e143 = metallicRoughnessColor[2u];
        metallic = (_e141 * _e143);
    }
    let _e145 = perceptualRoughness;
    perceptualRoughness = clamp(_e145, 0.03999999910593033, 1.0);
    let _e147 = metallic;
    metallic = clamp(_e147, 0.0, 1.0);
    let _e149 = perceptualRoughness;
    let _e150 = perceptualRoughness;
    alphaRoughness = (_e149 * _e150);
    let _e153 = ubo.useBaseColorTexture;
    if (_e153 != 0) {
        let _e155 = f_Texcoord_1;
        let _e156 = textureSample(baseColorTexture, baseColorTextureSampler, _e155);
        baseColor = _e156;
    } else {
        let _e158 = ubo.baseColorFactor;
        baseColor = _e158;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e159 = baseColor;
    let _e161 = f0_;
    diffuseColor = (_e159.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e161));
    let _e164 = metallic;
    let _e166 = diffuseColor;
    diffuseColor = (_e166 * (1.0 - _e164));
    let _e168 = f0_;
    let _e169 = baseColor;
    let _e171 = metallic;
    specularColor = mix(_e168, _e169.xyz, vec3<f32>(_e171));
    let _e175 = specularColor[0u];
    let _e177 = specularColor[1u];
    let _e180 = specularColor[2u];
    reflectance = max(max(_e175, _e177), _e180);
    let _e182 = reflectance;
    reflectance90_ = clamp((_e182 * 25.0), 0.0, 1.0);
    let _e185 = specularColor;
    specularEnvironmentR0_ = _e185;
    let _e186 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e186);
    let _e188 = getNormal();
    n_1 = _e188;
    let _e189 = f_WorldPos_1;
    let _e192 = ubo.cameraPos;
    v = (normalize((_e189.xyz - _e192.xyz)) * -1.0);
    let _e198 = ubo.lightDir;
    l = (normalize(_e198.xyz) * -1.0);
    let _e202 = v;
    let _e203 = l;
    h = normalize((_e202 + _e203));
    let _e206 = v;
    let _e207 = n_1;
    reflection = normalize(reflect(_e206, _e207));
    let _e210 = n_1;
    let _e211 = l;
    NdotL_1 = clamp(dot(_e210, _e211), 0.0, 1.0);
    let _e214 = n_1;
    let _e215 = v;
    NdotV_1 = clamp(abs(dot(_e214, _e215)), 0.0, 1.0);
    let _e219 = n_1;
    let _e220 = h;
    NdotH = clamp(dot(_e219, _e220), 0.0, 1.0);
    let _e223 = l;
    let _e224 = h;
    LdotH = clamp(dot(_e223, _e224), 0.0, 1.0);
    let _e227 = v;
    let _e228 = h;
    VdotH = clamp(dot(_e227, _e228), 0.0, 1.0);
    let _e231 = NdotL_1;
    let _e232 = NdotV_1;
    let _e233 = NdotH;
    let _e234 = LdotH;
    let _e235 = VdotH;
    let _e236 = perceptualRoughness;
    let _e237 = metallic;
    let _e238 = specularEnvironmentR0_;
    let _e239 = specularEnvironmentR90_;
    let _e240 = alphaRoughness;
    let _e241 = diffuseColor;
    let _e242 = specularColor;
    pbrParam = PBRParam(_e231, _e232, _e233, _e234, _e235, _e236, _e237, _e238, _e239, _e240, _e241, _e242);
    specular = vec3<f32>(0.0, 0.0, 0.0);
    diffuse = vec3<f32>(0.0, 0.0, 0.0);
    let _e244 = NdotL_1;
    let _e246 = NdotV_1;
    if ((_e244 > 0.0) || (_e246 > 0.0)) {
        let _e249 = pbrParam;
        param_5 = _e249;
        let _e250 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
        D = _e250;
        let _e251 = pbrParam;
        param_6 = _e251;
        let _e252 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
        G = _e252;
        let _e253 = pbrParam;
        param_7 = _e253;
        let _e254 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
        F = _e254;
        let _e255 = D;
        let _e256 = G;
        let _e258 = F;
        let _e260 = NdotL_1;
        let _e262 = NdotV_1;
        let _e266 = specular;
        specular = (_e266 + ((_e258 * (_e255 * _e256)) / vec3<f32>(((4.0 * _e260) * _e262))));
        let _e268 = specular;
        specular = max(_e268, vec3<f32>(0.0, 0.0, 0.0));
        let _e270 = F;
        let _e273 = pbrParam;
        param_8 = _e273;
        let _e274 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_8));
        let _e276 = diffuse;
        diffuse = (_e276 + ((vec3<f32>(1.0) - _e270) * _e274));
        reflectColor = vec3<f32>(0.0, 0.0, 0.0);
        let _e279 = ubo.useCubeMap;
        if (_e279 != 0) {
            let _e282 = ubo.mipCount;
            mipCount = _e282;
            let _e283 = mipCount;
            let _e284 = perceptualRoughness;
            lod = (_e283 * _e284);
            let _e286 = v;
            let _e287 = n_1;
            let _e289 = lod;
            let _e290 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e286, _e287), _e289);
            param_9 = _e290;
            let _e291 = LINEARtoSRGBvf4_((&param_9));
            reflectColor = _e291.xyz;
        }
        let _e293 = NdotL_1;
        let _e294 = specular;
        let _e295 = diffuse;
        let _e298 = reflectColor;
        let _e299 = F;
        let _e301 = (((_e294 + _e295) * _e293) + (_e298 * _e299));
        col[0u] = _e301.x;
        col[1u] = _e301.y;
        col[2u] = _e301.z;
        let _e308 = specular;
        gi_diffuse = clamp(_e308, vec3<f32>(0.03999999910593033), vec3<f32>(1.0));
        let _e312 = gi_diffuse;
        let _e313 = diffuse;
        let _e315 = col;
        let _e317 = (_e315.xyz + (_e312 * _e313));
        col[0u] = _e317.x;
        col[1u] = _e317.y;
        col[2u] = _e317.z;
    }
    let _e325 = ubo.useOcclusionTexture;
    if (_e325 != 0) {
        let _e327 = f_Texcoord_1;
        let _e328 = textureSample(occlusionTexture, occlusionTextureSampler, _e327);
        ao = _e328.x;
        let _e330 = col;
        let _e332 = col;
        let _e334 = ao;
        let _e337 = ubo.occlusionStrength;
        let _e339 = mix(_e330.xyz, (_e332.xyz * _e334), vec3<f32>(_e337));
        col[0u] = _e339.x;
        col[1u] = _e339.y;
        col[2u] = _e339.z;
    }
    let _e347 = ubo.useEmissiveTexture;
    if (_e347 != 0) {
        let _e349 = f_Texcoord_1;
        let _e350 = textureSample(emissiveTexture, emissiveTextureSampler, _e349);
        param_10 = _e350;
        let _e351 = SRGBtoLINEARvf4_((&param_10));
        let _e354 = ubo.emissiveFactor;
        emissive = (_e351.xyz * _e354.xyz);
        let _e357 = emissive;
        let _e358 = col;
        let _e360 = (_e358.xyz + _e357);
        col[0u] = _e360.x;
        col[1u] = _e360.y;
        col[2u] = _e360.z;
    }
    let _e368 = ubo.useShadowMap;
    if (_e368 != 0) {
        let _e370 = f_LightSpacePos_1;
        let _e373 = f_LightSpacePos_1[3u];
        lsp_1 = (_e370.xyz / vec3<f32>(_e373));
        let _e376 = lsp_1;
        lsp_1 = ((_e376 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e380 = lsp_1;
        param_11 = _e380;
        let _e381 = n_1;
        param_12 = _e381;
        let _e382 = l;
        param_13 = _e382;
        let _e383 = CalcShadowvf3vf3vf3_((&param_11), (&param_12), (&param_13));
        shadowCol = _e383;
        let _e384 = shadowCol;
        let _e385 = col;
        let _e387 = (_e385.xyz * _e384);
        col[0u] = _e387.x;
        col[1u] = _e387.y;
        col[2u] = _e387.z;
    }
    let _e394 = col;
    let _e396 = pow(_e394.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e396.x;
    col[1u] = _e396.y;
    col[2u] = _e396.z;
    let _e404 = baseColor[3u];
    col[3u] = _e404;
    let _e406 = col;
    outColor = _e406;
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
