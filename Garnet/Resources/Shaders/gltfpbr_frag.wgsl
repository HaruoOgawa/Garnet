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
    useBaseColorTexture: i32,
    useMetallicRoughnessTexture: i32,
    useEmissiveTexture: i32,
    useNormalTexture: i32,
    useOcclusionTexture: i32,
}

@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
@group(0) @binding(3) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(4) 
var metallicRoughnessTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
@group(0) @binding(1) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(2) 
var baseColorTextureSampler: sampler;
var<private> f_WorldNormal_1: vec3<f32>;
var<private> f_WorldPos_1: vec4<f32>;
var<private> outColor: vec4<f32>;
@group(0) @binding(5) 
var emissiveTexture: texture_2d<f32>;
@group(0) @binding(6) 
var emissiveTextureSampler: sampler;
@group(0) @binding(7) 
var normalTexture: texture_2d<f32>;
@group(0) @binding(8) 
var normalTextureSampler: sampler;
@group(0) @binding(9) 
var occlusionTexture: texture_2d<f32>;
@group(0) @binding(10) 
var occlusionTextureSampler: sampler;

fn CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param: ptr<function, PBRParam>) -> vec3<f32> {
    let _e52 = (*param).diffuseColor;
    return (_e52 / vec3<f32>(3.1415927410125732));
}

fn CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_1: ptr<function, PBRParam>) -> vec3<f32> {
    let _e52 = (*param_1).reflectance0_;
    let _e54 = (*param_1).reflectance90_;
    let _e56 = (*param_1).reflectance0_;
    let _e59 = (*param_1).VdotH;
    return (_e52 + ((_e54 - _e56) * pow(clamp((1.0 - _e59), 0.0, 1.0), 5.0)));
}

fn CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_2: ptr<function, PBRParam>) -> f32 {
    var NdotL: f32;
    var NdotV: f32;
    var r: f32;
    var attenuationL: f32;
    var attenuationV: f32;

    let _e57 = (*param_2).NdotL;
    NdotL = _e57;
    let _e59 = (*param_2).NdotV;
    NdotV = _e59;
    let _e61 = (*param_2).alphaRoughness;
    r = _e61;
    let _e62 = NdotL;
    let _e64 = NdotL;
    let _e65 = r;
    let _e66 = r;
    let _e68 = r;
    let _e69 = r;
    let _e72 = NdotL;
    let _e73 = NdotL;
    attenuationL = ((2.0 * _e62) / (_e64 + sqrt(((_e65 * _e66) + ((1.0 - (_e68 * _e69)) * (_e72 * _e73))))));
    let _e80 = NdotV;
    let _e82 = NdotV;
    let _e83 = r;
    let _e84 = r;
    let _e86 = r;
    let _e87 = r;
    let _e90 = NdotV;
    let _e91 = NdotV;
    attenuationV = ((2.0 * _e80) / (_e82 + sqrt(((_e83 * _e84) + ((1.0 - (_e86 * _e87)) * (_e90 * _e91))))));
    let _e98 = attenuationL;
    let _e99 = attenuationV;
    return (_e98 * _e99);
}

fn CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_(param_3: ptr<function, PBRParam>) -> f32 {
    var roughness2_: f32;
    var f: f32;

    let _e54 = (*param_3).alphaRoughness;
    let _e56 = (*param_3).alphaRoughness;
    roughness2_ = (_e54 * _e56);
    let _e59 = (*param_3).NdotH;
    let _e60 = roughness2_;
    let _e63 = (*param_3).NdotH;
    let _e66 = (*param_3).NdotH;
    f = ((((_e59 * _e60) - _e63) * _e66) + 1.0);
    let _e69 = roughness2_;
    let _e70 = f;
    let _e72 = f;
    return (_e69 / ((3.1415927410125732 * _e70) * _e72));
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
    var n: vec3<f32>;
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
    let _e84 = ubo.roughnessFactor;
    perceptualRoughness = _e84;
    let _e86 = ubo.metallicFactor;
    metallic = _e86;
    let _e88 = ubo.useMetallicRoughnessTexture;
    if (_e88 != 0) {
        let _e90 = f_Texcoord_1;
        let _e91 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e90);
        metallicRoughnessColor = _e91;
        let _e92 = perceptualRoughness;
        let _e94 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e92 * _e94);
        let _e96 = metallic;
        let _e98 = metallicRoughnessColor[2u];
        metallic = (_e96 * _e98);
    }
    let _e100 = perceptualRoughness;
    perceptualRoughness = clamp(_e100, 0.03999999910593033, 1.0);
    let _e102 = metallic;
    metallic = clamp(_e102, 0.0, 1.0);
    let _e104 = perceptualRoughness;
    let _e105 = perceptualRoughness;
    alphaRoughness = (_e104 * _e105);
    let _e108 = ubo.useBaseColorTexture;
    if (_e108 != 0) {
        let _e110 = f_Texcoord_1;
        let _e111 = textureSample(baseColorTexture, baseColorTextureSampler, _e110);
        baseColor = _e111;
    } else {
        let _e113 = ubo.baseColorFactor;
        baseColor = _e113;
    }
    f0_ = vec3<f32>(0.03999999910593033, 0.03999999910593033, 0.03999999910593033);
    let _e114 = baseColor;
    let _e116 = f0_;
    diffuseColor = (_e114.xyz * (vec3<f32>(1.0, 1.0, 1.0) - _e116));
    let _e119 = metallic;
    let _e121 = diffuseColor;
    diffuseColor = (_e121 * (1.0 - _e119));
    let _e123 = f0_;
    let _e124 = baseColor;
    let _e126 = metallic;
    specularColor = mix(_e123, _e124.xyz, vec3<f32>(_e126));
    let _e130 = specularColor[0u];
    let _e132 = specularColor[1u];
    let _e135 = specularColor[2u];
    reflectance = max(max(_e130, _e132), _e135);
    let _e137 = reflectance;
    reflectance90_ = clamp((_e137 * 25.0), 0.0, 1.0);
    let _e140 = specularColor;
    specularEnvironmentR0_ = _e140;
    let _e141 = reflectance90_;
    specularEnvironmentR90_ = (vec3<f32>(1.0, 1.0, 1.0) * _e141);
    let _e143 = f_WorldNormal_1;
    n = _e143;
    let _e145 = ubo.cameraPos;
    let _e147 = f_WorldPos_1;
    v = normalize((_e145.xyz - _e147.xyz));
    let _e152 = ubo.lightDir;
    l = normalize(_e152.xyz);
    let _e155 = v;
    let _e156 = l;
    h = normalize((_e155 + _e156));
    let _e159 = v;
    let _e160 = n;
    reflection = -(normalize(reflect(_e159, _e160)));
    let _e164 = n;
    let _e165 = l;
    NdotL_1 = clamp(dot(_e164, _e165), 0.0010000000474974513, 1.0);
    let _e168 = n;
    let _e169 = v;
    NdotV_1 = clamp(abs(dot(_e168, _e169)), 0.0010000000474974513, 1.0);
    let _e173 = n;
    let _e174 = h;
    NdotH = clamp(dot(_e173, _e174), 0.0, 1.0);
    let _e177 = l;
    let _e178 = h;
    LdotH = clamp(dot(_e177, _e178), 0.0, 1.0);
    let _e181 = v;
    let _e182 = h;
    VdotH = clamp(dot(_e181, _e182), 0.0, 1.0);
    let _e185 = NdotL_1;
    let _e186 = NdotV_1;
    let _e187 = NdotH;
    let _e188 = LdotH;
    let _e189 = VdotH;
    let _e190 = perceptualRoughness;
    let _e191 = metallic;
    let _e192 = specularEnvironmentR0_;
    let _e193 = specularEnvironmentR90_;
    let _e194 = alphaRoughness;
    let _e195 = diffuseColor;
    let _e196 = specularColor;
    pbrParam = PBRParam(_e185, _e186, _e187, _e188, _e189, _e190, _e191, _e192, _e193, _e194, _e195, _e196);
    let _e198 = pbrParam;
    param_4 = _e198;
    let _e199 = CalcMicrofacetstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_4));
    D = _e199;
    let _e200 = pbrParam;
    param_5 = _e200;
    let _e201 = CalcGeometricOcculusionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_5));
    G = _e201;
    let _e202 = pbrParam;
    param_6 = _e202;
    let _e203 = CalcFrenelReflectionstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_6));
    F = _e203;
    let _e204 = D;
    let _e205 = G;
    let _e207 = F;
    let _e209 = NdotL_1;
    let _e211 = NdotV_1;
    specularBRDF = ((_e207 * (_e204 * _e205)) / vec3<f32>(((4.0 * _e209) * _e211)));
    let _e215 = F;
    let _e218 = pbrParam;
    param_7 = _e218;
    let _e219 = CalcDiffuseBRDFstructPBRParamf1f1f1f1f1f1f1vf3vf3f1vf3vf31_((&param_7));
    diffuseBRDF = ((vec3<f32>(1.0) - _e215) * _e219);
    let _e221 = NdotL_1;
    let _e223 = ubo.lightColor;
    let _e226 = specularBRDF;
    let _e227 = diffuseBRDF;
    let _e229 = ((_e223.xyz * _e221) * (_e226 + _e227));
    col[0u] = _e229.x;
    col[1u] = _e229.y;
    col[2u] = _e229.z;
    let _e236 = col;
    let _e238 = pow(_e236.xyz, vec3<f32>(0.4545454680919647, 0.4545454680919647, 0.4545454680919647));
    col[0u] = _e238.x;
    col[1u] = _e238.y;
    col[2u] = _e238.z;
    let _e246 = baseColor[3u];
    col[3u] = _e246;
    let _e248 = col;
    outColor = _e248;
    return;
}

@fragment 
fn main(@location(1) f_Texcoord: vec2<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(2) f_WorldPos: vec4<f32>) -> @location(0) vec4<f32> {
    f_Texcoord_1 = f_Texcoord;
    f_WorldNormal_1 = f_WorldNormal;
    f_WorldPos_1 = f_WorldPos;
    main_1();
    let _e7 = outColor;
    return _e7;
}
