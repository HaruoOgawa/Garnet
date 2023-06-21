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
    s_pad0_: f32,
    s_pad1_: f32,
    s_pad2_: f32,
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
var<private> f_WorldTangent_1: vec4<f32>;
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
var<private> f_WorldBioTangent_1: vec4<f32>;

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e63 = (*srgbIn);
    let _e65 = pow(_e63.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e67 = (*srgbIn)[3u];
    return vec4<f32>(_e65.x, _e65.y, _e65.z, _e67);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e64 = (*param).diffuseColor;
    return (_e64 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e64 = (*param_1).reflectance0_;
    let _e66 = (*param_1).reflectance90_;
    let _e68 = (*param_1).reflectance0_;
    let _e71 = (*param_1).VdotH;
    return (_e64 + ((_e66 - _e68) * pow(clamp((1.0 - _e71), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e69 = (*param_2).NdotL;
    NdotL = _e69;
    let _e71 = (*param_2).NdotV;
    NdotV = _e71;
    let _e73 = (*param_2).alphaRoughness;
    r = _e73;
    let _e74 = NdotL;
    let _e76 = NdotL;
    let _e77 = r;
    let _e78 = r;
    let _e80 = r;
    let _e81 = r;
    let _e84 = NdotL;
    let _e85 = NdotL;
    attenuationL = ((2.0 * _e74) / (_e76 + sqrt(((_e77 * _e78) + ((1.0 - (_e80 * _e81)) * (_e84 * _e85))))));
    let _e92 = NdotV;
    let _e94 = NdotV;
    let _e95 = r;
    let _e96 = r;
    let _e98 = r;
    let _e99 = r;
    let _e102 = NdotV;
    let _e103 = NdotV;
    attenuationV = ((2.0 * _e92) / (_e94 + sqrt(((_e95 * _e96) + ((1.0 - (_e98 * _e99)) * (_e102 * _e103))))));
    let _e110 = attenuationL;
    let _e111 = attenuationV;
    return (_e110 * _e111);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e66 = (*param_3).alphaRoughness;
    let _e68 = (*param_3).alphaRoughness;
    roughness2_ = (_e66 * _e68);
    let _e71 = (*param_3).NdotH;
    let _e72 = roughness2_;
    let _e75 = (*param_3).NdotH;
    let _e78 = (*param_3).NdotH;
    f = ((((_e71 * _e72) - _e75) * _e78) + 1.0);
    let _e81 = roughness2_;
    let _e82 = f;
    let _e84 = f;
    return (_e81 / ((3.1415927410125732 * _e82) * _e84));
}

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e68 = ubo.useNormalTexture;
    if (_e68 != 0) {
        let _e70 = f_WorldTangent_1;
        t = normalize(_e70.xyz);
        let _e73 = f_WorldTangent_1;
        b = normalize(_e73.xyz);
        let _e76 = f_WorldNormal_1;
        n = normalize(_e76);
        let _e78 = t;
        let _e79 = b;
        let _e80 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e78.x, _e78.y, _e78.z), vec3<f32>(_e79.x, _e79.y, _e79.z), vec3<f32>(_e80.x, _e80.y, _e80.z));
        let _e94 = f_Texcoord_1;
        let _e95 = textureSample(normalTexture, normalTextureSampler, _e94);
        nomral = _e95.xyz;
        let _e97 = tbn;
        let _e98 = nomral;
        let _e103 = ubo.normalMapScale;
        let _e105 = ubo.normalMapScale;
        nomral = normalize((_e97 * (((_e98 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e103, _e105, 1.0))));
    } else {
        let _e110 = f_WorldNormal_1;
        nomral = _e110;
    }
    let _e111 = nomral;
    return _e111;
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
    var mipIndex: f32;
    var reflectColor: vec3<f32>;
    var ao: f32;
    var emissive: vec3<f32>;
    var param_8: vec4<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e101 = ubo.roughnessFactor;
    perceptualRoughness = _e101;
    let _e103 = ubo.metallicFactor;
    metallic = _e103;
    let _e105 = ubo.useMetallicRoughnessTexture;
    if (_e105 != 0) {
        let _e107 = f_Texcoord_1;
        let _e108 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e107);
        metallicRoughnessColor = _e108;
        let _e109 = perceptualRoughness;
        let _e111 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e109 * _e111);
        let _e113 = metallic;
        let _e115 = metallicRoughnessColor[2u];
        metallic = (_e113 * _e115);
    }
    let _e117 = perceptualRoughness;
    perceptualRoughness = clamp(_e117, 0.03999999910593033, 1.0);
    let _e119 = metallic;
    metallic = clamp(_e119, 0.0, 1.0);
    let _e121 = perceptualRoughness;
    let _e122 = perceptualRoughness;
    alphaRoughness = (_e121 * _e122);
    let _e125 = ubo.useBaseColorTexture;
    if (_e125 != 0) {
        let _e127 = f_Texcoord_1;
        let _e128 = textureSample(baseColorTexture, baseColorTextureSampler, _e127);
        baseColor = _e128;
    } else {
        let _e130 = ubo.baseColorFactor;
        baseColor = _e130;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e131 = baseColor;
    let _e133 = f0_;
    diffuseColor = (_e131.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e133));
    let _e136 = metallic;
    let _e138 = diffuseColor;
    diffuseColor = (_e138 * (1.0 - _e136));
    let _e140 = f0_;
    let _e141 = baseColor;
    let _e143 = metallic;
    specularColor = mix(_e140, _e141.xyz, vec3<f32>(_e143));
    let _e147 = specularColor[0u];
    let _e149 = specularColor[1u];
    let _e152 = specularColor[2u];
    reflectance = max(max(_e147, _e149), _e152);
    let _e154 = reflectance;
    reflectance90_ = clamp((_e154 * 25.0), 0.0, 1.0);
    let _e157 = specularColor;
    specularEnvironmentR0_ = _e157;
    let _e158 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e158);
    let _e160 = getNormal();
    n_1 = _e160;
    let _e162 = ubo.cameraPos;
    let _e164 = f_WorldPos_1;
    v = normalize((_e162.xyz - _e164.xyz));
    let _e169 = ubo.lightDir;
    l = normalize(_e169.xyz);
    let _e172 = v;
    let _e173 = l;
    h = normalize((_e172 + _e173));
    let _e176 = v;
    let _e177 = n_1;
    reflection = -(normalize(reflect(_e176, _e177)));
    let _e181 = n_1;
    let _e182 = l;
    NdotL_1 = clamp(dot(_e181, _e182), 0.0010000000474974513, 1.0);
    let _e185 = n_1;
    let _e186 = v;
    NdotV_1 = clamp(abs(dot(_e185, _e186)), 0.0010000000474974513, 1.0);
    let _e190 = n_1;
    let _e191 = h;
    NdotH = clamp(dot(_e190, _e191), 0.0, 1.0);
    let _e194 = l;
    let _e195 = h;
    LdotH = clamp(dot(_e194, _e195), 0.0, 1.0);
    let _e198 = v;
    let _e199 = h;
    VdotH = clamp(dot(_e198, _e199), 0.0, 1.0);
    let _e202 = NdotL_1;
    let _e203 = NdotV_1;
    let _e204 = NdotH;
    let _e205 = LdotH;
    let _e206 = VdotH;
    let _e207 = perceptualRoughness;
    let _e208 = metallic;
    let _e209 = specularEnvironmentR0_;
    let _e210 = specularEnvironmentR90_;
    let _e211 = alphaRoughness;
    let _e212 = diffuseColor;
    let _e213 = specularColor;
    pbrParam = PBRParam(_e202, _e203, _e204, _e205, _e206, _e207, _e208, _e209, _e210, _e211, _e212, _e213);
    let _e215 = pbrParam;
    param_4 = _e215;
    let _e216 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e216;
    let _e217 = pbrParam;
    param_5 = _e217;
    let _e218 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e218;
    let _e219 = pbrParam;
    param_6 = _e219;
    let _e220 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e220;
    let _e221 = D;
    let _e222 = G;
    let _e224 = F;
    let _e226 = NdotL_1;
    let _e228 = NdotV_1;
    specularBRDF = ((_e224 * (_e221 * _e222)) / vec3<f32>(((4.0 * _e226) * _e228)));
    let _e232 = F;
    let _e235 = pbrParam;
    param_7 = _e235;
    let _e236 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e232) * _e236);
    let _e238 = metallic;
    mipIndex = _e238;
    let _e239 = v;
    let _e240 = n_1;
    let _e242 = mipIndex;
    let _e243 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e239, _e240), _e242);
    reflectColor = _e243.xyz;
    let _e245 = NdotL_1;
    let _e247 = ubo.lightColor;
    let _e250 = specularBRDF;
    let _e251 = diffuseBRDF;
    let _e254 = reflectColor;
    let _e255 = (((_e247.xyz * _e245) * (_e250 + _e251)) + _e254);
    col[0u] = _e255.x;
    col[1u] = _e255.y;
    col[2u] = _e255.z;
    let _e263 = ubo.useOcclusionTexture;
    if (_e263 != 0) {
        let _e265 = f_Texcoord_1;
        let _e266 = textureSample(occlusionTexture, occlusionTextureSampler, _e265);
        ao = _e266.x;
        let _e268 = col;
        let _e270 = col;
        let _e272 = ao;
        let _e275 = ubo.occlusionStrength;
        let _e277 = mix(_e268.xyz, (_e270.xyz * _e272), vec3<f32>(_e275));
        col[0u] = _e277.x;
        col[1u] = _e277.y;
        col[2u] = _e277.z;
    }
    let _e285 = ubo.useEmissiveTexture;
    if (_e285 != 0) {
        let _e287 = f_Texcoord_1;
        let _e288 = textureSample(emissiveTexture, emissiveTextureSampler, _e287);
        param_8 = _e288;
        let _e289 = SRGBtoLINEARvf4_((&param_8));
        let _e292 = ubo.emissiveFactor;
        emissive = (_e289.xyz * _e292.xyz);
        let _e295 = emissive;
        let _e296 = col;
        let _e298 = (_e296.xyz + _e295);
        col[0u] = _e298.x;
        col[1u] = _e298.y;
        col[2u] = _e298.z;
    }
    let _e305 = col;
    let _e307 = pow(_e305.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e307.x;
    col[1u] = _e307.y;
    col[2u] = _e307.z;
    let _e315 = baseColor[3u];
    col[3u] = _e315;
    let _e317 = col;
    outColor = _e317;
    return;
}

@fragment 
fn main(@location(3) f_WorldTangent: vec4<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(1) f_Texcoord: vec2<f32>, @location(2) f_WorldPos: vec4<f32>, @location(4) f_WorldBioTangent: vec4<f32>) -> @location(0) vec4<f32> {
    f_WorldTangent_1 = f_WorldTangent;
    f_WorldNormal_1 = f_WorldNormal;
    f_Texcoord_1 = f_Texcoord;
    f_WorldPos_1 = f_WorldPos;
    f_WorldBioTangent_1 = f_WorldBioTangent;
    main_1();
    let _e11 = outColor;
    return _e11;
}
