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
    JointIndexOffset: i32,
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
@group(0) @binding(2) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(3) 
var baseColorTextureSampler: sampler;
@group(0) @binding(4) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(5) 
var metallicRoughnessTextureSampler: sampler;
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
var<private> outColor: vec4<f32>;
@group(0) @binding(12) 
var cubemapTexture: texture_cube<f32>;
@group(0) @binding(13) 
var cubemapTextureSampler: sampler;
@group(0) @binding(16) 
var IBL_Diffuse_Texture: texture_2d<f32>;
@group(0) @binding(17) 
var IBL_Diffuse_TextureSampler: sampler;
@group(0) @binding(18) 
var IBL_Specular_Texture: texture_2d<f32>;
@group(0) @binding(19) 
var IBL_Specular_TextureSampler: sampler;
@group(0) @binding(20) 
var IBL_GGXLUT_Texture: texture_2d<f32>;
@group(0) @binding(21) 
var IBL_GGXLUT_TextureSampler: sampler;

fn ComputePCFvf2_(uv: ptr<function, vec2<f32>>) -> vec2<f32> {
    var moments: vec2<f32>;
    var texelSize: vec2<f32>;

    moments = vec2<f32>(0.0, 0.0);
    let _e90 = ubo.ShadowMapX;
    let _e93 = ubo.ShadowMapY;
    texelSize = vec2<f32>((1.0 / _e90), (1.0 / _e93));
    let _e96 = (*uv);
    let _e97 = texelSize;
    let _e100 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e96 + (vec2<f32>(-1.0, -1.0) * _e97)));
    let _e102 = moments;
    moments = (_e102 + _e100.xy);
    let _e104 = (*uv);
    let _e105 = texelSize;
    let _e108 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e104 + (vec2<f32>(-1.0, 0.0) * _e105)));
    let _e110 = moments;
    moments = (_e110 + _e108.xy);
    let _e112 = (*uv);
    let _e113 = texelSize;
    let _e116 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e112 + (vec2<f32>(-1.0, 1.0) * _e113)));
    let _e118 = moments;
    moments = (_e118 + _e116.xy);
    let _e120 = (*uv);
    let _e121 = texelSize;
    let _e124 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e120 + (vec2<f32>(0.0, -1.0) * _e121)));
    let _e126 = moments;
    moments = (_e126 + _e124.xy);
    let _e128 = (*uv);
    let _e129 = texelSize;
    let _e132 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e128 + (vec2<f32>(0.0, 0.0) * _e129)));
    let _e134 = moments;
    moments = (_e134 + _e132.xy);
    let _e136 = (*uv);
    let _e137 = texelSize;
    let _e140 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e136 + (vec2<f32>(0.0, 1.0) * _e137)));
    let _e142 = moments;
    moments = (_e142 + _e140.xy);
    let _e144 = (*uv);
    let _e145 = texelSize;
    let _e148 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e144 + (vec2<f32>(1.0, -1.0) * _e145)));
    let _e150 = moments;
    moments = (_e150 + _e148.xy);
    let _e152 = (*uv);
    let _e153 = texelSize;
    let _e156 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e152 + (vec2<f32>(1.0, 0.0) * _e153)));
    let _e158 = moments;
    moments = (_e158 + _e156.xy);
    let _e160 = (*uv);
    let _e161 = texelSize;
    let _e164 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e160 + (vec2<f32>(1.0, 1.0) * _e161)));
    let _e166 = moments;
    moments = (_e166 + _e164.xy);
    let _e168 = moments;
    moments = (_e168 / vec2<f32>(9.0));
    let _e171 = moments;
    return _e171;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e96 = (*lsp);
    param = _e96.xy;
    let _e98 = ComputePCFvf2_((&param));
    moments_1 = _e98;
    let _e99 = (*nomral);
    let _e100 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e99, _e100))));
    let _e106 = (*lsp)[2u];
    let _e107 = ShadowBias;
    distance = (_e106 - _e107);
    let _e109 = distance;
    let _e111 = moments_1[0u];
    if (_e109 <= _e111) {
        return 1.0;
    }
    let _e114 = moments_1[1u];
    let _e116 = moments_1[0u];
    let _e118 = moments_1[0u];
    variance = (_e114 - (_e116 * _e118));
    let _e121 = variance;
    variance = max(0.004999999888241291, _e121);
    let _e123 = distance;
    let _e125 = moments_1[0u];
    d = (_e123 - _e125);
    let _e127 = variance;
    let _e128 = variance;
    let _e129 = d;
    let _e130 = d;
    p_max = (_e127 / (_e128 + (_e129 * _e130)));
    let _e134 = p_max;
    return _e134;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e87 = (*srgbIn);
    let _e89 = pow(_e87.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e91 = (*srgbIn)[3u];
    return vec4<f32>(_e89.x, _e89.y, _e89.z, _e91);
}

fn ComputeIBLstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_(pbrParam: ptr<function, PBRParam>, v: ptr<function, vec3<f32>>, n: ptr<function, vec3<f32>>) -> vec3<f32> {
    var baseColor: vec4<f32>;

    let _e90 = f_Texcoord_1;
    let _e91 = textureSample(baseColorTexture, baseColorTextureSampler, _e90);
    baseColor = _e91;
    let _e92 = baseColor;
    return _e92.xyz;
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e88 = (*param_1).diffuseColor;
    return (_e88 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> vec3<f32> {
    let _e88 = (*param_2).reflectance0_;
    let _e90 = (*param_2).reflectance90_;
    let _e92 = (*param_2).reflectance0_;
    let _e95 = (*param_2).VdotH;
    return (_e88 + ((_e90 - _e92) * pow(clamp((1.0 - _e95), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e93 = (*param_3).NdotL;
    NdotL = _e93;
    let _e95 = (*param_3).NdotV;
    NdotV = _e95;
    let _e97 = (*param_3).alphaRoughness;
    r = _e97;
    let _e98 = NdotL;
    let _e100 = NdotL;
    let _e101 = r;
    let _e102 = r;
    let _e104 = r;
    let _e105 = r;
    let _e108 = NdotL;
    let _e109 = NdotL;
    attenuationL = ((2.0 * _e98) / (_e100 + sqrt(((_e101 * _e102) + ((1.0 - (_e104 * _e105)) * (_e108 * _e109))))));
    let _e116 = NdotV;
    let _e118 = NdotV;
    let _e119 = r;
    let _e120 = r;
    let _e122 = r;
    let _e123 = r;
    let _e126 = NdotV;
    let _e127 = NdotV;
    attenuationV = ((2.0 * _e116) / (_e118 + sqrt(((_e119 * _e120) + ((1.0 - (_e122 * _e123)) * (_e126 * _e127))))));
    let _e134 = attenuationL;
    let _e135 = attenuationV;
    return (_e134 * _e135);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_4: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e90 = (*param_4).alphaRoughness;
    let _e92 = (*param_4).alphaRoughness;
    roughness2_ = (_e90 * _e92);
    let _e95 = (*param_4).NdotH;
    let _e96 = roughness2_;
    let _e99 = (*param_4).NdotH;
    let _e102 = (*param_4).NdotH;
    f = ((((_e95 * _e96) - _e99) * _e102) + 1.0);
    let _e105 = roughness2_;
    let _e106 = f;
    let _e108 = f;
    return (_e105 / ((3.1415927410125732 * _e106) * _e108));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n_1: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e92 = ubo.useNormalTexture;
    if (_e92 != 0) {
        let _e94 = f_WorldTangent_1;
        t = normalize(_e94);
        let _e96 = f_WorldBioTangent_1;
        b = normalize(_e96);
        let _e98 = f_WorldNormal_1;
        n_1 = normalize(_e98);
        let _e100 = t;
        let _e101 = b;
        let _e102 = n_1;
        tbn = mat3x3<f32>(vec3<f32>(_e100.x, _e100.y, _e100.z), vec3<f32>(_e101.x, _e101.y, _e101.z), vec3<f32>(_e102.x, _e102.y, _e102.z));
        let _e116 = f_Texcoord_1;
        let _e117 = textureSample(normalTexture, normalTextureSampler, _e116);
        nomral_1 = _e117.xyz;
        let _e119 = tbn;
        let _e120 = nomral_1;
        let _e125 = ubo.normalMapScale;
        let _e127 = ubo.normalMapScale;
        nomral_1 = normalize((_e119 * (((_e120 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e125, _e127, 1.0))));
    } else {
        let _e132 = f_WorldNormal_1;
        nomral_1 = _e132;
    }
    let _e133 = nomral_1;
    return _e133;
}

fn main_1() {
    var col: vec4<f32>;
    var perceptualRoughness: f32;
    var metallic: f32;
    var metallicRoughnessColor: vec4<f32>;
    var alphaRoughness: f32;
    var baseColor_1: vec4<f32>;
    var f0_: vec3<f32>;
    var diffuseColor: vec3<f32>;
    var specularColor: vec3<f32>;
    var reflectance: f32;
    var reflectance90_: f32;
    var specularEnvironmentR0_: vec3<f32>;
    var specularEnvironmentR90_: vec3<f32>;
    var n_2: vec3<f32>;
    var v_1: vec3<f32>;
    var l: vec3<f32>;
    var h: vec3<f32>;
    var reflection: vec3<f32>;
    var NdotL_1: f32;
    var NdotV_1: f32;
    var NdotH: f32;
    var LdotH: f32;
    var VdotH: f32;
    var pbrParam_1: PBRParam;
    var specular: vec3<f32>;
    var diffuse: vec3<f32>;
    var D: f32;
    var param_5: PBRParam;
    var G: f32;
    var param_6: PBRParam;
    var F: vec3<f32>;
    var param_7: PBRParam;
    var param_8: PBRParam;
    var param_9: PBRParam;
    var param_10: vec3<f32>;
    var param_11: vec3<f32>;
    var ao: f32;
    var emissive: vec3<f32>;
    var param_12: vec4<f32>;
    var lsp_1: vec3<f32>;
    var shadowCol: f32;
    var param_13: vec3<f32>;
    var param_14: vec3<f32>;
    var param_15: vec3<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e131 = ubo.roughnessFactor;
    perceptualRoughness = _e131;
    let _e133 = ubo.metallicFactor;
    metallic = _e133;
    let _e135 = ubo.useMetallicRoughnessTexture;
    if (_e135 != 0) {
        let _e137 = f_Texcoord_1;
        let _e138 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e137);
        metallicRoughnessColor = _e138;
        let _e139 = perceptualRoughness;
        let _e141 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e139 * _e141);
        let _e143 = metallic;
        let _e145 = metallicRoughnessColor[2u];
        metallic = (_e143 * _e145);
    }
    let _e147 = perceptualRoughness;
    perceptualRoughness = clamp(_e147, 0.03999999910593033, 1.0);
    let _e149 = metallic;
    metallic = clamp(_e149, 0.0, 1.0);
    let _e151 = perceptualRoughness;
    let _e152 = perceptualRoughness;
    alphaRoughness = (_e151 * _e152);
    let _e155 = ubo.useBaseColorTexture;
    if (_e155 != 0) {
        let _e157 = f_Texcoord_1;
        let _e158 = textureSample(baseColorTexture, baseColorTextureSampler, _e157);
        baseColor_1 = _e158;
    } else {
        let _e160 = ubo.baseColorFactor;
        baseColor_1 = _e160;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e161 = baseColor_1;
    let _e163 = f0_;
    diffuseColor = (_e161.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e163));
    let _e166 = metallic;
    let _e168 = diffuseColor;
    diffuseColor = (_e168 * (1.0 - _e166));
    let _e170 = f0_;
    let _e171 = baseColor_1;
    let _e173 = metallic;
    specularColor = mix(_e170, _e171.xyz, vec3<f32>(_e173));
    let _e177 = specularColor[0u];
    let _e179 = specularColor[1u];
    let _e182 = specularColor[2u];
    reflectance = max(max(_e177, _e179), _e182);
    let _e184 = reflectance;
    reflectance90_ = clamp((_e184 * 25.0), 0.0, 1.0);
    let _e187 = specularColor;
    specularEnvironmentR0_ = _e187;
    let _e188 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e188);
    let _e190 = getNormal();
    n_2 = _e190;
    let _e191 = f_WorldPos_1;
    let _e194 = ubo.cameraPos;
    v_1 = (normalize((_e191.xyz - _e194.xyz)) * -1.0);
    let _e200 = ubo.lightDir;
    l = (normalize(_e200.xyz) * -1.0);
    let _e204 = v_1;
    let _e205 = l;
    h = normalize((_e204 + _e205));
    let _e208 = v_1;
    let _e209 = n_2;
    reflection = normalize(reflect(_e208, _e209));
    let _e212 = n_2;
    let _e213 = l;
    NdotL_1 = clamp(dot(_e212, _e213), 0.0, 1.0);
    let _e216 = n_2;
    let _e217 = v_1;
    NdotV_1 = clamp(abs(dot(_e216, _e217)), 0.0, 1.0);
    let _e221 = n_2;
    let _e222 = h;
    NdotH = clamp(dot(_e221, _e222), 0.0, 1.0);
    let _e225 = l;
    let _e226 = h;
    LdotH = clamp(dot(_e225, _e226), 0.0, 1.0);
    let _e229 = v_1;
    let _e230 = h;
    VdotH = clamp(dot(_e229, _e230), 0.0, 1.0);
    let _e233 = NdotL_1;
    let _e234 = NdotV_1;
    let _e235 = NdotH;
    let _e236 = LdotH;
    let _e237 = VdotH;
    let _e238 = perceptualRoughness;
    let _e239 = metallic;
    let _e240 = specularEnvironmentR0_;
    let _e241 = specularEnvironmentR90_;
    let _e242 = alphaRoughness;
    let _e243 = diffuseColor;
    let _e244 = specularColor;
    pbrParam_1 = PBRParam(_e233, _e234, _e235, _e236, _e237, _e238, _e239, _e240, _e241, _e242, _e243, _e244);
    specular = vec3<f32>(0.0, 0.0, 0.0);
    diffuse = vec3<f32>(0.0, 0.0, 0.0);
    let _e246 = NdotL_1;
    let _e248 = NdotV_1;
    if ((_e246 > 0.0) || (_e248 > 0.0)) {
        let _e251 = pbrParam_1;
        param_5 = _e251;
        let _e252 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
        D = _e252;
        let _e253 = pbrParam_1;
        param_6 = _e253;
        let _e254 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
        G = _e254;
        let _e255 = pbrParam_1;
        param_7 = _e255;
        let _e256 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
        F = _e256;
        let _e257 = D;
        let _e258 = G;
        let _e260 = F;
        let _e262 = NdotL_1;
        let _e264 = NdotV_1;
        let _e268 = specular;
        specular = (_e268 + ((_e260 * (_e257 * _e258)) / vec3<f32>(((4.0 * _e262) * _e264))));
        let _e270 = specular;
        specular = max(_e270, vec3<f32>(0.0, 0.0, 0.0));
        let _e272 = F;
        let _e275 = pbrParam_1;
        param_8 = _e275;
        let _e276 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_8));
        let _e278 = diffuse;
        diffuse = (_e278 + ((vec3<f32>(1.0) - _e272) * _e276));
        let _e280 = NdotL_1;
        let _e281 = specular;
        let _e282 = diffuse;
        let _e284 = ((_e281 + _e282) * _e280);
        col[0u] = _e284.x;
        col[1u] = _e284.y;
        col[2u] = _e284.z;
    }
    let _e291 = pbrParam_1;
    param_9 = _e291;
    let _e292 = v_1;
    param_10 = _e292;
    let _e293 = n_2;
    param_11 = _e293;
    let _e294 = ComputeIBLstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31vf3vf3_((&param_9), (&param_10), (&param_11));
    let _e295 = col;
    let _e297 = (_e295.xyz + _e294);
    col[0u] = _e297.x;
    col[1u] = _e297.y;
    col[2u] = _e297.z;
    let _e305 = ubo.useOcclusionTexture;
    if (_e305 != 0) {
        let _e307 = f_Texcoord_1;
        let _e308 = textureSample(occlusionTexture, occlusionTextureSampler, _e307);
        ao = _e308.x;
        let _e310 = col;
        let _e312 = col;
        let _e314 = ao;
        let _e317 = ubo.occlusionStrength;
        let _e319 = mix(_e310.xyz, (_e312.xyz * _e314), vec3<f32>(_e317));
        col[0u] = _e319.x;
        col[1u] = _e319.y;
        col[2u] = _e319.z;
    }
    let _e327 = ubo.useEmissiveTexture;
    if (_e327 != 0) {
        let _e329 = f_Texcoord_1;
        let _e330 = textureSample(emissiveTexture, emissiveTextureSampler, _e329);
        param_12 = _e330;
        let _e331 = SRGBtoLINEARvf4_((&param_12));
        let _e334 = ubo.emissiveFactor;
        emissive = (_e331.xyz * _e334.xyz);
        let _e337 = emissive;
        let _e338 = col;
        let _e340 = (_e338.xyz + _e337);
        col[0u] = _e340.x;
        col[1u] = _e340.y;
        col[2u] = _e340.z;
    }
    let _e348 = ubo.useShadowMap;
    if (_e348 != 0) {
        let _e350 = f_LightSpacePos_1;
        let _e353 = f_LightSpacePos_1[3u];
        lsp_1 = (_e350.xyz / vec3<f32>(_e353));
        let _e356 = lsp_1;
        lsp_1 = ((_e356 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e360 = lsp_1;
        param_13 = _e360;
        let _e361 = n_2;
        param_14 = _e361;
        let _e362 = l;
        param_15 = _e362;
        let _e363 = CalcShadowvf3vf3vf3_((&param_13), (&param_14), (&param_15));
        shadowCol = _e363;
        let _e364 = shadowCol;
        let _e365 = col;
        let _e367 = (_e365.xyz * _e364);
        col[0u] = _e367.x;
        col[1u] = _e367.y;
        col[2u] = _e367.z;
    }
    let _e374 = col;
    let _e376 = pow(_e374.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e376.x;
    col[1u] = _e376.y;
    col[2u] = _e376.z;
    let _e384 = baseColor_1[3u];
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
