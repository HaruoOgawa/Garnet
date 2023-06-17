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
    let _e61 = (*srgbIn);
    let _e63 = pow(_e61.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e65 = (*srgbIn)[3u];
    return vec4<f32>(_e63.x, _e63.y, _e63.z, _e65);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e62 = (*param).diffuseColor;
    return (_e62 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e62 = (*param_1).reflectance0_;
    let _e64 = (*param_1).reflectance90_;
    let _e66 = (*param_1).reflectance0_;
    let _e69 = (*param_1).VdotH;
    return (_e62 + ((_e64 - _e66) * pow(clamp((1.0 - _e69), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e67 = (*param_2).NdotL;
    NdotL = _e67;
    let _e69 = (*param_2).NdotV;
    NdotV = _e69;
    let _e71 = (*param_2).alphaRoughness;
    r = _e71;
    let _e72 = NdotL;
    let _e74 = NdotL;
    let _e75 = r;
    let _e76 = r;
    let _e78 = r;
    let _e79 = r;
    let _e82 = NdotL;
    let _e83 = NdotL;
    attenuationL = ((2.0 * _e72) / (_e74 + sqrt(((_e75 * _e76) + ((1.0 - (_e78 * _e79)) * (_e82 * _e83))))));
    let _e90 = NdotV;
    let _e92 = NdotV;
    let _e93 = r;
    let _e94 = r;
    let _e96 = r;
    let _e97 = r;
    let _e100 = NdotV;
    let _e101 = NdotV;
    attenuationV = ((2.0 * _e90) / (_e92 + sqrt(((_e93 * _e94) + ((1.0 - (_e96 * _e97)) * (_e100 * _e101))))));
    let _e108 = attenuationL;
    let _e109 = attenuationV;
    return (_e108 * _e109);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e64 = (*param_3).alphaRoughness;
    let _e66 = (*param_3).alphaRoughness;
    roughness2_ = (_e64 * _e66);
    let _e69 = (*param_3).NdotH;
    let _e70 = roughness2_;
    let _e73 = (*param_3).NdotH;
    let _e76 = (*param_3).NdotH;
    f = ((((_e69 * _e70) - _e73) * _e76) + 1.0);
    let _e79 = roughness2_;
    let _e80 = f;
    let _e82 = f;
    return (_e79 / ((3.1415927410125732 * _e80) * _e82));
}

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e66 = ubo.useNormalTexture;
    if (_e66 != 0) {
        let _e68 = f_WorldTangent_1;
        t = normalize(_e68.xyz);
        let _e71 = f_WorldTangent_1;
        b = normalize(_e71.xyz);
        let _e74 = f_WorldNormal_1;
        n = normalize(_e74);
        let _e76 = t;
        let _e77 = b;
        let _e78 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e76.x, _e76.y, _e76.z), vec3<f32>(_e77.x, _e77.y, _e77.z), vec3<f32>(_e78.x, _e78.y, _e78.z));
        let _e92 = f_Texcoord_1;
        let _e93 = textureSample(normalTexture, normalTextureSampler, _e92);
        nomral = _e93.xyz;
        let _e95 = tbn;
        let _e96 = nomral;
        let _e101 = ubo.normalMapScale;
        let _e103 = ubo.normalMapScale;
        nomral = normalize((_e95 * (((_e96 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e101, _e103, 1.0))));
    } else {
        let _e108 = f_WorldNormal_1;
        nomral = _e108;
    }
    let _e109 = nomral;
    return _e109;
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
    var ao: f32;
    var emissive: vec3<f32>;
    var param_8: vec4<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e97 = ubo.roughnessFactor;
    perceptualRoughness = _e97;
    let _e99 = ubo.metallicFactor;
    metallic = _e99;
    let _e101 = ubo.useMetallicRoughnessTexture;
    if (_e101 != 0) {
        let _e103 = f_Texcoord_1;
        let _e104 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e103);
        metallicRoughnessColor = _e104;
        let _e105 = perceptualRoughness;
        let _e107 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e105 * _e107);
        let _e109 = metallic;
        let _e111 = metallicRoughnessColor[2u];
        metallic = (_e109 * _e111);
    }
    let _e113 = perceptualRoughness;
    perceptualRoughness = clamp(_e113, 0.03999999910593033, 1.0);
    let _e115 = metallic;
    metallic = clamp(_e115, 0.0, 1.0);
    let _e117 = perceptualRoughness;
    let _e118 = perceptualRoughness;
    alphaRoughness = (_e117 * _e118);
    let _e121 = ubo.useBaseColorTexture;
    if (_e121 != 0) {
        let _e123 = f_Texcoord_1;
        let _e124 = textureSample(baseColorTexture, baseColorTextureSampler, _e123);
        baseColor = _e124;
    } else {
        let _e126 = ubo.baseColorFactor;
        baseColor = _e126;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e127 = baseColor;
    let _e129 = f0_;
    diffuseColor = (_e127.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e129));
    let _e132 = metallic;
    let _e134 = diffuseColor;
    diffuseColor = (_e134 * (1.0 - _e132));
    let _e136 = f0_;
    let _e137 = baseColor;
    let _e139 = metallic;
    specularColor = mix(_e136, _e137.xyz, vec3<f32>(_e139));
    let _e143 = specularColor[0u];
    let _e145 = specularColor[1u];
    let _e148 = specularColor[2u];
    reflectance = max(max(_e143, _e145), _e148);
    let _e150 = reflectance;
    reflectance90_ = clamp((_e150 * 25.0), 0.0, 1.0);
    let _e153 = specularColor;
    specularEnvironmentR0_ = _e153;
    let _e154 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e154);
    let _e156 = getNormal();
    n_1 = _e156;
    let _e158 = ubo.cameraPos;
    let _e160 = f_WorldPos_1;
    v = normalize((_e158.xyz - _e160.xyz));
    let _e165 = ubo.lightDir;
    l = normalize(_e165.xyz);
    let _e168 = v;
    let _e169 = l;
    h = normalize((_e168 + _e169));
    let _e172 = v;
    let _e173 = n_1;
    reflection = -(normalize(reflect(_e172, _e173)));
    let _e177 = n_1;
    let _e178 = l;
    NdotL_1 = clamp(dot(_e177, _e178), 0.0010000000474974513, 1.0);
    let _e181 = n_1;
    let _e182 = v;
    NdotV_1 = clamp(abs(dot(_e181, _e182)), 0.0010000000474974513, 1.0);
    let _e186 = n_1;
    let _e187 = h;
    NdotH = clamp(dot(_e186, _e187), 0.0, 1.0);
    let _e190 = l;
    let _e191 = h;
    LdotH = clamp(dot(_e190, _e191), 0.0, 1.0);
    let _e194 = v;
    let _e195 = h;
    VdotH = clamp(dot(_e194, _e195), 0.0, 1.0);
    let _e198 = NdotL_1;
    let _e199 = NdotV_1;
    let _e200 = NdotH;
    let _e201 = LdotH;
    let _e202 = VdotH;
    let _e203 = perceptualRoughness;
    let _e204 = metallic;
    let _e205 = specularEnvironmentR0_;
    let _e206 = specularEnvironmentR90_;
    let _e207 = alphaRoughness;
    let _e208 = diffuseColor;
    let _e209 = specularColor;
    pbrParam = PBRParam(_e198, _e199, _e200, _e201, _e202, _e203, _e204, _e205, _e206, _e207, _e208, _e209);
    let _e211 = pbrParam;
    param_4 = _e211;
    let _e212 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e212;
    let _e213 = pbrParam;
    param_5 = _e213;
    let _e214 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e214;
    let _e215 = pbrParam;
    param_6 = _e215;
    let _e216 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e216;
    let _e217 = D;
    let _e218 = G;
    let _e220 = F;
    let _e222 = NdotL_1;
    let _e224 = NdotV_1;
    specularBRDF = ((_e220 * (_e217 * _e218)) / vec3<f32>(((4.0 * _e222) * _e224)));
    let _e228 = F;
    let _e231 = pbrParam;
    param_7 = _e231;
    let _e232 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e228) * _e232);
    let _e234 = NdotL_1;
    let _e236 = ubo.lightColor;
    let _e239 = specularBRDF;
    let _e240 = diffuseBRDF;
    let _e242 = ((_e236.xyz * _e234) * (_e239 + _e240));
    col[0u] = _e242.x;
    col[1u] = _e242.y;
    col[2u] = _e242.z;
    let _e250 = ubo.useOcclusionTexture;
    if (_e250 != 0) {
        let _e252 = f_Texcoord_1;
        let _e253 = textureSample(occlusionTexture, occlusionTextureSampler, _e252);
        ao = _e253.x;
        let _e255 = col;
        let _e257 = col;
        let _e259 = ao;
        let _e262 = ubo.occlusionStrength;
        let _e264 = mix(_e255.xyz, (_e257.xyz * _e259), vec3<f32>(_e262));
        col[0u] = _e264.x;
        col[1u] = _e264.y;
        col[2u] = _e264.z;
    }
    let _e272 = ubo.useEmissiveTexture;
    if (_e272 != 0) {
        let _e274 = f_Texcoord_1;
        let _e275 = textureSample(emissiveTexture, emissiveTextureSampler, _e274);
        param_8 = _e275;
        let _e276 = SRGBtoLINEARvf4_((&param_8));
        let _e279 = ubo.emissiveFactor;
        emissive = (_e276.xyz * _e279.xyz);
        let _e282 = emissive;
        let _e283 = col;
        let _e285 = (_e283.xyz + _e282);
        col[0u] = _e285.x;
        col[1u] = _e285.y;
        col[2u] = _e285.z;
    }
    let _e292 = col;
    let _e294 = pow(_e292.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e294.x;
    col[1u] = _e294.y;
    col[2u] = _e294.z;
    let _e302 = baseColor[3u];
    col[3u] = _e302;
    let _e304 = col;
    outColor = _e304;
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
