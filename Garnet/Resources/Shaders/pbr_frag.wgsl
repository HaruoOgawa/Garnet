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
    NdotL_1 = clamp(dot(_e196, _e197), 0.0, 1.0);
    let _e200 = n_1;
    let _e201 = v;
    NdotV_1 = clamp(abs(dot(_e200, _e201)), 0.0, 1.0);
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
    specular = vec3<f32>(0.0, 0.0, 0.0);
    diffuse = vec3<f32>(0.0, 0.0, 0.0);
    let _e230 = NdotL_1;
    let _e232 = NdotV_1;
    if ((_e230 > 0.0) || (_e232 > 0.0)) {
        let _e235 = pbrParam;
        param_5 = _e235;
        let _e236 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
        D = _e236;
        let _e237 = pbrParam;
        param_6 = _e237;
        let _e238 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
        G = _e238;
        let _e239 = pbrParam;
        param_7 = _e239;
        let _e240 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
        F = _e240;
        let _e241 = NdotL_1;
        let _e242 = D;
        let _e244 = G;
        let _e246 = F;
        let _e248 = specular;
        specular = (_e248 + (_e246 * ((_e241 * _e242) * _e244)));
        let _e250 = NdotL_1;
        let _e251 = F;
        let _e255 = pbrParam;
        param_8 = _e255;
        let _e256 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_8));
        let _e258 = diffuse;
        diffuse = (_e258 + (((vec3<f32>(1.0) - _e251) * _e250) * _e256));
        reflectColor = vec3<f32>(1.0, 1.0, 1.0);
        let _e261 = ubo.useCubeMap;
        if (_e261 != 0) {
            let _e264 = ubo.mipCount;
            mipCount = _e264;
            let _e265 = mipCount;
            let _e266 = perceptualRoughness;
            lod = (_e265 * _e266);
            let _e268 = v;
            let _e269 = n_1;
            let _e271 = lod;
            let _e272 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e268, _e269), _e271);
            param_9 = _e272;
            let _e273 = LINEARtoSRGBvf4_((&param_9));
            reflectColor = _e273.xyz;
        }
        let _e275 = specular;
        let _e276 = diffuse;
        let _e278 = reflectColor;
        let _e279 = F;
        let _e281 = ((_e275 + _e276) + (_e278 * _e279));
        col[0u] = _e281.x;
        col[1u] = _e281.y;
        col[2u] = _e281.z;
    }
    let _e289 = ubo.useOcclusionTexture;
    if (_e289 != 0) {
        let _e291 = f_Texcoord_1;
        let _e292 = textureSample(occlusionTexture, occlusionTextureSampler, _e291);
        ao = _e292.x;
        let _e294 = col;
        let _e296 = col;
        let _e298 = ao;
        let _e301 = ubo.occlusionStrength;
        let _e303 = mix(_e294.xyz, (_e296.xyz * _e298), vec3<f32>(_e301));
        col[0u] = _e303.x;
        col[1u] = _e303.y;
        col[2u] = _e303.z;
    }
    let _e311 = ubo.useEmissiveTexture;
    if (_e311 != 0) {
        let _e313 = f_Texcoord_1;
        let _e314 = textureSample(emissiveTexture, emissiveTextureSampler, _e313);
        param_10 = _e314;
        let _e315 = SRGBtoLINEARvf4_((&param_10));
        let _e318 = ubo.emissiveFactor;
        emissive = (_e315.xyz * _e318.xyz);
        let _e321 = emissive;
        let _e322 = col;
        let _e324 = (_e322.xyz + _e321);
        col[0u] = _e324.x;
        col[1u] = _e324.y;
        col[2u] = _e324.z;
    }
    let _e332 = ubo.useShadowMap;
    if (_e332 != 0) {
        let _e334 = f_LightSpacePos_1;
        let _e337 = f_LightSpacePos_1[3u];
        lsp_1 = (_e334.xyz / vec3<f32>(_e337));
        let _e340 = lsp_1;
        lsp_1 = ((_e340 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e345 = f_LightSpacePos_1[2u];
        let _e346 = (_e345 <= 0.0);
        phi_695_ = _e346;
        if !(_e346) {
            let _e349 = lsp_1[0u];
            let _e350 = (_e349 < 0.0);
            phi_694_ = _e350;
            if !(_e350) {
                let _e353 = lsp_1[1u];
                phi_694_ = (_e353 < 0.0);
            }
            let _e356 = phi_694_;
            phi_695_ = _e356;
        }
        let _e358 = phi_695_;
        phi_709_ = _e358;
        if !(_e358) {
            let _e361 = lsp_1[0u];
            let _e362 = (_e361 > 1.0);
            phi_708_ = _e362;
            if !(_e362) {
                let _e365 = lsp_1[1u];
                phi_708_ = (_e365 > 1.0);
            }
            let _e368 = phi_708_;
            phi_709_ = _e368;
        }
        let _e370 = phi_709_;
        outSide = _e370;
        let _e371 = outSide;
        if !(_e371) {
            let _e373 = lsp_1;
            param_11 = _e373;
            let _e374 = n_1;
            param_12 = _e374;
            let _e375 = l;
            param_13 = _e375;
            let _e376 = CalcShadowvf3vf3vf3_((&param_11), (&param_12), (&param_13));
            shadowCol = _e376;
        }
        let _e377 = shadowCol;
        let _e378 = col;
        let _e380 = (_e378.xyz * _e377);
        col[0u] = _e380.x;
        col[1u] = _e380.y;
        col[2u] = _e380.z;
    }
    let _e387 = col;
    let _e389 = pow(_e387.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e389.x;
    col[1u] = _e389.y;
    col[2u] = _e389.z;
    let _e397 = baseColor[3u];
    col[3u] = _e397;
    let _e399 = col;
    outColor = _e399;
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
