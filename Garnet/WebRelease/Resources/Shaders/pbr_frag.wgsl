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
@group(0) @binding(11) 
var cubemapTexture: texture_cube<f32>;
@group(0) @binding(12) 
var cubemapTextureSampler: sampler;
@group(0) @binding(19) 
var IBL_GGXLUT_Texture: texture_2d<f32>;
@group(0) @binding(20) 
var IBL_GGXLUT_TextureSampler: sampler;
@group(0) @binding(15) 
var IBL_Diffuse_Texture: texture_2d<f32>;
@group(0) @binding(16) 
var IBL_Diffuse_TextureSampler: sampler;
@group(0) @binding(17) 
var IBL_Specular_Texture: texture_2d<f32>;
@group(0) @binding(18) 
var IBL_Specular_TextureSampler: sampler;
@group(0) @binding(3) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(4) 
var metallicRoughnessTextureSampler: sampler;
@group(0) @binding(1) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(2) 
var baseColorTextureSampler: sampler;
var<private> f_WorldPos_1: vec4<f32>;
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
    let _e95 = ubo.ShadowMapX;
    let _e98 = ubo.ShadowMapY;
    texelSize = vec2<f32>((1.0 / _e95), (1.0 / _e98));
    let _e101 = (*uv);
    let _e102 = texelSize;
    let _e105 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e101 + (vec2<f32>(-1.0, -1.0) * _e102)));
    let _e107 = moments;
    moments = (_e107 + _e105.xy);
    let _e109 = (*uv);
    let _e110 = texelSize;
    let _e113 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e109 + (vec2<f32>(-1.0, 0.0) * _e110)));
    let _e115 = moments;
    moments = (_e115 + _e113.xy);
    let _e117 = (*uv);
    let _e118 = texelSize;
    let _e121 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e117 + (vec2<f32>(-1.0, 1.0) * _e118)));
    let _e123 = moments;
    moments = (_e123 + _e121.xy);
    let _e125 = (*uv);
    let _e126 = texelSize;
    let _e129 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e125 + (vec2<f32>(0.0, -1.0) * _e126)));
    let _e131 = moments;
    moments = (_e131 + _e129.xy);
    let _e133 = (*uv);
    let _e134 = texelSize;
    let _e137 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e133 + (vec2<f32>(0.0, 0.0) * _e134)));
    let _e139 = moments;
    moments = (_e139 + _e137.xy);
    let _e141 = (*uv);
    let _e142 = texelSize;
    let _e145 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e141 + (vec2<f32>(0.0, 1.0) * _e142)));
    let _e147 = moments;
    moments = (_e147 + _e145.xy);
    let _e149 = (*uv);
    let _e150 = texelSize;
    let _e153 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e149 + (vec2<f32>(1.0, -1.0) * _e150)));
    let _e155 = moments;
    moments = (_e155 + _e153.xy);
    let _e157 = (*uv);
    let _e158 = texelSize;
    let _e161 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e157 + (vec2<f32>(1.0, 0.0) * _e158)));
    let _e163 = moments;
    moments = (_e163 + _e161.xy);
    let _e165 = (*uv);
    let _e166 = texelSize;
    let _e169 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e165 + (vec2<f32>(1.0, 1.0) * _e166)));
    let _e171 = moments;
    moments = (_e171 + _e169.xy);
    let _e173 = moments;
    moments = (_e173 / vec2<f32>(9.0));
    let _e176 = moments;
    return _e176;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e101 = (*lsp);
    param = _e101.xy;
    let _e103 = ComputePCFvf2_((&param));
    moments_1 = _e103;
    let _e104 = (*nomral);
    let _e105 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e104, _e105))));
    let _e111 = (*lsp)[2u];
    let _e112 = ShadowBias;
    distance = (_e111 - _e112);
    let _e114 = distance;
    let _e116 = moments_1[0u];
    if (_e114 <= _e116) {
        return 1.0;
    }
    let _e119 = moments_1[1u];
    let _e121 = moments_1[0u];
    let _e123 = moments_1[0u];
    variance = (_e119 - (_e121 * _e123));
    let _e126 = variance;
    variance = max(0.004999999888241291, _e126);
    let _e128 = distance;
    let _e130 = moments_1[0u];
    d = (_e128 - _e130);
    let _e132 = variance;
    let _e133 = variance;
    let _e134 = d;
    let _e135 = d;
    p_max = (_e132 / (_e133 + (_e134 * _e135)));
    let _e139 = p_max;
    return _e139;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e92 = (*srgbIn);
    let _e94 = pow(_e92.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e96 = (*srgbIn)[3u];
    return vec4<f32>(_e94.x, _e94.y, _e94.z, _e96);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e92 = (*srgbIn_1);
    let _e94 = pow(_e92.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e96 = (*srgbIn_1)[3u];
    return vec4<f32>(_e94.x, _e94.y, _e94.z, _e96);
}

fn ComputeReflectionColorstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_(pbrParam: ptr<function, PBRParam>, v: ptr<function, vec3<f32>>, n: ptr<function, vec3<f32>>) -> vec3<f32> {
    var reflectColor: vec3<f32>;
    var mipCount: f32;
    var lod: f32;
    var param_1: vec4<f32>;

    reflectColor = vec3<f32>(0.0, 0.0, 0.0);
    let _e99 = ubo.useCubeMap;
    if (_e99 != 0) {
        let _e102 = ubo.mipCount;
        mipCount = _e102;
        let _e103 = mipCount;
        let _e105 = (*pbrParam).perceptualRoughness;
        lod = (_e103 * _e105);
        let _e107 = (*v);
        let _e108 = (*n);
        let _e110 = lod;
        let _e111 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e107, _e108), _e110);
        param_1 = _e111;
        let _e112 = LINEARtoSRGBvf4_((&param_1));
        reflectColor = _e112.xyz;
    }
    let _e114 = reflectColor;
    return _e114;
}

fn GetSphericalTexcoordvf3_(Dir: ptr<function, vec3<f32>>) -> vec2<f32> {
    var pi: f32;
    var theta: f32;
    var phi: f32;
    var st: vec2<f32>;

    pi = 3.1414999961853027;
    let _e97 = (*Dir)[1u];
    theta = acos(_e97);
    let _e100 = (*Dir)[2u];
    let _e102 = (*Dir)[0u];
    phi = atan2(_e100, _e102);
    let _e104 = phi;
    let _e105 = pi;
    let _e108 = theta;
    let _e109 = pi;
    st = vec2<f32>((_e104 / (2.0 * _e105)), (_e108 / _e109));
    let _e112 = st;
    return _e112;
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

    let _e107 = ubo.mipCount;
    mipCount_1 = _e107;
    let _e108 = mipCount_1;
    let _e110 = (*pbrParam_1).perceptualRoughness;
    lod_1 = (_e108 * _e110);
    let _e113 = (*pbrParam_1).NdotV;
    let _e115 = (*pbrParam_1).perceptualRoughness;
    let _e118 = textureSample(IBL_GGXLUT_Texture, IBL_GGXLUT_TextureSampler, vec2<f32>(_e113, (1.0 - _e115)));
    param_2 = _e118;
    let _e119 = SRGBtoLINEARvf4_((&param_2));
    brdf = _e119.xyz;
    let _e121 = (*n_1);
    param_3 = _e121;
    let _e122 = GetSphericalTexcoordvf3_((&param_3));
    let _e123 = textureSample(IBL_Diffuse_Texture, IBL_Diffuse_TextureSampler, _e122);
    param_4 = _e123;
    let _e124 = SRGBtoLINEARvf4_((&param_4));
    diffuseLight = _e124.xyz;
    let _e126 = (*v_1);
    let _e127 = (*n_1);
    param_5 = reflect(_e126, _e127);
    let _e129 = GetSphericalTexcoordvf3_((&param_5));
    let _e130 = lod_1;
    let _e131 = textureSampleLevel(IBL_Specular_Texture, IBL_Specular_TextureSampler, _e129, _e130);
    param_6 = _e131;
    let _e132 = SRGBtoLINEARvf4_((&param_6));
    specularLight = _e132.xyz;
    let _e134 = diffuseLight;
    let _e136 = (*pbrParam_1).diffuseColor;
    diffuse = (_e134 * _e136);
    let _e138 = specularLight;
    let _e140 = (*pbrParam_1).specularColor;
    let _e142 = brdf[0u];
    let _e145 = brdf[1u];
    specular = (_e138 * ((_e140 * _e142) + vec3<f32>(_e145)));
    let _e149 = diffuse;
    let _e150 = specular;
    return (_e149 + _e150);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_7: ptr<function, PBRParam>) -> vec3<f32> {
    let _e93 = (*param_7).diffuseColor;
    return (_e93 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_8: ptr<function, PBRParam>) -> vec3<f32> {
    let _e93 = (*param_8).reflectance0_;
    let _e95 = (*param_8).reflectance90_;
    let _e97 = (*param_8).reflectance0_;
    let _e100 = (*param_8).VdotH;
    return (_e93 + ((_e95 - _e97) * pow(clamp((1.0 - _e100), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_9: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e98 = (*param_9).NdotL;
    NdotL = _e98;
    let _e100 = (*param_9).NdotV;
    NdotV = _e100;
    let _e102 = (*param_9).alphaRoughness;
    r = _e102;
    let _e103 = NdotL;
    let _e105 = NdotL;
    let _e106 = r;
    let _e107 = r;
    let _e109 = r;
    let _e110 = r;
    let _e113 = NdotL;
    let _e114 = NdotL;
    attenuationL = ((2.0 * _e103) / (_e105 + sqrt(((_e106 * _e107) + ((1.0 - (_e109 * _e110)) * (_e113 * _e114))))));
    let _e121 = NdotV;
    let _e123 = NdotV;
    let _e124 = r;
    let _e125 = r;
    let _e127 = r;
    let _e128 = r;
    let _e131 = NdotV;
    let _e132 = NdotV;
    attenuationV = ((2.0 * _e121) / (_e123 + sqrt(((_e124 * _e125) + ((1.0 - (_e127 * _e128)) * (_e131 * _e132))))));
    let _e139 = attenuationL;
    let _e140 = attenuationV;
    return (_e139 * _e140);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_10: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e95 = (*param_10).alphaRoughness;
    let _e97 = (*param_10).alphaRoughness;
    roughness2_ = (_e95 * _e97);
    let _e100 = (*param_10).NdotH;
    let _e101 = roughness2_;
    let _e104 = (*param_10).NdotH;
    let _e107 = (*param_10).NdotH;
    f = ((((_e100 * _e101) - _e104) * _e107) + 1.0);
    let _e110 = roughness2_;
    let _e111 = f;
    let _e113 = f;
    return (_e110 / ((3.1415927410125732 * _e111) * _e113));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n_2: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e97 = ubo.useNormalTexture;
    if (_e97 != 0) {
        let _e99 = f_WorldTangent_1;
        t = normalize(_e99);
        let _e101 = f_WorldBioTangent_1;
        b = normalize(_e101);
        let _e103 = f_WorldNormal_1;
        n_2 = normalize(_e103);
        let _e105 = t;
        let _e106 = b;
        let _e107 = n_2;
        tbn = mat3x3<f32>(vec3<f32>(_e105.x, _e105.y, _e105.z), vec3<f32>(_e106.x, _e106.y, _e106.z), vec3<f32>(_e107.x, _e107.y, _e107.z));
        let _e121 = f_Texcoord_1;
        let _e122 = textureSample(normalTexture, normalTextureSampler, _e121);
        nomral_1 = _e122.xyz;
        let _e124 = tbn;
        let _e125 = nomral_1;
        let _e130 = ubo.normalMapScale;
        let _e132 = ubo.normalMapScale;
        nomral_1 = normalize((_e124 * (((_e125 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e130, _e132, 1.0))));
    } else {
        let _e137 = f_WorldNormal_1;
        nomral_1 = _e137;
    }
    let _e138 = nomral_1;
    return _e138;
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
    let _e140 = ubo.roughnessFactor;
    perceptualRoughness = _e140;
    let _e142 = ubo.metallicFactor;
    metallic = _e142;
    let _e144 = ubo.useMetallicRoughnessTexture;
    if (_e144 != 0) {
        let _e146 = f_Texcoord_1;
        let _e147 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e146);
        metallicRoughnessColor = _e147;
        let _e148 = perceptualRoughness;
        let _e150 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e148 * _e150);
        let _e152 = metallic;
        let _e154 = metallicRoughnessColor[2u];
        metallic = (_e152 * _e154);
    }
    let _e156 = perceptualRoughness;
    perceptualRoughness = clamp(_e156, 0.03999999910593033, 1.0);
    let _e158 = metallic;
    metallic = clamp(_e158, 0.0, 1.0);
    let _e160 = perceptualRoughness;
    let _e161 = perceptualRoughness;
    alphaRoughness = (_e160 * _e161);
    let _e164 = ubo.useBaseColorTexture;
    if (_e164 != 0) {
        let _e166 = f_Texcoord_1;
        let _e167 = textureSample(baseColorTexture, baseColorTextureSampler, _e166);
        baseColor = _e167;
    } else {
        let _e169 = ubo.baseColorFactor;
        baseColor = _e169;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e170 = baseColor;
    let _e172 = f0_;
    diffuseColor = (_e170.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e172));
    let _e175 = metallic;
    let _e177 = diffuseColor;
    diffuseColor = (_e177 * (1.0 - _e175));
    let _e179 = f0_;
    let _e180 = baseColor;
    let _e182 = metallic;
    specularColor = mix(_e179, _e180.xyz, vec3<f32>(_e182));
    let _e186 = specularColor[0u];
    let _e188 = specularColor[1u];
    let _e191 = specularColor[2u];
    reflectance = max(max(_e186, _e188), _e191);
    let _e193 = reflectance;
    reflectance90_ = clamp((_e193 * 25.0), 0.0, 1.0);
    let _e196 = specularColor;
    specularEnvironmentR0_ = _e196;
    let _e197 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e197);
    let _e199 = getNormal();
    n_3 = _e199;
    let _e200 = f_WorldPos_1;
    let _e203 = ubo.cameraPos;
    v_2 = (normalize((_e200.xyz - _e203.xyz)) * -1.0);
    let _e209 = ubo.lightDir;
    l = (normalize(_e209.xyz) * -1.0);
    let _e213 = v_2;
    let _e214 = l;
    h = normalize((_e213 + _e214));
    let _e217 = v_2;
    let _e218 = n_3;
    reflection = normalize(reflect(_e217, _e218));
    let _e221 = n_3;
    let _e222 = l;
    NdotL_1 = clamp(dot(_e221, _e222), 0.0, 1.0);
    let _e225 = n_3;
    let _e226 = v_2;
    NdotV_1 = clamp(abs(dot(_e225, _e226)), 0.0, 1.0);
    let _e230 = n_3;
    let _e231 = h;
    NdotH = clamp(dot(_e230, _e231), 0.0, 1.0);
    let _e234 = l;
    let _e235 = h;
    LdotH = clamp(dot(_e234, _e235), 0.0, 1.0);
    let _e238 = v_2;
    let _e239 = h;
    VdotH = clamp(dot(_e238, _e239), 0.0, 1.0);
    let _e242 = NdotL_1;
    let _e243 = NdotV_1;
    let _e244 = NdotH;
    let _e245 = LdotH;
    let _e246 = VdotH;
    let _e247 = perceptualRoughness;
    let _e248 = metallic;
    let _e249 = specularEnvironmentR0_;
    let _e250 = specularEnvironmentR90_;
    let _e251 = alphaRoughness;
    let _e252 = diffuseColor;
    let _e253 = specularColor;
    pbrParam_2 = PBRParam(_e242, _e243, _e244, _e245, _e246, _e247, _e248, _e249, _e250, _e251, _e252, _e253);
    specular_1 = vec3<f32>(0.0, 0.0, 0.0);
    diffuse_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e255 = NdotL_1;
    let _e257 = NdotV_1;
    if ((_e255 > 0.0) || (_e257 > 0.0)) {
        let _e260 = pbrParam_2;
        param_11 = _e260;
        let _e261 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_11));
        D = _e261;
        let _e262 = pbrParam_2;
        param_12 = _e262;
        let _e263 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_12));
        G = _e263;
        let _e264 = pbrParam_2;
        param_13 = _e264;
        let _e265 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_13));
        F = _e265;
        let _e266 = D;
        let _e267 = G;
        let _e269 = F;
        let _e271 = NdotL_1;
        let _e273 = NdotV_1;
        let _e277 = specular_1;
        specular_1 = (_e277 + ((_e269 * (_e266 * _e267)) / vec3<f32>(((4.0 * _e271) * _e273))));
        let _e279 = specular_1;
        specular_1 = max(_e279, vec3<f32>(0.0, 0.0, 0.0));
        let _e281 = F;
        let _e284 = pbrParam_2;
        param_14 = _e284;
        let _e285 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_14));
        let _e287 = diffuse_1;
        diffuse_1 = (_e287 + ((vec3<f32>(1.0) - _e281) * _e285));
        let _e289 = NdotL_1;
        let _e290 = specular_1;
        let _e291 = diffuse_1;
        let _e293 = ((_e290 + _e291) * _e289);
        col[0u] = _e293.x;
        col[1u] = _e293.y;
        col[2u] = _e293.z;
        let _e301 = ubo.useIBL;
        if (_e301 != 0) {
            let _e303 = pbrParam_2;
            param_15 = _e303;
            let _e304 = v_2;
            param_16 = _e304;
            let _e305 = n_3;
            param_17 = _e305;
            let _e306 = ComputeIBLstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_((&param_15), (&param_16), (&param_17));
            let _e307 = col;
            let _e309 = (_e307.xyz + _e306);
            col[0u] = _e309.x;
            col[1u] = _e309.y;
            col[2u] = _e309.z;
        } else {
            let _e316 = pbrParam_2;
            param_18 = _e316;
            let _e317 = v_2;
            param_19 = _e317;
            let _e318 = n_3;
            param_20 = _e318;
            let _e319 = ComputeReflectionColorstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_((&param_18), (&param_19), (&param_20));
            let _e320 = F;
            let _e322 = col;
            let _e324 = (_e322.xyz + (_e319 * _e320));
            col[0u] = _e324.x;
            col[1u] = _e324.y;
            col[2u] = _e324.z;
            let _e331 = specular_1;
            gi_diffuse = clamp(_e331, vec3<f32>(0.03999999910593033), vec3<f32>(1.0));
            let _e335 = gi_diffuse;
            let _e336 = diffuse_1;
            let _e338 = col;
            let _e340 = (_e338.xyz + (_e335 * _e336));
            col[0u] = _e340.x;
            col[1u] = _e340.y;
            col[2u] = _e340.z;
        }
    }
    let _e348 = ubo.useOcclusionTexture;
    if (_e348 != 0) {
        let _e350 = f_Texcoord_1;
        let _e351 = textureSample(occlusionTexture, occlusionTextureSampler, _e350);
        ao = _e351.x;
        let _e353 = col;
        let _e355 = col;
        let _e357 = ao;
        let _e360 = ubo.occlusionStrength;
        let _e362 = mix(_e353.xyz, (_e355.xyz * _e357), vec3<f32>(_e360));
        col[0u] = _e362.x;
        col[1u] = _e362.y;
        col[2u] = _e362.z;
    }
    let _e370 = ubo.useEmissiveTexture;
    if (_e370 != 0) {
        let _e372 = f_Texcoord_1;
        let _e373 = textureSample(emissiveTexture, emissiveTextureSampler, _e372);
        param_21 = _e373;
        let _e374 = SRGBtoLINEARvf4_((&param_21));
        let _e377 = ubo.emissiveFactor;
        emissive = (_e374.xyz * _e377.xyz);
        let _e380 = emissive;
        let _e381 = col;
        let _e383 = (_e381.xyz + _e380);
        col[0u] = _e383.x;
        col[1u] = _e383.y;
        col[2u] = _e383.z;
    }
    let _e391 = ubo.useShadowMap;
    if (_e391 != 0) {
        let _e393 = f_LightSpacePos_1;
        let _e396 = f_LightSpacePos_1[3u];
        lsp_1 = (_e393.xyz / vec3<f32>(_e396));
        let _e399 = lsp_1;
        lsp_1 = ((_e399 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e403 = lsp_1;
        param_22 = _e403;
        let _e404 = n_3;
        param_23 = _e404;
        let _e405 = l;
        param_24 = _e405;
        let _e406 = CalcShadowvf3vf3vf3_((&param_22), (&param_23), (&param_24));
        shadowCol = _e406;
        let _e407 = shadowCol;
        let _e408 = col;
        let _e410 = (_e408.xyz * _e407);
        col[0u] = _e410.x;
        col[1u] = _e410.y;
        col[2u] = _e410.z;
    }
    let _e417 = col;
    let _e419 = pow(_e417.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e419.x;
    col[1u] = _e419.y;
    col[2u] = _e419.z;
    let _e427 = baseColor[3u];
    col[3u] = _e427;
    let _e429 = col;
    outColor = _e429;
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
