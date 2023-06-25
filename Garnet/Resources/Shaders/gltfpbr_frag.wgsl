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

fn SRGBtoLINEARvf4_(srgbIn: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e64 = (*srgbIn);
    let _e66 = pow(_e64.xyz, vec3<f32>(2.200000047683716, 2.200000047683716, 2.200000047683716));
    let _e68 = (*srgbIn)[3u];
    return vec4<f32>(_e66.x, _e66.y, _e66.z, _e68);
}

fn LINEARtoSRGBvf4_(srgbIn_1: ptr<function, vec4<f32>>) -> vec4<f32> {
    let _e64 = (*srgbIn_1);
    let _e66 = pow(_e64.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    let _e68 = (*srgbIn_1)[3u];
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
        t = normalize(_e71);
        let _e73 = f_WorldBioTangent_1;
        b = normalize(_e73);
        let _e75 = f_WorldNormal_1;
        n = normalize(_e75);
        let _e77 = t;
        let _e78 = b;
        let _e79 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e77.x, _e77.y, _e77.z), vec3<f32>(_e78.x, _e78.y, _e78.z), vec3<f32>(_e79.x, _e79.y, _e79.z));
        let _e93 = f_Texcoord_1;
        let _e94 = textureSample(normalTexture, normalTextureSampler, _e93);
        nomral = _e94.xyz;
        let _e96 = tbn;
        let _e97 = nomral;
        let _e102 = ubo.normalMapScale;
        let _e104 = ubo.normalMapScale;
        nomral = normalize((_e96 * (((_e97 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e102, _e104, 1.0))));
    } else {
        let _e109 = f_WorldNormal_1;
        nomral = _e109;
    }
    let _e110 = nomral;
    return _e110;
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
    let _e104 = ubo.roughnessFactor;
    perceptualRoughness = _e104;
    let _e106 = ubo.metallicFactor;
    metallic = _e106;
    let _e108 = ubo.useMetallicRoughnessTexture;
    if (_e108 != 0) {
        let _e110 = f_Texcoord_1;
        let _e111 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e110);
        metallicRoughnessColor = _e111;
        let _e112 = perceptualRoughness;
        let _e114 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e112 * _e114);
        let _e116 = metallic;
        let _e118 = metallicRoughnessColor[2u];
        metallic = (_e116 * _e118);
    }
    let _e120 = perceptualRoughness;
    perceptualRoughness = clamp(_e120, 0.03999999910593033, 1.0);
    let _e122 = metallic;
    metallic = clamp(_e122, 0.0, 1.0);
    let _e124 = perceptualRoughness;
    let _e125 = perceptualRoughness;
    alphaRoughness = (_e124 * _e125);
    let _e128 = ubo.useBaseColorTexture;
    if (_e128 != 0) {
        let _e130 = f_Texcoord_1;
        let _e131 = textureSample(baseColorTexture, baseColorTextureSampler, _e130);
        baseColor = _e131;
    } else {
        let _e133 = ubo.baseColorFactor;
        baseColor = _e133;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e134 = baseColor;
    let _e136 = f0_;
    diffuseColor = (_e134.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e136));
    let _e139 = metallic;
    let _e141 = diffuseColor;
    diffuseColor = (_e141 * (1.0 - _e139));
    let _e143 = f0_;
    let _e144 = baseColor;
    let _e146 = metallic;
    specularColor = mix(_e143, _e144.xyz, vec3<f32>(_e146));
    let _e150 = specularColor[0u];
    let _e152 = specularColor[1u];
    let _e155 = specularColor[2u];
    reflectance = max(max(_e150, _e152), _e155);
    let _e157 = reflectance;
    reflectance90_ = clamp((_e157 * 25.0), 0.0, 1.0);
    let _e160 = specularColor;
    specularEnvironmentR0_ = _e160;
    let _e161 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e161);
    let _e163 = getNormal();
    n_1 = _e163;
    let _e165 = ubo.cameraPos;
    let _e167 = f_WorldPos_1;
    v = normalize((_e165.xyz - _e167.xyz));
    let _e172 = ubo.lightDir;
    l = normalize(_e172.xyz);
    let _e175 = v;
    let _e176 = l;
    h = normalize((_e175 + _e176));
    let _e179 = v;
    let _e180 = n_1;
    reflection = -(normalize(reflect(_e179, _e180)));
    let _e184 = n_1;
    let _e185 = l;
    NdotL_1 = clamp(dot(_e184, _e185), 0.0010000000474974513, 1.0);
    let _e188 = n_1;
    let _e189 = v;
    NdotV_1 = clamp(abs(dot(_e188, _e189)), 0.0010000000474974513, 1.0);
    let _e193 = n_1;
    let _e194 = h;
    NdotH = clamp(dot(_e193, _e194), 0.0, 1.0);
    let _e197 = l;
    let _e198 = h;
    LdotH = clamp(dot(_e197, _e198), 0.0, 1.0);
    let _e201 = v;
    let _e202 = h;
    VdotH = clamp(dot(_e201, _e202), 0.0, 1.0);
    let _e205 = NdotL_1;
    let _e206 = NdotV_1;
    let _e207 = NdotH;
    let _e208 = LdotH;
    let _e209 = VdotH;
    let _e210 = perceptualRoughness;
    let _e211 = metallic;
    let _e212 = specularEnvironmentR0_;
    let _e213 = specularEnvironmentR90_;
    let _e214 = alphaRoughness;
    let _e215 = diffuseColor;
    let _e216 = specularColor;
    pbrParam = PBRParam(_e205, _e206, _e207, _e208, _e209, _e210, _e211, _e212, _e213, _e214, _e215, _e216);
    let _e218 = pbrParam;
    param_4 = _e218;
    let _e219 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e219;
    let _e220 = pbrParam;
    param_5 = _e220;
    let _e221 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e221;
    let _e222 = pbrParam;
    param_6 = _e222;
    let _e223 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e223;
    let _e224 = D;
    let _e225 = G;
    let _e227 = F;
    let _e229 = NdotL_1;
    let _e231 = NdotV_1;
    specularBRDF = ((_e227 * (_e224 * _e225)) / vec3<f32>(((4.0 * _e229) * _e231)));
    let _e235 = F;
    let _e238 = pbrParam;
    param_7 = _e238;
    let _e239 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e235) * _e239);
    let _e242 = ubo.mipCount;
    mipCount = _e242;
    let _e243 = mipCount;
    let _e244 = perceptualRoughness;
    lod = (_e243 * _e244);
    let _e246 = v;
    let _e247 = n_1;
    let _e249 = lod;
    let _e250 = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(_e246, _e247), _e249);
    param_8 = _e250;
    let _e251 = LINEARtoSRGBvf4_((&param_8));
    reflectColor = _e251.xyz;
    let _e253 = NdotL_1;
    let _e255 = ubo.lightColor;
    let _e258 = specularBRDF;
    let _e259 = diffuseBRDF;
    let _e262 = reflectColor;
    let _e263 = specularColor;
    let _e265 = (((_e255.xyz * _e253) * (_e258 + _e259)) + (_e262 * _e263));
    col[0u] = _e265.x;
    col[1u] = _e265.y;
    col[2u] = _e265.z;
    let _e273 = ubo.useOcclusionTexture;
    if (_e273 != 0) {
        let _e275 = f_Texcoord_1;
        let _e276 = textureSample(occlusionTexture, occlusionTextureSampler, _e275);
        ao = _e276.x;
        let _e278 = col;
        let _e280 = col;
        let _e282 = ao;
        let _e285 = ubo.occlusionStrength;
        let _e287 = mix(_e278.xyz, (_e280.xyz * _e282), vec3<f32>(_e285));
        col[0u] = _e287.x;
        col[1u] = _e287.y;
        col[2u] = _e287.z;
    }
    let _e295 = ubo.useEmissiveTexture;
    if (_e295 != 0) {
        let _e297 = f_Texcoord_1;
        let _e298 = textureSample(emissiveTexture, emissiveTextureSampler, _e297);
        param_9 = _e298;
        let _e299 = SRGBtoLINEARvf4_((&param_9));
        let _e302 = ubo.emissiveFactor;
        emissive = (_e299.xyz * _e302.xyz);
        let _e305 = emissive;
        let _e306 = col;
        let _e308 = (_e306.xyz + _e305);
        col[0u] = _e308.x;
        col[1u] = _e308.y;
        col[2u] = _e308.z;
    }
    let _e315 = col;
    let _e317 = pow(_e315.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e317.x;
    col[1u] = _e317.y;
    col[2u] = _e317.z;
    let _e325 = baseColor[3u];
    col[3u] = _e325;
    let _e327 = col;
    outColor = _e327;
    return;
}

@fragment 
fn main(@location(3) f_WorldTangent: vec3<f32>, @location(4) f_WorldBioTangent: vec3<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(1) f_Texcoord: vec2<f32>, @location(2) f_WorldPos: vec4<f32>) -> @location(0) vec4<f32> {
    f_WorldTangent_1 = f_WorldTangent;
    f_WorldBioTangent_1 = f_WorldBioTangent;
    f_WorldNormal_1 = f_WorldNormal;
    f_Texcoord_1 = f_Texcoord;
    f_WorldPos_1 = f_WorldPos;
    main_1();
    let _e11 = outColor;
    return _e11;
}
