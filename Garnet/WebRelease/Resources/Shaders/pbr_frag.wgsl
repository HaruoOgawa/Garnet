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
@group(0) @binding(15) 
var IBL_Diffuse_Texture: texture_2d<f32>;
@group(0) @binding(16) 
var IBL_Diffuse_TextureSampler: sampler;
@group(0) @binding(17) 
var IBL_Specular_Texture: texture_2d<f32>;
@group(0) @binding(18) 
var IBL_Specular_TextureSampler: sampler;
@group(0) @binding(19) 
var IBL_GGXLUT_Texture: texture_2d<f32>;
@group(0) @binding(20) 
var IBL_GGXLUT_TextureSampler: sampler;

fn ComputePCFvf2_(uv: ptr<function, vec2<f32>>) -> vec2<f32> {
    var moments: vec2<f32>;
    var texelSize: vec2<f32>;

    moments = vec2<f32>(0.0, 0.0);
    let _e92 = ubo.ShadowMapX;
    let _e95 = ubo.ShadowMapY;
    texelSize = vec2<f32>((1.0 / _e92), (1.0 / _e95));
    let _e98 = (*uv);
    let _e99 = texelSize;
    let _e102 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e98 + (vec2<f32>(-1.0, -1.0) * _e99)));
    let _e104 = moments;
    moments = (_e104 + _e102.xy);
    let _e106 = (*uv);
    let _e107 = texelSize;
    let _e110 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e106 + (vec2<f32>(-1.0, 0.0) * _e107)));
    let _e112 = moments;
    moments = (_e112 + _e110.xy);
    let _e114 = (*uv);
    let _e115 = texelSize;
    let _e118 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e114 + (vec2<f32>(-1.0, 1.0) * _e115)));
    let _e120 = moments;
    moments = (_e120 + _e118.xy);
    let _e122 = (*uv);
    let _e123 = texelSize;
    let _e126 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e122 + (vec2<f32>(0.0, -1.0) * _e123)));
    let _e128 = moments;
    moments = (_e128 + _e126.xy);
    let _e130 = (*uv);
    let _e131 = texelSize;
    let _e134 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e130 + (vec2<f32>(0.0, 0.0) * _e131)));
    let _e136 = moments;
    moments = (_e136 + _e134.xy);
    let _e138 = (*uv);
    let _e139 = texelSize;
    let _e142 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e138 + (vec2<f32>(0.0, 1.0) * _e139)));
    let _e144 = moments;
    moments = (_e144 + _e142.xy);
    let _e146 = (*uv);
    let _e147 = texelSize;
    let _e150 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e146 + (vec2<f32>(1.0, -1.0) * _e147)));
    let _e152 = moments;
    moments = (_e152 + _e150.xy);
    let _e154 = (*uv);
    let _e155 = texelSize;
    let _e158 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e154 + (vec2<f32>(1.0, 0.0) * _e155)));
    let _e160 = moments;
    moments = (_e160 + _e158.xy);
    let _e162 = (*uv);
    let _e163 = texelSize;
    let _e166 = textureSample(shadowmapTexture, shadowmapTextureSampler, (_e162 + (vec2<f32>(1.0, 1.0) * _e163)));
    let _e168 = moments;
    moments = (_e168 + _e166.xy);
    let _e170 = moments;
    moments = (_e170 / vec2<f32>(9.0));
    let _e173 = moments;
    return _e173;
}

fn CalcShadowvf3vf3vf3_(lsp: ptr<function, vec3<f32>>, nomral: ptr<function, vec3<f32>>, lightDir: ptr<function, vec3<f32>>) -> f32 {
    var moments_1: vec2<f32>;
    var param: vec2<f32>;
    var ShadowBias: f32;
    var distance: f32;
    var variance: f32;
    var d: f32;
    var p_max: f32;

    let _e98 = (*lsp);
    param = _e98.xy;
    let _e100 = ComputePCFvf2_((&param));
    moments_1 = _e100;
    let _e101 = (*nomral);
    let _e102 = (*lightDir);
    ShadowBias = max(0.004999999888241291, (0.05000000074505806 * (1.0 - dot(_e101, _e102))));
    let _e108 = (*lsp)[2u];
    let _e109 = ShadowBias;
    distance = (_e108 - _e109);
    let _e111 = distance;
    let _e113 = moments_1[0u];
    if (_e111 <= _e113) {
        return 1.0;
    }
    let _e116 = moments_1[1u];
    let _e118 = moments_1[0u];
    let _e120 = moments_1[0u];
    variance = (_e116 - (_e118 * _e120));
    let _e123 = variance;
    variance = max(0.004999999888241291, _e123);
    let _e125 = distance;
    let _e127 = moments_1[0u];
    d = (_e125 - _e127);
    let _e129 = variance;
    let _e130 = variance;
    let _e131 = d;
    let _e132 = d;
    p_max = (_e129 / (_e130 + (_e131 * _e132)));
    let _e136 = p_max;
    return _e136;
}

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e89 = (*srgbIn);
    let _e91 = pow(_e89.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e93 = (*srgbIn)[3u];
    return vec4<f32>(_e91.x, _e91.y, _e91.z, _e93);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e89 = (*srgbIn_1);
    let _e91 = pow(_e89.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e93 = (*srgbIn_1)[3u];
    return vec4<f32>(_e91.x, _e91.y, _e91.z, _e93);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e90 = (*param_1).diffuseColor;
    return (_e90 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> vec3<f32> {
    let _e90 = (*param_2).reflectance0_;
    let _e92 = (*param_2).reflectance90_;
    let _e94 = (*param_2).reflectance0_;
    let _e97 = (*param_2).VdotH;
    return (_e90 + ((_e92 - _e94) * pow(clamp((1.0 - _e97), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e95 = (*param_3).NdotL;
    NdotL = _e95;
    let _e97 = (*param_3).NdotV;
    NdotV = _e97;
    let _e99 = (*param_3).alphaRoughness;
    r = _e99;
    let _e100 = NdotL;
    let _e102 = NdotL;
    let _e103 = r;
    let _e104 = r;
    let _e106 = r;
    let _e107 = r;
    let _e110 = NdotL;
    let _e111 = NdotL;
    attenuationL = ((2.0 * _e100) / (_e102 + sqrt(((_e103 * _e104) + ((1.0 - (_e106 * _e107)) * (_e110 * _e111))))));
    let _e118 = NdotV;
    let _e120 = NdotV;
    let _e121 = r;
    let _e122 = r;
    let _e124 = r;
    let _e125 = r;
    let _e128 = NdotV;
    let _e129 = NdotV;
    attenuationV = ((2.0 * _e118) / (_e120 + sqrt(((_e121 * _e122) + ((1.0 - (_e124 * _e125)) * (_e128 * _e129))))));
    let _e136 = attenuationL;
    let _e137 = attenuationV;
    return (_e136 * _e137);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_4: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e92 = (*param_4).alphaRoughness;
    let _e94 = (*param_4).alphaRoughness;
    roughness2_ = (_e92 * _e94);
    let _e97 = (*param_4).NdotH;
    let _e98 = roughness2_;
    let _e101 = (*param_4).NdotH;
    let _e104 = (*param_4).NdotH;
    f = ((((_e97 * _e98) - _e101) * _e104) + 1.0);
    let _e107 = roughness2_;
    let _e108 = f;
    let _e110 = f;
    return (_e107 / ((3.1415927410125732 * _e108) * _e110));
}

fn getNormal() -> vec3<f32> {
    var nomral_1: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral_1 = vec3<f32>(0.0, 0.0, 0.0);
    let _e94 = ubo.useNormalTexture;
    if (_e94 != 0) {
        let _e96 = f_WorldTangent_1;
        t = normalize(_e96);
        let _e98 = f_WorldBioTangent_1;
        b = normalize(_e98);
        let _e100 = f_WorldNormal_1;
        n = normalize(_e100);
        let _e102 = t;
        let _e103 = b;
        let _e104 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e102.x, _e102.y, _e102.z), vec3<f32>(_e103.x, _e103.y, _e103.z), vec3<f32>(_e104.x, _e104.y, _e104.z));
        let _e118 = f_Texcoord_1;
        let _e119 = textureSample(normalTexture, normalTextureSampler, _e118);
        nomral_1 = _e119.xyz;
        let _e121 = tbn;
        let _e122 = nomral_1;
        let _e127 = ubo.normalMapScale;
        let _e129 = ubo.normalMapScale;
        nomral_1 = normalize((_e121 * (((_e122 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e127, _e129, 1.0))));
    } else {
        let _e134 = f_WorldNormal_1;
        nomral_1 = _e134;
    }
    let _e135 = nomral_1;
    return _e135;
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
    let _e135 = ubo.roughnessFactor;
    perceptualRoughness = _e135;
    let _e137 = ubo.metallicFactor;
    metallic = _e137;
    let _e139 = ubo.useMetallicRoughnessTexture;
    if (_e139 != 0) {
        let _e141 = f_Texcoord_1;
        let _e142 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e141);
        metallicRoughnessColor = _e142;
        let _e143 = perceptualRoughness;
        let _e145 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e143 * _e145);
        let _e147 = metallic;
        let _e149 = metallicRoughnessColor[2u];
        metallic = (_e147 * _e149);
    }
    let _e151 = perceptualRoughness;
    perceptualRoughness = clamp(_e151, 0.03999999910593033, 1.0);
    let _e153 = metallic;
    metallic = clamp(_e153, 0.0, 1.0);
    let _e155 = perceptualRoughness;
    let _e156 = perceptualRoughness;
    alphaRoughness = (_e155 * _e156);
    let _e159 = ubo.useBaseColorTexture;
    if (_e159 != 0) {
        let _e161 = f_Texcoord_1;
        let _e162 = textureSample(baseColorTexture, baseColorTextureSampler, _e161);
        baseColor = _e162;
    } else {
        let _e164 = ubo.baseColorFactor;
        baseColor = _e164;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e165 = baseColor;
    let _e167 = f0_;
    diffuseColor = (_e165.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e167));
    let _e170 = metallic;
    let _e172 = diffuseColor;
    diffuseColor = (_e172 * (1.0 - _e170));
    let _e174 = f0_;
    let _e175 = baseColor;
    let _e177 = metallic;
    specularColor = mix(_e174, _e175.xyz, vec3<f32>(_e177));
    let _e181 = specularColor[0u];
    let _e183 = specularColor[1u];
    let _e186 = specularColor[2u];
    reflectance = max(max(_e181, _e183), _e186);
    let _e188 = reflectance;
    reflectance90_ = clamp((_e188 * 25.0), 0.0, 1.0);
    let _e191 = specularColor;
    specularEnvironmentR0_ = _e191;
    let _e192 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e192);
    let _e194 = getNormal();
    n_1 = _e194;
    let _e195 = f_WorldPos_1;
    let _e198 = ubo.cameraPos;
    v = (normalize((_e195.xyz - _e198.xyz)) * -1.0);
    let _e204 = ubo.lightDir;
    l = (normalize(_e204.xyz) * -1.0);
    let _e208 = v;
    let _e209 = l;
    h = normalize((_e208 + _e209));
    let _e212 = v;
    let _e213 = n_1;
    reflection = normalize(reflect(_e212, _e213));
    let _e216 = n_1;
    let _e217 = l;
    NdotL_1 = clamp(dot(_e216, _e217), 0.0, 1.0);
    let _e220 = n_1;
    let _e221 = v;
    NdotV_1 = clamp(abs(dot(_e220, _e221)), 0.0, 1.0);
    let _e225 = n_1;
    let _e226 = h;
    NdotH = clamp(dot(_e225, _e226), 0.0, 1.0);
    let _e229 = l;
    let _e230 = h;
    LdotH = clamp(dot(_e229, _e230), 0.0, 1.0);
    let _e233 = v;
    let _e234 = h;
    VdotH = clamp(dot(_e233, _e234), 0.0, 1.0);
    let _e237 = NdotL_1;
    let _e238 = NdotV_1;
    let _e239 = NdotH;
    let _e240 = LdotH;
    let _e241 = VdotH;
    let _e242 = perceptualRoughness;
    let _e243 = metallic;
    let _e244 = specularEnvironmentR0_;
    let _e245 = specularEnvironmentR90_;
    let _e246 = alphaRoughness;
    let _e247 = diffuseColor;
    let _e248 = specularColor;
    pbrParam = PBRParam(_e237, _e238, _e239, _e240, _e241, _e242, _e243, _e244, _e245, _e246, _e247, _e248);
    specular = vec3<f32>(0.0, 0.0, 0.0);
    diffuse = vec3<f32>(0.0, 0.0, 0.0);
    let _e250 = NdotL_1;
    let _e252 = NdotV_1;
    if ((_e250 > 0.0) || (_e252 > 0.0)) {
        let _e255 = pbrParam;
        param_5 = _e255;
        let _e256 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
        D = _e256;
        let _e257 = pbrParam;
        param_6 = _e257;
        let _e258 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
        G = _e258;
        let _e259 = pbrParam;
        param_7 = _e259;
        let _e260 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
        F = _e260;
        let _e261 = D;
        let _e262 = G;
        let _e264 = F;
        let _e266 = NdotL_1;
        let _e268 = NdotV_1;
        let _e272 = specular;
        specular = (_e272 + ((_e264 * (_e261 * _e262)) / vec3<f32>(((4.0 * _e266) * _e268))));
        let _e274 = specular;
        specular = max(_e274, vec3<f32>(0.0, 0.0, 0.0));
        let _e276 = F;
        let _e279 = pbrParam;
        param_8 = _e279;
        let _e280 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_8));
        let _e282 = diffuse;
        diffuse = (_e282 + ((vec3<f32>(1.0) - _e276) * _e280));
        reflectColor = vec3<f32>(0.0, 0.0, 0.0);
        let _e285 = ubo.useCubeMap;
        if (_e285 != 0) {
            let _e288 = ubo.mipCount;
            mipCount = _e288;
            let _e289 = mipCount;
            let _e290 = perceptualRoughness;
            lod = (_e289 * _e290);
            let _e292 = v;
            let _e293 = n_1;
            let _e295 = lod;
            let _e296 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e292, _e293), _e295);
            param_9 = _e296;
            let _e297 = LINEARtoSRGBvf4_((&param_9));
            reflectColor = _e297.xyz;
        }
        let _e299 = NdotL_1;
        let _e300 = specular;
        let _e301 = diffuse;
        let _e304 = reflectColor;
        let _e305 = F;
        let _e307 = (((_e300 + _e301) * _e299) + (_e304 * _e305));
        col[0u] = _e307.x;
        col[1u] = _e307.y;
        col[2u] = _e307.z;
        let _e314 = specular;
        gi_diffuse = clamp(_e314, vec3<f32>(0.03999999910593033), vec3<f32>(1.0));
        let _e318 = gi_diffuse;
        let _e319 = diffuse;
        let _e321 = col;
        let _e323 = (_e321.xyz + (_e318 * _e319));
        col[0u] = _e323.x;
        col[1u] = _e323.y;
        col[2u] = _e323.z;
    }
    let _e331 = ubo.useOcclusionTexture;
    if (_e331 != 0) {
        let _e333 = f_Texcoord_1;
        let _e334 = textureSample(occlusionTexture, occlusionTextureSampler, _e333);
        ao = _e334.x;
        let _e336 = col;
        let _e338 = col;
        let _e340 = ao;
        let _e343 = ubo.occlusionStrength;
        let _e345 = mix(_e336.xyz, (_e338.xyz * _e340), vec3<f32>(_e343));
        col[0u] = _e345.x;
        col[1u] = _e345.y;
        col[2u] = _e345.z;
    }
    let _e353 = ubo.useEmissiveTexture;
    if (_e353 != 0) {
        let _e355 = f_Texcoord_1;
        let _e356 = textureSample(emissiveTexture, emissiveTextureSampler, _e355);
        param_10 = _e356;
        let _e357 = SRGBtoLINEARvf4_((&param_10));
        let _e360 = ubo.emissiveFactor;
        emissive = (_e357.xyz * _e360.xyz);
        let _e363 = emissive;
        let _e364 = col;
        let _e366 = (_e364.xyz + _e363);
        col[0u] = _e366.x;
        col[1u] = _e366.y;
        col[2u] = _e366.z;
    }
    let _e374 = ubo.useShadowMap;
    if (_e374 != 0) {
        let _e376 = f_LightSpacePos_1;
        let _e379 = f_LightSpacePos_1[3u];
        lsp_1 = (_e376.xyz / vec3<f32>(_e379));
        let _e382 = lsp_1;
        lsp_1 = ((_e382 * 0.5) + vec3<f32>(0.5));
        shadowCol = 1.0;
        let _e386 = lsp_1;
        param_11 = _e386;
        let _e387 = n_1;
        param_12 = _e387;
        let _e388 = l;
        param_13 = _e388;
        let _e389 = CalcShadowvf3vf3vf3_((&param_11), (&param_12), (&param_13));
        shadowCol = _e389;
        let _e390 = shadowCol;
        let _e391 = col;
        let _e393 = (_e391.xyz * _e390);
        col[0u] = _e393.x;
        col[1u] = _e393.y;
        col[2u] = _e393.z;
    }
    let _e400 = col;
    let _e402 = pow(_e400.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e402.x;
    col[1u] = _e402.y;
    col[2u] = _e402.z;
    let _e410 = baseColor[3u];
    col[3u] = _e410;
    let _e412 = col;
    outColor = _e412;
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
