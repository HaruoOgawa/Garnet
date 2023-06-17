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
var<private> outColor: vec4<f32>;
var<private> f_WorldBioTangent_1: vec4<f32>;
@group(0) @binding(5) 
var emissiveTexture: texture_2d<f32>;
@group(0) @binding(6) 
var emissiveTextureSampler: sampler;

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e59 = (*param).diffuseColor;
    return (_e59 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e59 = (*param_1).reflectance0_;
    let _e61 = (*param_1).reflectance90_;
    let _e63 = (*param_1).reflectance0_;
    let _e66 = (*param_1).VdotH;
    return (_e59 + ((_e61 - _e63) * pow(clamp((1.0 - _e66), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e64 = (*param_2).NdotL;
    NdotL = _e64;
    let _e66 = (*param_2).NdotV;
    NdotV = _e66;
    let _e68 = (*param_2).alphaRoughness;
    r = _e68;
    let _e69 = NdotL;
    let _e71 = NdotL;
    let _e72 = r;
    let _e73 = r;
    let _e75 = r;
    let _e76 = r;
    let _e79 = NdotL;
    let _e80 = NdotL;
    attenuationL = ((2.0 * _e69) / (_e71 + sqrt(((_e72 * _e73) + ((1.0 - (_e75 * _e76)) * (_e79 * _e80))))));
    let _e87 = NdotV;
    let _e89 = NdotV;
    let _e90 = r;
    let _e91 = r;
    let _e93 = r;
    let _e94 = r;
    let _e97 = NdotV;
    let _e98 = NdotV;
    attenuationV = ((2.0 * _e87) / (_e89 + sqrt(((_e90 * _e91) + ((1.0 - (_e93 * _e94)) * (_e97 * _e98))))));
    let _e105 = attenuationL;
    let _e106 = attenuationV;
    return (_e105 * _e106);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e61 = (*param_3).alphaRoughness;
    let _e63 = (*param_3).alphaRoughness;
    roughness2_ = (_e61 * _e63);
    let _e66 = (*param_3).NdotH;
    let _e67 = roughness2_;
    let _e70 = (*param_3).NdotH;
    let _e73 = (*param_3).NdotH;
    f = ((((_e66 * _e67) - _e70) * _e73) + 1.0);
    let _e76 = roughness2_;
    let _e77 = f;
    let _e79 = f;
    return (_e76 / ((3.1415927410125732 * _e77) * _e79));
}

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e63 = ubo.useNormalTexture;
    if (_e63 != 0) {
        let _e65 = f_WorldTangent_1;
        t = normalize(_e65.xyz);
        let _e68 = f_WorldTangent_1;
        b = normalize(_e68.xyz);
        let _e71 = f_WorldNormal_1;
        n = normalize(_e71);
        let _e73 = t;
        let _e74 = b;
        let _e75 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e73.x, _e73.y, _e73.z), vec3<f32>(_e74.x, _e74.y, _e74.z), vec3<f32>(_e75.x, _e75.y, _e75.z));
        let _e89 = f_Texcoord_1;
        let _e90 = textureSample(normalTexture, normalTextureSampler, _e89);
        nomral = _e90.xyz;
        let _e92 = tbn;
        let _e93 = nomral;
        let _e98 = ubo.normalMapScale;
        let _e100 = ubo.normalMapScale;
        nomral = normalize((_e92 * (((_e93 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e98, _e100, 1.0))));
    } else {
        let _e105 = f_WorldNormal_1;
        nomral = _e105;
    }
    let _e106 = nomral;
    return _e106;
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

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e92 = ubo.roughnessFactor;
    perceptualRoughness = _e92;
    let _e94 = ubo.metallicFactor;
    metallic = _e94;
    let _e96 = ubo.useMetallicRoughnessTexture;
    if (_e96 != 0) {
        let _e98 = f_Texcoord_1;
        let _e99 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e98);
        metallicRoughnessColor = _e99;
        let _e100 = perceptualRoughness;
        let _e102 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e100 * _e102);
        let _e104 = metallic;
        let _e106 = metallicRoughnessColor[2u];
        metallic = (_e104 * _e106);
    }
    let _e108 = perceptualRoughness;
    perceptualRoughness = clamp(_e108, 0.03999999910593033, 1.0);
    let _e110 = metallic;
    metallic = clamp(_e110, 0.0, 1.0);
    let _e112 = perceptualRoughness;
    let _e113 = perceptualRoughness;
    alphaRoughness = (_e112 * _e113);
    let _e116 = ubo.useBaseColorTexture;
    if (_e116 != 0) {
        let _e118 = f_Texcoord_1;
        let _e119 = textureSample(baseColorTexture, baseColorTextureSampler, _e118);
        baseColor = _e119;
    } else {
        let _e121 = ubo.baseColorFactor;
        baseColor = _e121;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e122 = baseColor;
    let _e124 = f0_;
    diffuseColor = (_e122.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e124));
    let _e127 = metallic;
    let _e129 = diffuseColor;
    diffuseColor = (_e129 * (1.0 - _e127));
    let _e131 = f0_;
    let _e132 = baseColor;
    let _e134 = metallic;
    specularColor = mix(_e131, _e132.xyz, vec3<f32>(_e134));
    let _e138 = specularColor[0u];
    let _e140 = specularColor[1u];
    let _e143 = specularColor[2u];
    reflectance = max(max(_e138, _e140), _e143);
    let _e145 = reflectance;
    reflectance90_ = clamp((_e145 * 25.0), 0.0, 1.0);
    let _e148 = specularColor;
    specularEnvironmentR0_ = _e148;
    let _e149 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e149);
    let _e151 = getNormal();
    n_1 = _e151;
    let _e153 = ubo.cameraPos;
    let _e155 = f_WorldPos_1;
    v = normalize((_e153.xyz - _e155.xyz));
    let _e160 = ubo.lightDir;
    l = normalize(_e160.xyz);
    let _e163 = v;
    let _e164 = l;
    h = normalize((_e163 + _e164));
    let _e167 = v;
    let _e168 = n_1;
    reflection = -(normalize(reflect(_e167, _e168)));
    let _e172 = n_1;
    let _e173 = l;
    NdotL_1 = clamp(dot(_e172, _e173), 0.0010000000474974513, 1.0);
    let _e176 = n_1;
    let _e177 = v;
    NdotV_1 = clamp(abs(dot(_e176, _e177)), 0.0010000000474974513, 1.0);
    let _e181 = n_1;
    let _e182 = h;
    NdotH = clamp(dot(_e181, _e182), 0.0, 1.0);
    let _e185 = l;
    let _e186 = h;
    LdotH = clamp(dot(_e185, _e186), 0.0, 1.0);
    let _e189 = v;
    let _e190 = h;
    VdotH = clamp(dot(_e189, _e190), 0.0, 1.0);
    let _e193 = NdotL_1;
    let _e194 = NdotV_1;
    let _e195 = NdotH;
    let _e196 = LdotH;
    let _e197 = VdotH;
    let _e198 = perceptualRoughness;
    let _e199 = metallic;
    let _e200 = specularEnvironmentR0_;
    let _e201 = specularEnvironmentR90_;
    let _e202 = alphaRoughness;
    let _e203 = diffuseColor;
    let _e204 = specularColor;
    pbrParam = PBRParam(_e193, _e194, _e195, _e196, _e197, _e198, _e199, _e200, _e201, _e202, _e203, _e204);
    let _e206 = pbrParam;
    param_4 = _e206;
    let _e207 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e207;
    let _e208 = pbrParam;
    param_5 = _e208;
    let _e209 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e209;
    let _e210 = pbrParam;
    param_6 = _e210;
    let _e211 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e211;
    let _e212 = D;
    let _e213 = G;
    let _e215 = F;
    let _e217 = NdotL_1;
    let _e219 = NdotV_1;
    specularBRDF = ((_e215 * (_e212 * _e213)) / vec3<f32>(((4.0 * _e217) * _e219)));
    let _e223 = F;
    let _e226 = pbrParam;
    param_7 = _e226;
    let _e227 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e223) * _e227);
    let _e229 = NdotL_1;
    let _e231 = ubo.lightColor;
    let _e234 = specularBRDF;
    let _e235 = diffuseBRDF;
    let _e237 = ((_e231.xyz * _e229) * (_e234 + _e235));
    col[0u] = _e237.x;
    col[1u] = _e237.y;
    col[2u] = _e237.z;
    let _e245 = ubo.useOcclusionTexture;
    if (_e245 != 0) {
        let _e247 = f_Texcoord_1;
        let _e248 = textureSample(occlusionTexture, occlusionTextureSampler, _e247);
        ao = _e248.x;
        let _e250 = col;
        let _e252 = col;
        let _e254 = ao;
        let _e257 = ubo.occlusionStrength;
        let _e259 = mix(_e250.xyz, (_e252.xyz * _e254), vec3<f32>(_e257));
        col[0u] = _e259.x;
        col[1u] = _e259.y;
        col[2u] = _e259.z;
    }
    let _e266 = col;
    let _e268 = pow(_e266.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e268.x;
    col[1u] = _e268.y;
    col[2u] = _e268.z;
    let _e276 = baseColor[3u];
    col[3u] = _e276;
    let _e278 = col;
    outColor = _e278;
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
