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
    useIBL: i32,
    useSkinMeshAnimation: i32,
    pad0_: i32,
    pad1_: i32,
    pad2_: i32,
}

@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> f_WorldTangent_1: vec3<f32>;
var<private> f_WorldBioTangent_1: vec3<f32>;
var<private> f_WorldNormal_1: vec3<f32>;
@group(0) @binding(8) 
var normalTexture: texture_2d<f32>;
@group(0) @binding(9) 
var normalTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
@group(0) @binding(14) 
var shadowmapTexture: texture_2d<f32>;
@group(0) @binding(15) 
var shadowmapTextureSampler: sampler;
@group(0) @binding(12) 
var cubemapTexture: texture_cube<f32>;
@group(0) @binding(13) 
var cubemapTextureSampler: sampler;
@group(0) @binding(20) 
var IBL_GGXLUT_Texture: texture_2d<f32>;
@group(0) @binding(21) 
var IBL_GGXLUT_TextureSampler: sampler;
@group(0) @binding(16) 
var IBL_Diffuse_Texture: texture_2d<f32>;
@group(0) @binding(17) 
var IBL_Diffuse_TextureSampler: sampler;
@group(0) @binding(18) 
var IBL_Specular_Texture: texture_2d<f32>;
@group(0) @binding(19) 
var IBL_Specular_TextureSampler: sampler;
@group(0) @binding(4) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(5) 
var metallicRoughnessTextureSampler: sampler;
@group(0) @binding(2) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(3) 
var baseColorTextureSampler: sampler;
var<private> f_WorldPos_1: vec4<f32>;
@group(0) @binding(10) 
var occlusionTexture: texture_2d<f32>;
@group(0) @binding(11) 
var occlusionTextureSampler: sampler;
@group(0) @binding(6) 
var emissiveTexture: texture_2d<f32>;
@group(0) @binding(7) 
var emissiveTextureSampler: sampler;
var<private> f_LightSpacePos_1: vec4<f32>;
var<private> f_DebugColor_1: vec4<f32>;
var<private> outColor: vec4<f32>;
var<private> f_DebugJoint_1: vec4<i32>;

fn ComputePCFvf2_(uv: ptr<function, vec2<f32>>) -> vec2<f32> {
    var moments: vec2<f32>;
    var texelSize: vec2<f32>;

    moments = vec2<f32>(0.0, 0.0);
    let _e97 = ubo.ShadowMapX;
    let _e100 = ubo.ShadowMapY;
    texelSize = vec2<f32>((1.0 / _e97), (1.0 / _e100));
    let _e103 = (*uv);
    let _e104 = texelSize;
    let _e107 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e103 + (vec2<f32>(-1.0, -1.0) * _e104)));
    let _e109 = moments;
    moments = (_e109 + _e107.xy);
    let _e111 = (*uv);
    let _e112 = texelSize;
    let _e115 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e111 + (vec2<f32>(-1.0, 0.0) * _e112)));
    let _e117 = moments;
    moments = (_e117 + _e115.xy);
    let _e119 = (*uv);
    let _e120 = texelSize;
    let _e123 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e119 + (vec2<f32>(-1.0, 1.0) * _e120)));
    let _e125 = moments;
    moments = (_e125 + _e123.xy);
    let _e127 = (*uv);
    let _e128 = texelSize;
    let _e131 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e127 + (vec2<f32>(0.0, -1.0) * _e128)));
    let _e133 = moments;
    moments = (_e133 + _e131.xy);
    let _e135 = (*uv);
    let _e136 = texelSize;
    let _e139 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e135 + (vec2<f32>(0.0, 0.0) * _e136)));
    let _e141 = moments;
    moments = (_e141 + _e139.xy);
    let _e143 = (*uv);
    let _e144 = texelSize;
    let _e147 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e143 + (vec2<f32>(0.0, 1.0) * _e144)));
    let _e149 = moments;
    moments = (_e149 + _e147.xy);
    let _e151 = (*uv);
    let _e152 = texelSize;
    let _e155 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e151 + (vec2<f32>(1.0, -1.0) * _e152)));
    let _e157 = moments;
    moments = (_e157 + _e155.xy);
    let _e159 = (*uv);
    let _e160 = texelSize;
    let _e163 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e159 + (vec2<f32>(1.0, 0.0) * _e160)));
    let _e165 = moments;
    moments = (_e165 + _e163.xy);
    let _e167 = (*uv);
    let _e168 = texelSize;
    let _e171 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e167 + (vec2<f32>(1.0, 1.0) * _e168)));
    let _e173 = moments;
    moments = (_e173 + _e171.xy);
    let _e175 = moments;
    moments = (_e175 / vec2<f32>(9.0));
    let _e178 = moments;
    return _e178;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e103 = (*lsp);
    param = _e103.xy;
    let _e105 = ComputePCFvf2_((&param));
    moments_1 = _e105;
    let _e106 = (*nomral);
    let _e107 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e106, _e107))));
    let _e113 = (*lsp)[2u];
    let _e114 = ShadowBias;
    distance = (_e113 - _e114);
    let _e116 = distance;
    let _e118 = moments_1[0u];
    if (_e116 <= _e118) {
        return 1.0;
    }
    let _e121 = moments_1[1u];
    let _e123 = moments_1[0u];
    let _e125 = moments_1[0u];
    variance = (_e121 - (_e123 * _e125));
    let _e128 = variance;
    variance = max(0.004999999888241291, _e128);
    let _e130 = distance;
    let _e132 = moments_1[0u];
    d = (_e130 - _e132);
    let _e134 = variance;
    let _e135 = variance;
    let _e136 = d;
    let _e137 = d;
    p_max = (_e134 / (_e135 + (_e136 * _e137)));
    let _e141 = p_max;
    return _e141;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e94 = (*srgbIn);
    let _e96 = pow(_e94.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e98 = (*srgbIn)[3u];
    return vec4<f32>(_e96.x, _e96.y, _e96.z, _e98);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e94 = (*srgbIn_1);
    let _e96 = pow(_e94.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e98 = (*srgbIn_1)[3u];
    return vec4<f32>(_e96.x, _e96.y, _e96.z, _e98);
}

fn ComputeReflectionColorstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_(pbrParam: ptr<function, PBRParam>, v: ptr<function, vec3<f32>>, n: ptr<function, vec3<f32>>) -> vec3<f32> {
    var reflectColor: vec3<f32>;
    var mipCount: f32;
    var lod: f32;
    var param_1: vec4<f32>;

    reflectColor = vec3<f32>(0.0, 0.0, 0.0);
    let _e101 = ubo.useCubeMap;
    if (_e101 != 0) {
        let _e104 = ubo.mipCount;
        mipCount = _e104;
        let _e105 = mipCount;
        let _e107 = (*pbrParam).perceptualRoughness;
        lod = (_e105 * _e107);
        let _e109 = (*v);
        let _e110 = (*n);
        let _e112 = lod;
        let _e113 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e109, _e110), _e112);
        param_1 = _e113;
        let _e114 = LINEARtoSRGBvf4_((&param_1));
        reflectColor = _e114.xyz;
    }
    let _e116 = reflectColor;
    return _e116;
}

fn GetSphericalTexcoordvf3_(Dir: ptr<function, vec3<f32>>) -> vec2<f32> {
    var pi: f32;
    var theta: f32;
    var phi: f32;
    var st: vec2<f32>;

    pi = 3.1414999961853027;
    let _e99 = (*Dir)[1u];
    theta = acos(_e99);
    let _e102 = (*Dir)[2u];
    let _e104 = (*Dir)[0u];
    phi = atan2(_e102, _e104);
    let _e106 = phi;
    let _e107 = pi;
    let _e110 = theta;
    let _e111 = pi;
    st = vec2<f32>((_e106 / (2.0 * _e107)), (_e110 / _e111));
    let _e114 = st;
    return _e114;
}

fn ComputeIBLstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_(pbrParam_1: ptr<function, PBRParam>, v_1: ptr<function, vec3<f32>>, n_1: ptr<function, vec3<f32>>) -> vec3<f32> {
    var mipCount_1: f32;
    var lod_1: f32;
    var brdf: vec3<f32>;
    var param_2: vec4<f32>;
    var diffuseLight: vec3<f32>;
    var param_3: vec3<f32>;
    var param_4: vec4<f32>;
    var specularLight: vec3<f32>;
    var param_5: vec3<f32>;
    var param_6: vec4<f32>;
    var diffuse: vec3<f32>;
    var specular: vec3<f32>;

    let _e109 = ubo.mipCount;
    mipCount_1 = _e109;
    let _e110 = mipCount_1;
    let _e112 = (*pbrParam_1).perceptualRoughness;
    lod_1 = (_e110 * _e112);
    let _e115 = (*pbrParam_1).NdotV;
    let _e117 = (*pbrParam_1).perceptualRoughness;
    let _e120 = textureSample(IBL_GGXLUT_Texture, IBL_GGXLUT_TextureSampler, vec2<f32>(_e115, (1.0 - _e117)));
    param_2 = _e120;
    let _e121 = SRGBtoLINEARvf4_((&param_2));
    brdf = _e121.xyz;
    let _e123 = (*n_1);
    param_3 = _e123;
    let _e124 = GetSphericalTexcoordvf3_((&param_3));
    let _e125 = textureSample(IBL_Diffuse_Texture, IBL_Diffuse_TextureSampler, _e124);
    param_4 = _e125;
    let _e126 = SRGBtoLINEARvf4_((&param_4));
    diffuseLight = _e126.xyz;
    let _e128 = (*v_1);
    let _e129 = (*n_1);
    param_5 = reflect(_e128, _e129);
    let _e131 = GetSphericalTexcoordvf3_((&param_5));
    let _e132 = lod_1;
    let _e133 = textureSampleLevel(IBL_Specular_Texture, IBL_Specular_TextureSampler, _e131, _e132);
    param_6 = _e133;
    let _e134 = SRGBtoLINEARvf4_((&param_6));
    specularLight = _e134.xyz;
    let _e136 = diffuseLight;
    let _e138 = (*pbrParam_1).diffuseColor;
    diffuse = (_e136 * _e138);
    let _e140 = specularLight;
    let _e142 = (*pbrParam_1).specularColor;
    let _e144 = brdf[0u];
    let _e147 = brdf[1u];
    specular = (_e140 * ((_e142 * _e144) + vec3<f32>(_e147)));
    let _e151 = diffuse;
    let _e152 = specular;
    return (_e151 + _e152);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_7: ptr<function, PBRParam>) -> vec3<f32> {
    let _e95 = (*param_7).diffuseColor;
    return (_e95 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_8: ptr<function, PBRParam>) -> vec3<f32> {
    let _e95 = (*param_8).reflectance0_;
    let _e97 = (*param_8).reflectance90_;
    let _e99 = (*param_8).reflectance0_;
    let _e102 = (*param_8).VdotH;
    return (_e95 + ((_e97 - _e99) * pow(clamp((1.0 - _e102), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_9: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e100 = (*param_9).NdotL;
    NdotL = _e100;
    let _e102 = (*param_9).NdotV;
    NdotV = _e102;
    let _e104 = (*param_9).alphaRoughness;
    r = _e104;
    let _e105 = NdotL;
    let _e107 = NdotL;
    let _e108 = r;
    let _e109 = r;
    let _e111 = r;
    let _e112 = r;
    let _e115 = NdotL;
    let _e116 = NdotL;
    attenuationL = ((2.0 * _e105) / (_e107 + sqrt(((_e108 * _e109) + ((1.0 - (_e111 * _e112)) * (_e115 * _e116))))));
    let _e123 = NdotV;
    let _e125 = NdotV;
    let _e126 = r;
    let _e127 = r;
    let _e129 = r;
    let _e130 = r;
    let _e133 = NdotV;
    let _e134 = NdotV;
    attenuationV = ((2.0 * _e123) / (_e125 + sqrt(((_e126 * _e127) + ((1.0 - (_e129 * _e130)) * (_e133 * _e134))))));
    let _e141 = attenuationL;
    let _e142 = attenuationV;
    return (_e141 * _e142);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_10: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e97 = (*param_10).alphaRoughness;
    let _e99 = (*param_10).alphaRoughness;
    roughness2_ = (_e97 * _e99);
    let _e102 = (*param_10).NdotH;
    let _e103 = roughness2_;
    let _e106 = (*param_10).NdotH;
    let _e109 = (*param_10).NdotH;
    f = ((((_e102 * _e103) - _e106) * _e109) + 1.0);
    let _e112 = roughness2_;
    let _e113 = f;
    let _e115 = f;
    return (_e112 / ((3.1415927410125732 * _e113) * _e115));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n_2: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e99 = ubo.useNormalTexture;
    if (_e99 != 0) {
        let _e101 = f_WorldTangent_1;
        t = normalize(_e101);
        let _e103 = f_WorldBioTangent_1;
        b = normalize(_e103);
        let _e105 = f_WorldNormal_1;
        n_2 = normalize(_e105);
        let _e107 = t;
        let _e108 = b;
        let _e109 = n_2;
        tbn = mat3x3<f32>(vec3<f32>(_e107.x, _e107.y, _e107.z), vec3<f32>(_e108.x, _e108.y, _e108.z), vec3<f32>(_e109.x, _e109.y, _e109.z));
        let _e123 = f_Texcoord_1;
        let _e124 = textureSample(normalTexture, normalTextureSampler, _e123);
        nomral_1 = _e124.xyz;
        let _e126 = tbn;
        let _e127 = nomral_1;
        let _e132 = ubo.normalMapScale;
        let _e134 = ubo.normalMapScale;
        nomral_1 = normalize((_e126 * (((_e127 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e132, _e134, 1.0))));
    } else {
        let _e139 = f_WorldNormal_1;
        nomral_1 = _e139;
    }
    let _e140 = nomral_1;
    return _e140;
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
    var n_3: vec3<f32>;
    var v_2: vec3<f32>;
    var l: vec3<f32>;
    var h: vec3<f32>;
    var reflection: vec3<f32>;
    var NdotL_1: f32;
    var NdotV_1: f32;
    var NdotH: f32;
    var LdotH: f32;
    var VdotH: f32;
    var pbrParam_2: PBRParam;
    var specular_1: vec3<f32>;
    var diffuse_1: vec3<f32>;
    var D: f32;
    var param_11: PBRParam;
    var G: f32;
    var param_12: PBRParam;
    var F: vec3<f32>;
    var param_13: PBRParam;
    var param_14: PBRParam;
    var param_15: PBRParam;
    var param_16: vec3<f32>;
    var param_17: vec3<f32>;
    var param_18: PBRParam;
    var param_19: vec3<f32>;
    var param_20: vec3<f32>;
    var gi_diffuse: vec3<f32>;
    var ao: f32;
    var emissive: vec3<f32>;
    var param_21: vec4<f32>;
    var lsp_1: vec3<f32>;
    var shadowCol: f32;
    var param_22: vec3<f32>;
    var param_23: vec3<f32>;
    var param_24: vec3<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e142 = ubo.roughnessFactor;
    perceptualRoughness = _e142;
    let _e144 = ubo.metallicFactor;
    metallic = _e144;
    let _e146 = ubo.useMetallicRoughnessTexture;
    if (_e146 != 0) {
        let _e148 = f_Texcoord_1;
        let _e149 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e148);
        metallicRoughnessColor = _e149;
        let _e150 = perceptualRoughness;
        let _e152 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e150 * _e152);
        let _e154 = metallic;
        let _e156 = metallicRoughnessColor[2u];
        metallic = (_e154 * _e156);
    }
    let _e158 = perceptualRoughness;
    perceptualRoughness = clamp(_e158, 0.03999999910593033, 1.0);
    let _e160 = metallic;
    metallic = clamp(_e160, 0.0, 1.0);
    let _e162 = perceptualRoughness;
    let _e163 = perceptualRoughness;
    alphaRoughness = (_e162 * _e163);
    let _e166 = ubo.useBaseColorTexture;
    if (_e166 != 0) {
        let _e168 = f_Texcoord_1;
        let _e169 = textureSample(baseColorTexture, baseColorTextureSampler, _e168);
        baseColor = _e169;
    } else {
        let _e171 = ubo.baseColorFactor;
        baseColor = _e171;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e172 = baseColor;
    let _e174 = f0_;
    diffuseColor = (_e172.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e174));
    let _e177 = metallic;
    let _e179 = diffuseColor;
    diffuseColor = (_e179 * (1.0 - _e177));
    let _e181 = f0_;
    let _e182 = baseColor;
    let _e184 = metallic;
    specularColor = mix(_e181, _e182.xyz, vec3<f32>(_e184));
    let _e188 = specularColor[0u];
    let _e190 = specularColor[1u];
    let _e193 = specularColor[2u];
    reflectance = max(max(_e188, _e190), _e193);
    let _e195 = reflectance;
    reflectance90_ = clamp((_e195 * 25.0), 0.0, 1.0);
    let _e198 = specularColor;
    specularEnvironmentR0_ = _e198;
    let _e199 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e199);
    let _e201 = getNormal();
    n_3 = _e201;
    let _e202 = f_WorldPos_1;
    let _e205 = ubo.cameraPos;
    v_2 = (normalize((_e202.xyz - _e205.xyz)) * -1.0);
    let _e211 = ubo.lightDir;
    l = (normalize(_e211.xyz) * -1.0);
    let _e215 = v_2;
    let _e216 = l;
    h = normalize((_e215 + _e216));
    let _e219 = v_2;
    let _e220 = n_3;
    reflection = normalize(reflect(_e219, _e220));
    let _e223 = n_3;
    let _e224 = l;
    NdotL_1 = clamp(dot(_e223, _e224), 0.0, 1.0);
    let _e227 = n_3;
    let _e228 = v_2;
    NdotV_1 = clamp(abs(dot(_e227, _e228)), 0.0, 1.0);
    let _e232 = n_3;
    let _e233 = h;
    NdotH = clamp(dot(_e232, _e233), 0.0, 1.0);
    let _e236 = l;
    let _e237 = h;
    LdotH = clamp(dot(_e236, _e237), 0.0, 1.0);
    let _e240 = v_2;
    let _e241 = h;
    VdotH = clamp(dot(_e240, _e241), 0.0, 1.0);
    let _e244 = NdotL_1;
    let _e245 = NdotV_1;
    let _e246 = NdotH;
    let _e247 = LdotH;
    let _e248 = VdotH;
    let _e249 = perceptualRoughness;
    let _e250 = metallic;
    let _e251 = specularEnvironmentR0_;
    let _e252 = specularEnvironmentR90_;
    let _e253 = alphaRoughness;
    let _e254 = diffuseColor;
    let _e255 = specularColor;
    pbrParam_2 = PBRParam(_e244, _e245, _e246, _e247, _e248, _e249, _e250, _e251, _e252, _e253, _e254, _e255);
    specular_1 = vec3<f32>(0.0, 0.0, 0.0);
    diffuse_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e257 = NdotL_1;
    let _e259 = NdotV_1;
    if ((_e257 > 0.0) || (_e259 > 0.0)) {
        let _e262 = pbrParam_2;
        param_11 = _e262;
        let _e263 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_11));
        D = _e263;
        let _e264 = pbrParam_2;
        param_12 = _e264;
        let _e265 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_12));
        G = _e265;
        let _e266 = pbrParam_2;
        param_13 = _e266;
        let _e267 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_13));
        F = _e267;
        let _e268 = D;
        let _e269 = G;
        let _e271 = F;
        let _e273 = NdotL_1;
        let _e275 = NdotV_1;
        let _e279 = specular_1;
        specular_1 = (_e279 + ((_e271 * (_e268 * _e269)) / vec3<f32>(((4.0 * _e273) * _e275))));
        let _e281 = specular_1;
        specular_1 = max(_e281, vec3<f32>(0.0, 0.0, 0.0));
        let _e283 = F;
        let _e286 = pbrParam_2;
        param_14 = _e286;
        let _e287 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_14));
        let _e289 = diffuse_1;
        diffuse_1 = (_e289 + ((vec3<f32>(1.0) - _e283) * _e287));
        let _e291 = NdotL_1;
        let _e292 = specular_1;
        let _e293 = diffuse_1;
        let _e295 = ((_e292 + _e293) * _e291);
        col[0u] = _e295.x;
        col[1u] = _e295.y;
        col[2u] = _e295.z;
        let _e303 = ubo.useIBL;
        if (_e303 != 0) {
            let _e305 = pbrParam_2;
            param_15 = _e305;
            let _e306 = v_2;
            param_16 = _e306;
            let _e307 = n_3;
            param_17 = _e307;
            let _e308 = ComputeIBLstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_((&param_15), (&param_16), (&param_17));
            let _e309 = col;
            let _e311 = (_e309.xyz + _e308);
            col[0u] = _e311.x;
            col[1u] = _e311.y;
            col[2u] = _e311.z;
        } else {
            let _e318 = pbrParam_2;
            param_18 = _e318;
            let _e319 = v_2;
            param_19 = _e319;
            let _e320 = n_3;
            param_20 = _e320;
            let _e321 = ComputeReflectionColorstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_((&param_18), (&param_19), (&param_20));
            let _e322 = F;
            let _e324 = col;
            let _e326 = (_e324.xyz + (_e321 * _e322));
            col[0u] = _e326.x;
            col[1u] = _e326.y;
            col[2u] = _e326.z;
            let _e333 = specular_1;
            gi_diffuse = clamp(_e333, vec3<f32>(0.03999999910593033), vec3<f32>(1.0));
            let _e337 = gi_diffuse;
            let _e338 = diffuse_1;
            let _e340 = col;
            let _e342 = (_e340.xyz + (_e337 * _e338));
            col[0u] = _e342.x;
            col[1u] = _e342.y;
            col[2u] = _e342.z;
        }
    }
    let _e350 = ubo.useOcclusionTexture;
    if (_e350 != 0) {
        let _e352 = f_Texcoord_1;
        let _e353 = textureSample(occlusionTexture, occlusionTextureSampler, _e352);
        ao = _e353.x;
        let _e355 = col;
        let _e357 = col;
        let _e359 = ao;
        let _e362 = ubo.occlusionStrength;
        let _e364 = mix(_e355.xyz, (_e357.xyz * _e359), vec3<f32>(_e362));
        col[0u] = _e364.x;
        col[1u] = _e364.y;
        col[2u] = _e364.z;
    }
    let _e372 = ubo.useEmissiveTexture;
    if (_e372 != 0) {
        let _e374 = f_Texcoord_1;
        let _e375 = textureSample(emissiveTexture, emissiveTextureSampler, _e374);
        param_21 = _e375;
        let _e376 = SRGBtoLINEARvf4_((&param_21));
        let _e379 = ubo.emissiveFactor;
        emissive = (_e376.xyz * _e379.xyz);
        let _e382 = emissive;
        let _e383 = col;
        let _e385 = (_e383.xyz + _e382);
        col[0u] = _e385.x;
        col[1u] = _e385.y;
        col[2u] = _e385.z;
    }
    let _e393 = ubo.useShadowMap;
    if (_e393 != 0) {
        let _e395 = f_LightSpacePos_1;
        let _e398 = f_LightSpacePos_1[3u];
        lsp_1 = (_e395.xyz / vec3<f32>(_e398));
        let _e401 = lsp_1;
        lsp_1 = ((_e401 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e405 = lsp_1;
        param_22 = _e405;
        let _e406 = n_3;
        param_23 = _e406;
        let _e407 = l;
        param_24 = _e407;
        let _e408 = CalcShadowvf3vf3vf3_((&param_22), (&param_23), (&param_24));
        shadowCol = _e408;
        let _e409 = shadowCol;
        let _e410 = col;
        let _e412 = (_e410.xyz * _e409);
        col[0u] = _e412.x;
        col[1u] = _e412.y;
        col[2u] = _e412.z;
    }
    let _e419 = col;
    let _e421 = pow(_e419.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e421.x;
    col[1u] = _e421.y;
    col[2u] = _e421.z;
    let _e429 = baseColor[3u];
    col[3u] = _e429;
    let _e431 = f_DebugColor_1;
    col = _e431;
    let _e432 = col;
    outColor = _e432;
    return;
}

@fragment 
fn main(@location(3) f_WorldTangent: vec3<f32>, @location(4) f_WorldBioTangent: vec3<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(1) f_Texcoord: vec2<f32>, @location(2) f_WorldPos: vec4<f32>, @location(5) f_LightSpacePos: vec4<f32>, @location(6) @interpolate(flat) f_DebugColor: vec4<f32>, @location(7) @interpolate(flat) f_DebugJoint: vec4<i32>) -> @location(0) vec4<f32> {
    f_WorldTangent_1 = f_WorldTangent;
    f_WorldBioTangent_1 = f_WorldBioTangent;
    f_WorldNormal_1 = f_WorldNormal;
    f_Texcoord_1 = f_Texcoord;
    f_WorldPos_1 = f_WorldPos;
    f_LightSpacePos_1 = f_LightSpacePos;
    f_DebugColor_1 = f_DebugColor;
    f_DebugJoint_1 = f_DebugJoint;
    main_1();
    let _e17 = outColor;
    return _e17;
}
