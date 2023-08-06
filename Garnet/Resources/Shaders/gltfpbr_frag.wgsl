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
    t_pad_0_: i32,
    t_pad_1_: i32,
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
var<private> outColor: vec4<f32>;
var<private> f_LightSpacePos_1: vec4<f32>;
@group(0) @binding(13) 
var shadowmapTexture: texture_2d<f32>;
@group(0) @binding(14) 
var shadowmapTextureSampler: sampler;

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e67 = (*srgbIn);
    let _e69 = pow(_e67.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e71 = (*srgbIn)[3u];
    return vec4<f32>(_e69.x, _e69.y, _e69.z, _e71);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e67 = (*srgbIn_1);
    let _e69 = pow(_e67.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e71 = (*srgbIn_1)[3u];
    return vec4<f32>(_e69.x, _e69.y, _e69.z, _e71);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e68 = (*param).diffuseColor;
    return (_e68 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e68 = (*param_1).reflectance0_;
    let _e70 = (*param_1).reflectance90_;
    let _e72 = (*param_1).reflectance0_;
    let _e75 = (*param_1).VdotH;
    return (_e68 + ((_e70 - _e72) * pow(clamp((1.0 - _e75), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e73 = (*param_2).NdotL;
    NdotL = _e73;
    let _e75 = (*param_2).NdotV;
    NdotV = _e75;
    let _e77 = (*param_2).alphaRoughness;
    r = _e77;
    let _e78 = NdotL;
    let _e80 = NdotL;
    let _e81 = r;
    let _e82 = r;
    let _e84 = r;
    let _e85 = r;
    let _e88 = NdotL;
    let _e89 = NdotL;
    attenuationL = ((2.0 * _e78) / (_e80 + sqrt(((_e81 * _e82) + ((1.0 - (_e84 * _e85)) * (_e88 * _e89))))));
    let _e96 = NdotV;
    let _e98 = NdotV;
    let _e99 = r;
    let _e100 = r;
    let _e102 = r;
    let _e103 = r;
    let _e106 = NdotV;
    let _e107 = NdotV;
    attenuationV = ((2.0 * _e96) / (_e98 + sqrt(((_e99 * _e100) + ((1.0 - (_e102 * _e103)) * (_e106 * _e107))))));
    let _e114 = attenuationL;
    let _e115 = attenuationV;
    return (_e114 * _e115);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e70 = (*param_3).alphaRoughness;
    let _e72 = (*param_3).alphaRoughness;
    roughness2_ = (_e70 * _e72);
    let _e75 = (*param_3).NdotH;
    let _e76 = roughness2_;
    let _e79 = (*param_3).NdotH;
    let _e82 = (*param_3).NdotH;
    f = ((((_e75 * _e76) - _e79) * _e82) + 1.0);
    let _e85 = roughness2_;
    let _e86 = f;
    let _e88 = f;
    return (_e85 / ((3.1415927410125732 * _e86) * _e88));
}

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e72 = ubo.useNormalTexture;
    if (_e72 != 0) {
        let _e74 = f_WorldTangent_1;
        t = normalize(_e74);
        let _e76 = f_WorldBioTangent_1;
        b = normalize(_e76);
        let _e78 = f_WorldNormal_1;
        n = normalize(_e78);
        let _e80 = t;
        let _e81 = b;
        let _e82 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e80.x, _e80.y, _e80.z), vec3<f32>(_e81.x, _e81.y, _e81.z), vec3<f32>(_e82.x, _e82.y, _e82.z));
        let _e96 = f_Texcoord_1;
        let _e97 = textureSample(normalTexture, normalTextureSampler, _e96);
        nomral = _e97.xyz;
        let _e99 = tbn;
        let _e100 = nomral;
        let _e105 = ubo.normalMapScale;
        let _e107 = ubo.normalMapScale;
        nomral = normalize((_e99 * (((_e100 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e105, _e107, 1.0))));
    } else {
        let _e112 = f_WorldNormal_1;
        nomral = _e112;
    }
    let _e113 = nomral;
    return _e113;
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
    var param_4: PBRParam;
    var G: f32;
    var param_5: PBRParam;
    var F: vec3<f32>;
    var param_6: PBRParam;
    var specularBRDF: vec3<f32>;
    var diffuseBRDF: vec3<f32>;
    var param_7: PBRParam;
    var mipCount: f32;
    var lod: f32;
    var reflectColor: vec3<f32>;
    var param_8: vec4<f32>;
    var ao: f32;
    var emissive: vec3<f32>;
    var param_9: vec4<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e107 = ubo.roughnessFactor;
    perceptualRoughness = _e107;
    let _e109 = ubo.metallicFactor;
    metallic = _e109;
    let _e111 = ubo.useMetallicRoughnessTexture;
    if (_e111 != 0) {
        let _e113 = f_Texcoord_1;
        let _e114 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e113);
        metallicRoughnessColor = _e114;
        let _e115 = perceptualRoughness;
        let _e117 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e115 * _e117);
        let _e119 = metallic;
        let _e121 = metallicRoughnessColor[2u];
        metallic = (_e119 * _e121);
    }
    let _e123 = perceptualRoughness;
    perceptualRoughness = clamp(_e123, 0.03999999910593033, 1.0);
    let _e125 = metallic;
    metallic = clamp(_e125, 0.0, 1.0);
    let _e127 = perceptualRoughness;
    let _e128 = perceptualRoughness;
    alphaRoughness = (_e127 * _e128);
    let _e131 = ubo.useBaseColorTexture;
    if (_e131 != 0) {
        let _e133 = f_Texcoord_1;
        let _e134 = textureSample(baseColorTexture, baseColorTextureSampler, _e133);
        baseColor = _e134;
    } else {
        let _e136 = ubo.baseColorFactor;
        baseColor = _e136;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e137 = baseColor;
    let _e139 = f0_;
    diffuseColor = (_e137.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e139));
    let _e142 = metallic;
    let _e144 = diffuseColor;
    diffuseColor = (_e144 * (1.0 - _e142));
    let _e146 = f0_;
    let _e147 = baseColor;
    let _e149 = metallic;
    specularColor = mix(_e146, _e147.xyz, vec3<f32>(_e149));
    let _e153 = specularColor[0u];
    let _e155 = specularColor[1u];
    let _e158 = specularColor[2u];
    reflectance = max(max(_e153, _e155), _e158);
    let _e160 = reflectance;
    reflectance90_ = clamp((_e160 * 25.0), 0.0, 1.0);
    let _e163 = specularColor;
    specularEnvironmentR0_ = _e163;
    let _e164 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e164);
    let _e166 = getNormal();
    n_1 = _e166;
    let _e168 = ubo.cameraPos;
    let _e170 = f_WorldPos_1;
    v = normalize((_e168.xyz - _e170.xyz));
    let _e175 = ubo.lightDir;
    l = normalize(_e175.xyz);
    let _e178 = v;
    let _e179 = l;
    h = normalize((_e178 + _e179));
    let _e182 = v;
    let _e183 = n_1;
    reflection = -(normalize(reflect(_e182, _e183)));
    let _e187 = n_1;
    let _e188 = l;
    NdotL_1 = clamp(dot(_e187, _e188), 0.0010000000474974513, 1.0);
    let _e191 = n_1;
    let _e192 = v;
    NdotV_1 = clamp(abs(dot(_e191, _e192)), 0.0010000000474974513, 1.0);
    let _e196 = n_1;
    let _e197 = h;
    NdotH = clamp(dot(_e196, _e197), 0.0, 1.0);
    let _e200 = l;
    let _e201 = h;
    LdotH = clamp(dot(_e200, _e201), 0.0, 1.0);
    let _e204 = v;
    let _e205 = h;
    VdotH = clamp(dot(_e204, _e205), 0.0, 1.0);
    let _e208 = NdotL_1;
    let _e209 = NdotV_1;
    let _e210 = NdotH;
    let _e211 = LdotH;
    let _e212 = VdotH;
    let _e213 = perceptualRoughness;
    let _e214 = metallic;
    let _e215 = specularEnvironmentR0_;
    let _e216 = specularEnvironmentR90_;
    let _e217 = alphaRoughness;
    let _e218 = diffuseColor;
    let _e219 = specularColor;
    pbrParam = PBRParam(_e208, _e209, _e210, _e211, _e212, _e213, _e214, _e215, _e216, _e217, _e218, _e219);
    let _e221 = pbrParam;
    param_4 = _e221;
    let _e222 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e222;
    let _e223 = pbrParam;
    param_5 = _e223;
    let _e224 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e224;
    let _e225 = pbrParam;
    param_6 = _e225;
    let _e226 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e226;
    let _e227 = D;
    let _e228 = G;
    let _e230 = F;
    let _e232 = NdotL_1;
    let _e234 = NdotV_1;
    specularBRDF = ((_e230 * (_e227 * _e228)) / vec3<f32>(((4.0 * _e232) * _e234)));
    let _e238 = F;
    let _e241 = pbrParam;
    param_7 = _e241;
    let _e242 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e238) * _e242);
    let _e245 = ubo.mipCount;
    mipCount = _e245;
    let _e246 = mipCount;
    let _e247 = perceptualRoughness;
    lod = (_e246 * _e247);
    let _e249 = v;
    let _e250 = n_1;
    let _e252 = lod;
    let _e253 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e249, _e250), _e252);
    param_8 = _e253;
    let _e254 = LINEARtoSRGBvf4_((&param_8));
    reflectColor = _e254.xyz;
    let _e256 = NdotL_1;
    let _e258 = ubo.lightColor;
    let _e261 = specularBRDF;
    let _e262 = diffuseBRDF;
    let _e265 = reflectColor;
    let _e266 = specularColor;
    let _e268 = (((_e258.xyz * _e256) * (_e261 + _e262)) + (_e265 * _e266));
    col[0u] = _e268.x;
    col[1u] = _e268.y;
    col[2u] = _e268.z;
    let _e276 = ubo.useOcclusionTexture;
    if (_e276 != 0) {
        let _e278 = f_Texcoord_1;
        let _e279 = textureSample(occlusionTexture, occlusionTextureSampler, _e278);
        ao = _e279.x;
        let _e281 = col;
        let _e283 = col;
        let _e285 = ao;
        let _e288 = ubo.occlusionStrength;
        let _e290 = mix(_e281.xyz, (_e283.xyz * _e285), vec3<f32>(_e288));
        col[0u] = _e290.x;
        col[1u] = _e290.y;
        col[2u] = _e290.z;
    }
    let _e298 = ubo.useEmissiveTexture;
    if (_e298 != 0) {
        let _e300 = f_Texcoord_1;
        let _e301 = textureSample(emissiveTexture, emissiveTextureSampler, _e300);
        param_9 = _e301;
        let _e302 = SRGBtoLINEARvf4_((&param_9));
        let _e305 = ubo.emissiveFactor;
        emissive = (_e302.xyz * _e305.xyz);
        let _e308 = emissive;
        let _e309 = col;
        let _e311 = (_e309.xyz + _e308);
        col[0u] = _e311.x;
        col[1u] = _e311.y;
        col[2u] = _e311.z;
    }
    let _e318 = col;
    let _e320 = pow(_e318.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e320.x;
    col[1u] = _e320.y;
    col[2u] = _e320.z;
    let _e328 = baseColor[3u];
    col[3u] = _e328;
    let _e330 = col;
    outColor = _e330;
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
