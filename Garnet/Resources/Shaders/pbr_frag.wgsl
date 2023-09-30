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
    let _e73 = (*uv);
    let _e74 = textureSample(shadowmapTexture, shadowmapTextureSampler, _e73);
    moments = _e74.xy;
    let _e76 = moments;
    return _e76;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e81 = (*lsp);
    param = _e81.xy;
    let _e83 = ComputePCFvf2_((&param));
    moments_1 = _e83;
    let _e84 = (*nomral);
    let _e85 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e84, _e85))));
    let _e91 = (*lsp)[2u];
    let _e92 = ShadowBias;
    distance = (_e91 - _e92);
    let _e94 = distance;
    let _e96 = moments_1[0u];
    if (_e94 <= _e96) {
        return 1.0;
    }
    let _e99 = moments_1[1u];
    let _e101 = moments_1[0u];
    let _e103 = moments_1[0u];
    variance = (_e99 - (_e101 * _e103));
    let _e106 = variance;
    variance = max(0.004999999888241291, _e106);
    let _e108 = distance;
    let _e110 = moments_1[0u];
    d = (_e108 - _e110);
    let _e112 = variance;
    let _e113 = variance;
    let _e114 = d;
    let _e115 = d;
    p_max = (_e112 / (_e113 + (_e114 * _e115)));
    let _e119 = p_max;
    return _e119;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e72 = (*srgbIn);
    let _e74 = pow(_e72.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e76 = (*srgbIn)[3u];
    return vec4<f32>(_e74.x, _e74.y, _e74.z, _e76);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e72 = (*srgbIn_1);
    let _e74 = pow(_e72.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e76 = (*srgbIn_1)[3u];
    return vec4<f32>(_e74.x, _e74.y, _e74.z, _e76);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e73 = (*param_1).diffuseColor;
    return (_e73 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> vec3<f32> {
    let _e73 = (*param_2).reflectance0_;
    let _e75 = (*param_2).reflectance90_;
    let _e77 = (*param_2).reflectance0_;
    let _e80 = (*param_2).VdotH;
    return (_e73 + ((_e75 - _e77) * pow(clamp((1.0 - _e80), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e78 = (*param_3).NdotL;
    NdotL = _e78;
    let _e80 = (*param_3).NdotV;
    NdotV = _e80;
    let _e82 = (*param_3).alphaRoughness;
    r = _e82;
    let _e83 = NdotL;
    let _e85 = NdotL;
    let _e86 = r;
    let _e87 = r;
    let _e89 = r;
    let _e90 = r;
    let _e93 = NdotL;
    let _e94 = NdotL;
    attenuationL = ((2.0 * _e83) / (_e85 + sqrt(((_e86 * _e87) + ((1.0 - (_e89 * _e90)) * (_e93 * _e94))))));
    let _e101 = NdotV;
    let _e103 = NdotV;
    let _e104 = r;
    let _e105 = r;
    let _e107 = r;
    let _e108 = r;
    let _e111 = NdotV;
    let _e112 = NdotV;
    attenuationV = ((2.0 * _e101) / (_e103 + sqrt(((_e104 * _e105) + ((1.0 - (_e107 * _e108)) * (_e111 * _e112))))));
    let _e119 = attenuationL;
    let _e120 = attenuationV;
    return (_e119 * _e120);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_4: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e75 = (*param_4).alphaRoughness;
    let _e77 = (*param_4).alphaRoughness;
    roughness2_ = (_e75 * _e77);
    let _e80 = (*param_4).NdotH;
    let _e81 = roughness2_;
    let _e84 = (*param_4).NdotH;
    let _e87 = (*param_4).NdotH;
    f = ((((_e80 * _e81) - _e84) * _e87) + 1.0);
    let _e90 = roughness2_;
    let _e91 = f;
    let _e93 = f;
    return (_e90 / ((3.1415927410125732 * _e91) * _e93));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e77 = ubo.useNormalTexture;
    if (_e77 != 0) {
        let _e79 = f_WorldTangent_1;
        t = normalize(_e79);
        let _e81 = f_WorldBioTangent_1;
        b = normalize(_e81);
        let _e83 = f_WorldNormal_1;
        n = normalize(_e83);
        let _e85 = t;
        let _e86 = b;
        let _e87 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e85.x, _e85.y, _e85.z), vec3<f32>(_e86.x, _e86.y, _e86.z), vec3<f32>(_e87.x, _e87.y, _e87.z));
        let _e101 = f_Texcoord_1;
        let _e102 = textureSample(normalTexture, normalTextureSampler, _e101);
        nomral_1 = _e102.xyz;
        let _e104 = tbn;
        let _e105 = nomral_1;
        let _e110 = ubo.normalMapScale;
        let _e112 = ubo.normalMapScale;
        nomral_1 = normalize((_e104 * (((_e105 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e110, _e112, 1.0))));
    } else {
        let _e117 = f_WorldNormal_1;
        nomral_1 = _e117;
    }
    let _e118 = nomral_1;
    return _e118;
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
    var phi_720_: bool;
    var phi_721_: bool;
    var phi_734_: bool;
    var phi_735_: bool;

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
    let _e179 = f_WorldPos_1;
    let _e182 = ubo.cameraPos;
    v = (normalize((_e179.xyz - _e182.xyz)) * -1.0);
    let _e188 = ubo.lightDir;
    l = (normalize(_e188.xyz) * -1.0);
    let _e192 = v;
    let _e193 = l;
    h = normalize((_e192 + _e193));
    let _e196 = v;
    let _e197 = n_1;
    reflection = normalize(reflect(_e196, _e197));
    let _e200 = n_1;
    let _e201 = l;
    NdotL_1 = clamp(dot(_e200, _e201), 0.0, 1.0);
    let _e204 = n_1;
    let _e205 = v;
    NdotV_1 = clamp(abs(dot(_e204, _e205)), 0.0, 1.0);
    let _e209 = n_1;
    let _e210 = h;
    NdotH = clamp(dot(_e209, _e210), 0.0, 1.0);
    let _e213 = l;
    let _e214 = h;
    LdotH = clamp(dot(_e213, _e214), 0.0, 1.0);
    let _e217 = v;
    let _e218 = h;
    VdotH = clamp(dot(_e217, _e218), 0.0, 1.0);
    let _e221 = NdotL_1;
    let _e222 = NdotV_1;
    let _e223 = NdotH;
    let _e224 = LdotH;
    let _e225 = VdotH;
    let _e226 = perceptualRoughness;
    let _e227 = metallic;
    let _e228 = specularEnvironmentR0_;
    let _e229 = specularEnvironmentR90_;
    let _e230 = alphaRoughness;
    let _e231 = diffuseColor;
    let _e232 = specularColor;
    pbrParam = PBRParam(_e221, _e222, _e223, _e224, _e225, _e226, _e227, _e228, _e229, _e230, _e231, _e232);
    specular = vec3<f32>(0.0, 0.0, 0.0);
    diffuse = vec3<f32>(0.0, 0.0, 0.0);
    let _e234 = NdotL_1;
    let _e236 = NdotV_1;
    if ((_e234 > 0.0) || (_e236 > 0.0)) {
        let _e239 = pbrParam;
        param_5 = _e239;
        let _e240 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
        D = _e240;
        let _e241 = pbrParam;
        param_6 = _e241;
        let _e242 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
        G = _e242;
        let _e243 = pbrParam;
        param_7 = _e243;
        let _e244 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
        F = _e244;
        let _e245 = D;
        let _e246 = G;
        let _e248 = F;
        let _e250 = NdotL_1;
        let _e252 = NdotV_1;
        let _e256 = specular;
        specular = (_e256 + ((_e248 * (_e245 * _e246)) / vec3<f32>(((4.0 * _e250) * _e252))));
        let _e258 = specular;
        specular = max(_e258, vec3<f32>(0.0, 0.0, 0.0));
        let _e260 = F;
        let _e263 = pbrParam;
        param_8 = _e263;
        let _e264 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_8));
        let _e266 = diffuse;
        diffuse = (_e266 + ((vec3<f32>(1.0) - _e260) * _e264));
        reflectColor = vec3<f32>(0.0, 0.0, 0.0);
        let _e269 = ubo.useCubeMap;
        if (_e269 != 0) {
            let _e272 = ubo.mipCount;
            mipCount = _e272;
            let _e273 = mipCount;
            let _e274 = perceptualRoughness;
            lod = (_e273 * _e274);
            let _e276 = v;
            let _e277 = n_1;
            let _e279 = lod;
            let _e280 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e276, _e277), _e279);
            param_9 = _e280;
            let _e281 = LINEARtoSRGBvf4_((&param_9));
            reflectColor = _e281.xyz;
        }
        let _e283 = NdotL_1;
        let _e284 = specular;
        let _e285 = diffuse;
        let _e288 = reflectColor;
        let _e289 = F;
        let _e291 = (((_e284 + _e285) * _e283) + (_e288 * _e289));
        col[0u] = _e291.x;
        col[1u] = _e291.y;
        col[2u] = _e291.z;
        let _e298 = specular;
        gi_diffuse = clamp(_e298, vec3<f32>(0.03999999910593033), vec3<f32>(1.0));
        let _e302 = gi_diffuse;
        let _e303 = diffuse;
        let _e305 = col;
        let _e307 = (_e305.xyz + (_e302 * _e303));
        col[0u] = _e307.x;
        col[1u] = _e307.y;
        col[2u] = _e307.z;
    }
    let _e315 = ubo.useOcclusionTexture;
    if (_e315 != 0) {
        let _e317 = f_Texcoord_1;
        let _e318 = textureSample(occlusionTexture, occlusionTextureSampler, _e317);
        ao = _e318.x;
        let _e320 = col;
        let _e322 = col;
        let _e324 = ao;
        let _e327 = ubo.occlusionStrength;
        let _e329 = mix(_e320.xyz, (_e322.xyz * _e324), vec3<f32>(_e327));
        col[0u] = _e329.x;
        col[1u] = _e329.y;
        col[2u] = _e329.z;
    }
    let _e337 = ubo.useEmissiveTexture;
    if (_e337 != 0) {
        let _e339 = f_Texcoord_1;
        let _e340 = textureSample(emissiveTexture, emissiveTextureSampler, _e339);
        param_10 = _e340;
        let _e341 = SRGBtoLINEARvf4_((&param_10));
        let _e344 = ubo.emissiveFactor;
        emissive = (_e341.xyz * _e344.xyz);
        let _e347 = emissive;
        let _e348 = col;
        let _e350 = (_e348.xyz + _e347);
        col[0u] = _e350.x;
        col[1u] = _e350.y;
        col[2u] = _e350.z;
    }
    let _e358 = ubo.useShadowMap;
    if (_e358 != 0) {
        let _e360 = f_LightSpacePos_1;
        let _e363 = f_LightSpacePos_1[3u];
        lsp_1 = (_e360.xyz / vec3<f32>(_e363));
        let _e366 = lsp_1;
        lsp_1 = ((_e366 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e371 = f_LightSpacePos_1[2u];
        let _e372 = (_e371 <= 0.0);
        phi_721_ = _e372;
        if !(_e372) {
            let _e375 = lsp_1[0u];
            let _e376 = (_e375 < 0.0);
            phi_720_ = _e376;
            if !(_e376) {
                let _e379 = lsp_1[1u];
                phi_720_ = (_e379 < 0.0);
            }
            let _e382 = phi_720_;
            phi_721_ = _e382;
        }
        let _e384 = phi_721_;
        phi_735_ = _e384;
        if !(_e384) {
            let _e387 = lsp_1[0u];
            let _e388 = (_e387 > 1.0);
            phi_734_ = _e388;
            if !(_e388) {
                let _e391 = lsp_1[1u];
                phi_734_ = (_e391 > 1.0);
            }
            let _e394 = phi_734_;
            phi_735_ = _e394;
        }
        let _e396 = phi_735_;
        outSide = _e396;
        let _e397 = outSide;
        if !(_e397) {
            let _e399 = lsp_1;
            param_11 = _e399;
            let _e400 = n_1;
            param_12 = _e400;
            let _e401 = l;
            param_13 = _e401;
            let _e402 = CalcShadowvf3vf3vf3_((&param_11), (&param_12), (&param_13));
            shadowCol = _e402;
        }
        let _e403 = shadowCol;
        let _e404 = col;
        let _e406 = (_e404.xyz * _e403);
        col[0u] = _e406.x;
        col[1u] = _e406.y;
        col[2u] = _e406.z;
    }
    let _e413 = col;
    let _e415 = pow(_e413.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e415.x;
    col[1u] = _e415.y;
    col[2u] = _e415.z;
    let _e423 = baseColor[3u];
    col[3u] = _e423;
    let _e425 = col;
    outColor = _e425;
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
