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
var<private> f_WorldNormal_1: vec3<f32>;
var<private> f_WorldTangent_1: vec4<f32>;
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
var<private> outColor: vec4<f32>;
@group(0) @binding(5) 
var emissiveTexture: texture_2d<f32>;
@group(0) @binding(6) 
var emissiveTextureSampler: sampler;
@group(0) @binding(9) 
var occlusionTexture: texture_2d<f32>;
@group(0) @binding(10) 
var occlusionTextureSampler: sampler;

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e56 = (*param).diffuseColor;
    return (_e56 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e56 = (*param_1).reflectance0_;
    let _e58 = (*param_1).reflectance90_;
    let _e60 = (*param_1).reflectance0_;
    let _e63 = (*param_1).VdotH;
    return (_e56 + ((_e58 - _e60) * pow(clamp((1.0 - _e63), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e61 = (*param_2).NdotL;
    NdotL = _e61;
    let _e63 = (*param_2).NdotV;
    NdotV = _e63;
    let _e65 = (*param_2).alphaRoughness;
    r = _e65;
    let _e66 = NdotL;
    let _e68 = NdotL;
    let _e69 = r;
    let _e70 = r;
    let _e72 = r;
    let _e73 = r;
    let _e76 = NdotL;
    let _e77 = NdotL;
    attenuationL = ((2.0 * _e66) / (_e68 + sqrt(((_e69 * _e70) + ((1.0 - (_e72 * _e73)) * (_e76 * _e77))))));
    let _e84 = NdotV;
    let _e86 = NdotV;
    let _e87 = r;
    let _e88 = r;
    let _e90 = r;
    let _e91 = r;
    let _e94 = NdotV;
    let _e95 = NdotV;
    attenuationV = ((2.0 * _e84) / (_e86 + sqrt(((_e87 * _e88) + ((1.0 - (_e90 * _e91)) * (_e94 * _e95))))));
    let _e102 = attenuationL;
    let _e103 = attenuationV;
    return (_e102 * _e103);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e58 = (*param_3).alphaRoughness;
    let _e60 = (*param_3).alphaRoughness;
    roughness2_ = (_e58 * _e60);
    let _e63 = (*param_3).NdotH;
    let _e64 = roughness2_;
    let _e67 = (*param_3).NdotH;
    let _e70 = (*param_3).NdotH;
    f = ((((_e63 * _e64) - _e67) * _e70) + 1.0);
    let _e73 = roughness2_;
    let _e74 = f;
    let _e76 = f;
    return (_e73 / ((3.1415927410125732 * _e74) * _e76));
}

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var n: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e60 = ubo.useNormalTexture;
    if (_e60 != 0) {
        let _e62 = f_WorldNormal_1;
        n = normalize(_e62);
        let _e64 = f_WorldTangent_1;
        t = normalize(_e64.xyz);
        let _e67 = n;
        let _e68 = t;
        b = normalize(cross(_e67, _e68));
        let _e71 = t;
        let _e72 = b;
        let _e73 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e71.x, _e71.y, _e71.z), vec3<f32>(_e72.x, _e72.y, _e72.z), vec3<f32>(_e73.x, _e73.y, _e73.z));
        let _e87 = f_Texcoord_1;
        let _e88 = textureSample(normalTexture, normalTextureSampler, _e87);
        nomral = _e88.xyz;
        let _e90 = tbn;
        let _e91 = nomral;
        let _e96 = ubo.normalMapScale;
        let _e98 = ubo.normalMapScale;
        nomral = normalize((_e90 * (((_e91 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e96, _e98, 1.0))));
    } else {
        let _e103 = f_WorldNormal_1;
        nomral = _e103;
    }
    let _e104 = nomral;
    return _e104;
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

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e88 = ubo.roughnessFactor;
    perceptualRoughness = _e88;
    let _e90 = ubo.metallicFactor;
    metallic = _e90;
    let _e92 = ubo.useMetallicRoughnessTexture;
    if (_e92 != 0) {
        let _e94 = f_Texcoord_1;
        let _e95 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e94);
        metallicRoughnessColor = _e95;
        let _e96 = perceptualRoughness;
        let _e98 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e96 * _e98);
        let _e100 = metallic;
        let _e102 = metallicRoughnessColor[2u];
        metallic = (_e100 * _e102);
    }
    let _e104 = perceptualRoughness;
    perceptualRoughness = clamp(_e104, 0.03999999910593033, 1.0);
    let _e106 = metallic;
    metallic = clamp(_e106, 0.0, 1.0);
    let _e108 = perceptualRoughness;
    let _e109 = perceptualRoughness;
    alphaRoughness = (_e108 * _e109);
    let _e112 = ubo.useBaseColorTexture;
    if (_e112 != 0) {
        let _e114 = f_Texcoord_1;
        let _e115 = textureSample(baseColorTexture, baseColorTextureSampler, _e114);
        baseColor = _e115;
    } else {
        let _e117 = ubo.baseColorFactor;
        baseColor = _e117;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e118 = baseColor;
    let _e120 = f0_;
    diffuseColor = (_e118.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e120));
    let _e123 = metallic;
    let _e125 = diffuseColor;
    diffuseColor = (_e125 * (1.0 - _e123));
    let _e127 = f0_;
    let _e128 = baseColor;
    let _e130 = metallic;
    specularColor = mix(_e127, _e128.xyz, vec3<f32>(_e130));
    let _e134 = specularColor[0u];
    let _e136 = specularColor[1u];
    let _e139 = specularColor[2u];
    reflectance = max(max(_e134, _e136), _e139);
    let _e141 = reflectance;
    reflectance90_ = clamp((_e141 * 25.0), 0.0, 1.0);
    let _e144 = specularColor;
    specularEnvironmentR0_ = _e144;
    let _e145 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e145);
    let _e147 = getNormal();
    n_1 = _e147;
    let _e149 = ubo.cameraPos;
    let _e151 = f_WorldPos_1;
    v = normalize((_e149.xyz - _e151.xyz));
    let _e156 = ubo.lightDir;
    l = normalize(_e156.xyz);
    let _e159 = v;
    let _e160 = l;
    h = normalize((_e159 + _e160));
    let _e163 = v;
    let _e164 = n_1;
    reflection = -(normalize(reflect(_e163, _e164)));
    let _e168 = n_1;
    let _e169 = l;
    NdotL_1 = clamp(dot(_e168, _e169), 0.0010000000474974513, 1.0);
    let _e172 = n_1;
    let _e173 = v;
    NdotV_1 = clamp(abs(dot(_e172, _e173)), 0.0010000000474974513, 1.0);
    let _e177 = n_1;
    let _e178 = h;
    NdotH = clamp(dot(_e177, _e178), 0.0, 1.0);
    let _e181 = l;
    let _e182 = h;
    LdotH = clamp(dot(_e181, _e182), 0.0, 1.0);
    let _e185 = v;
    let _e186 = h;
    VdotH = clamp(dot(_e185, _e186), 0.0, 1.0);
    let _e189 = NdotL_1;
    let _e190 = NdotV_1;
    let _e191 = NdotH;
    let _e192 = LdotH;
    let _e193 = VdotH;
    let _e194 = perceptualRoughness;
    let _e195 = metallic;
    let _e196 = specularEnvironmentR0_;
    let _e197 = specularEnvironmentR90_;
    let _e198 = alphaRoughness;
    let _e199 = diffuseColor;
    let _e200 = specularColor;
    pbrParam = PBRParam(_e189, _e190, _e191, _e192, _e193, _e194, _e195, _e196, _e197, _e198, _e199, _e200);
    let _e202 = pbrParam;
    param_4 = _e202;
    let _e203 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e203;
    let _e204 = pbrParam;
    param_5 = _e204;
    let _e205 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e205;
    let _e206 = pbrParam;
    param_6 = _e206;
    let _e207 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e207;
    let _e208 = D;
    let _e209 = G;
    let _e211 = F;
    let _e213 = NdotL_1;
    let _e215 = NdotV_1;
    specularBRDF = ((_e211 * (_e208 * _e209)) / vec3<f32>(((4.0 * _e213) * _e215)));
    let _e219 = F;
    let _e222 = pbrParam;
    param_7 = _e222;
    let _e223 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e219) * _e223);
    let _e225 = NdotL_1;
    let _e227 = ubo.lightColor;
    let _e230 = specularBRDF;
    let _e231 = diffuseBRDF;
    let _e233 = ((_e227.xyz * _e225) * (_e230 + _e231));
    col[0u] = _e233.x;
    col[1u] = _e233.y;
    col[2u] = _e233.z;
    let _e240 = col;
    let _e242 = pow(_e240.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e242.x;
    col[1u] = _e242.y;
    col[2u] = _e242.z;
    let _e250 = baseColor[3u];
    col[3u] = _e250;
    let _e252 = col;
    outColor = _e252;
    return;
}

@fragment 
fn main(@location(0) f_WorldNormal: vec3<f32>, @location(3) f_WorldTangent: vec4<f32>, @location(1) f_Texcoord: vec2<f32>, @location(2) f_WorldPos: vec4<f32>) -> @location(0) vec4<f32> {
    f_WorldNormal_1 = f_WorldNormal;
    f_WorldTangent_1 = f_WorldTangent;
    f_Texcoord_1 = f_Texcoord;
    f_WorldPos_1 = f_WorldPos;
    main_1();
    let _e9 = outColor;
    return _e9;
}
