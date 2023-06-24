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
    mipCount: f32,
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
    let _e64 = (*srgbIn);
    let _e66 = pow(_e64.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e68 = (*srgbIn)[3u];
    return vec4<f32>(_e66.x, _e66.y, _e66.z, _e68);
}

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e65 = (*param).diffuseColor;
    return (_e65 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e65 = (*param_1).reflectance0_;
    let _e67 = (*param_1).reflectance90_;
    let _e69 = (*param_1).reflectance0_;
    let _e72 = (*param_1).VdotH;
    return (_e65 + ((_e67 - _e69) * pow(clamp((1.0 - _e72), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e70 = (*param_2).NdotL;
    NdotL = _e70;
    let _e72 = (*param_2).NdotV;
    NdotV = _e72;
    let _e74 = (*param_2).alphaRoughness;
    r = _e74;
    let _e75 = NdotL;
    let _e77 = NdotL;
    let _e78 = r;
    let _e79 = r;
    let _e81 = r;
    let _e82 = r;
    let _e85 = NdotL;
    let _e86 = NdotL;
    attenuationL = ((2.0 * _e75) / (_e77 + sqrt(((_e78 * _e79) + ((1.0 - (_e81 * _e82)) * (_e85 * _e86))))));
    let _e93 = NdotV;
    let _e95 = NdotV;
    let _e96 = r;
    let _e97 = r;
    let _e99 = r;
    let _e100 = r;
    let _e103 = NdotV;
    let _e104 = NdotV;
    attenuationV = ((2.0 * _e93) / (_e95 + sqrt(((_e96 * _e97) + ((1.0 - (_e99 * _e100)) * (_e103 * _e104))))));
    let _e111 = attenuationL;
    let _e112 = attenuationV;
    return (_e111 * _e112);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e67 = (*param_3).alphaRoughness;
    let _e69 = (*param_3).alphaRoughness;
    roughness2_ = (_e67 * _e69);
    let _e72 = (*param_3).NdotH;
    let _e73 = roughness2_;
    let _e76 = (*param_3).NdotH;
    let _e79 = (*param_3).NdotH;
    f = ((((_e72 * _e73) - _e76) * _e79) + 1.0);
    let _e82 = roughness2_;
    let _e83 = f;
    let _e85 = f;
    return (_e82 / ((3.1415927410125732 * _e83) * _e85));
}

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e69 = ubo.useNormalTexture;
    if (_e69 != 0) {
        let _e71 = f_WorldTangent_1;
        t = normalize(_e71.xyz);
        let _e74 = f_WorldTangent_1;
        b = normalize(_e74.xyz);
        let _e77 = f_WorldNormal_1;
        n = normalize(_e77);
        let _e79 = t;
        let _e80 = b;
        let _e81 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e79.x, _e79.y, _e79.z), vec3<f32>(_e80.x, _e80.y, _e80.z), vec3<f32>(_e81.x, _e81.y, _e81.z));
        let _e95 = f_Texcoord_1;
        let _e96 = textureSample(normalTexture, normalTextureSampler, _e95);
        nomral = _e96.xyz;
        let _e98 = tbn;
        let _e99 = nomral;
        let _e104 = ubo.normalMapScale;
        let _e106 = ubo.normalMapScale;
        nomral = normalize((_e98 * (((_e99 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e104, _e106, 1.0))));
    } else {
        let _e111 = f_WorldNormal_1;
        nomral = _e111;
    }
    let _e112 = nomral;
    return _e112;
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
    var ao: f32;
    var emissive: vec3<f32>;
    var param_8: vec4<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e103 = ubo.roughnessFactor;
    perceptualRoughness = _e103;
    let _e105 = ubo.metallicFactor;
    metallic = _e105;
    let _e107 = ubo.useMetallicRoughnessTexture;
    if (_e107 != 0) {
        let _e109 = f_Texcoord_1;
        let _e110 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e109);
        metallicRoughnessColor = _e110;
        let _e111 = perceptualRoughness;
        let _e113 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e111 * _e113);
        let _e115 = metallic;
        let _e117 = metallicRoughnessColor[2u];
        metallic = (_e115 * _e117);
    }
    let _e119 = perceptualRoughness;
    perceptualRoughness = clamp(_e119, 0.03999999910593033, 1.0);
    let _e121 = metallic;
    metallic = clamp(_e121, 0.0, 1.0);
    let _e123 = perceptualRoughness;
    let _e124 = perceptualRoughness;
    alphaRoughness = (_e123 * _e124);
    let _e127 = ubo.useBaseColorTexture;
    if (_e127 != 0) {
        let _e129 = f_Texcoord_1;
        let _e130 = textureSample(baseColorTexture, baseColorTextureSampler, _e129);
        baseColor = _e130;
    } else {
        let _e132 = ubo.baseColorFactor;
        baseColor = _e132;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e133 = baseColor;
    let _e135 = f0_;
    diffuseColor = (_e133.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e135));
    let _e138 = metallic;
    let _e140 = diffuseColor;
    diffuseColor = (_e140 * (1.0 - _e138));
    let _e142 = f0_;
    let _e143 = baseColor;
    let _e145 = metallic;
    specularColor = mix(_e142, _e143.xyz, vec3<f32>(_e145));
    let _e149 = specularColor[0u];
    let _e151 = specularColor[1u];
    let _e154 = specularColor[2u];
    reflectance = max(max(_e149, _e151), _e154);
    let _e156 = reflectance;
    reflectance90_ = clamp((_e156 * 25.0), 0.0, 1.0);
    let _e159 = specularColor;
    specularEnvironmentR0_ = _e159;
    let _e160 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e160);
    let _e162 = getNormal();
    n_1 = _e162;
    let _e164 = ubo.cameraPos;
    let _e166 = f_WorldPos_1;
    v = normalize((_e164.xyz - _e166.xyz));
    let _e171 = ubo.lightDir;
    l = normalize(_e171.xyz);
    let _e174 = v;
    let _e175 = l;
    h = normalize((_e174 + _e175));
    let _e178 = v;
    let _e179 = n_1;
    reflection = -(normalize(reflect(_e178, _e179)));
    let _e183 = n_1;
    let _e184 = l;
    NdotL_1 = clamp(dot(_e183, _e184), 0.0010000000474974513, 1.0);
    let _e187 = n_1;
    let _e188 = v;
    NdotV_1 = clamp(abs(dot(_e187, _e188)), 0.0010000000474974513, 1.0);
    let _e192 = n_1;
    let _e193 = h;
    NdotH = clamp(dot(_e192, _e193), 0.0, 1.0);
    let _e196 = l;
    let _e197 = h;
    LdotH = clamp(dot(_e196, _e197), 0.0, 1.0);
    let _e200 = v;
    let _e201 = h;
    VdotH = clamp(dot(_e200, _e201), 0.0, 1.0);
    let _e204 = NdotL_1;
    let _e205 = NdotV_1;
    let _e206 = NdotH;
    let _e207 = LdotH;
    let _e208 = VdotH;
    let _e209 = perceptualRoughness;
    let _e210 = metallic;
    let _e211 = specularEnvironmentR0_;
    let _e212 = specularEnvironmentR90_;
    let _e213 = alphaRoughness;
    let _e214 = diffuseColor;
    let _e215 = specularColor;
    pbrParam = PBRParam(_e204, _e205, _e206, _e207, _e208, _e209, _e210, _e211, _e212, _e213, _e214, _e215);
    let _e217 = pbrParam;
    param_4 = _e217;
    let _e218 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e218;
    let _e219 = pbrParam;
    param_5 = _e219;
    let _e220 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e220;
    let _e221 = pbrParam;
    param_6 = _e221;
    let _e222 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e222;
    let _e223 = D;
    let _e224 = G;
    let _e226 = F;
    let _e228 = NdotL_1;
    let _e230 = NdotV_1;
    specularBRDF = ((_e226 * (_e223 * _e224)) / vec3<f32>(((4.0 * _e228) * _e230)));
    let _e234 = F;
    let _e237 = pbrParam;
    param_7 = _e237;
    let _e238 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e234) * _e238);
    let _e241 = ubo.mipCount;
    mipCount = _e241;
    let _e242 = mipCount;
    let _e243 = perceptualRoughness;
    lod = (_e242 * _e243);
    let _e245 = v;
    let _e246 = n_1;
    let _e248 = lod;
    let _e249 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e245, _e246), _e248);
    reflectColor = _e249.xyz;
    let _e251 = NdotL_1;
    let _e253 = ubo.lightColor;
    let _e256 = specularBRDF;
    let _e257 = diffuseBRDF;
    let _e260 = reflectColor;
    let _e261 = (((_e253.xyz * _e251) * (_e256 + _e257)) + _e260);
    col[0u] = _e261.x;
    col[1u] = _e261.y;
    col[2u] = _e261.z;
    let _e269 = ubo.useOcclusionTexture;
    if (_e269 != 0) {
        let _e271 = f_Texcoord_1;
        let _e272 = textureSample(occlusionTexture, occlusionTextureSampler, _e271);
        ao = _e272.x;
        let _e274 = col;
        let _e276 = col;
        let _e278 = ao;
        let _e281 = ubo.occlusionStrength;
        let _e283 = mix(_e274.xyz, (_e276.xyz * _e278), vec3<f32>(_e281));
        col[0u] = _e283.x;
        col[1u] = _e283.y;
        col[2u] = _e283.z;
    }
    let _e291 = ubo.useEmissiveTexture;
    if (_e291 != 0) {
        let _e293 = f_Texcoord_1;
        let _e294 = textureSample(emissiveTexture, emissiveTextureSampler, _e293);
        param_8 = _e294;
        let _e295 = SRGBtoLINEARvf4_((&param_8));
        let _e298 = ubo.emissiveFactor;
        emissive = (_e295.xyz * _e298.xyz);
        let _e301 = emissive;
        let _e302 = col;
        let _e304 = (_e302.xyz + _e301);
        col[0u] = _e304.x;
        col[1u] = _e304.y;
        col[2u] = _e304.z;
    }
    let _e311 = col;
    let _e313 = pow(_e311.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e313.x;
    col[1u] = _e313.y;
    col[2u] = _e313.z;
    let _e321 = baseColor[3u];
    col[3u] = _e321;
    let _e323 = col;
    outColor = _e323;
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
