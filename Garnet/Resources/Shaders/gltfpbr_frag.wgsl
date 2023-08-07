struct PBRParam {
  NdotL : f32,
  NdotV : f32,
  NdotH : f32,
  LdotH : f32,
  VdotH : f32,
  perceptualRoughness : f32,
  metallic : f32,
  reflectance0 : vec3<f32>,
  reflectance90 : vec3<f32>,
  alphaRoughness : f32,
  diffuseColor : vec3<f32>,
  specularColor : vec3<f32>,
}

struct UniformBufferObject {
  /* @offset(0) */
  model : mat4x4<f32>,
  /* @offset(64) */
  view : mat4x4<f32>,
  /* @offset(128) */
  proj : mat4x4<f32>,
  /* @offset(192) */
  lightVPMat : mat4x4<f32>,
  /* @offset(256) */
  lightDir : vec4<f32>,
  /* @offset(272) */
  lightColor : vec4<f32>,
  /* @offset(288) */
  cameraPos : vec4<f32>,
  /* @offset(304) */
  baseColorFactor : vec4<f32>,
  /* @offset(320) */
  emissiveFactor : vec4<f32>,
  /* @offset(336) */
  time : f32,
  /* @offset(340) */
  metallicFactor : f32,
  /* @offset(344) */
  roughnessFactor : f32,
  /* @offset(348) */
  normalMapScale : f32,
  /* @offset(352) */
  occlusionStrength : f32,
  /* @offset(356) */
  mipCount : f32,
  /* @offset(360) */
  ShadowMapX : f32,
  /* @offset(364) */
  ShadowMapY : f32,
  /* @offset(368) */
  useBaseColorTexture : i32,
  /* @offset(372) */
  useMetallicRoughnessTexture : i32,
  /* @offset(376) */
  useEmissiveTexture : i32,
  /* @offset(380) */
  useNormalTexture : i32,
  /* @offset(384) */
  useOcclusionTexture : i32,
  /* @offset(388) */
  t_pad_0 : i32,
  /* @offset(392) */
  t_pad_1 : i32,
  /* @offset(396) */
  t_pad_2 : i32,
}

@group(0) @binding(0) var<uniform> ubo : UniformBufferObject;

var<private> f_WorldTangent : vec3<f32>;

var<private> f_WorldBioTangent : vec3<f32>;

var<private> f_WorldNormal : vec3<f32>;

@group(0) @binding(7) var normalTexture : texture_2d<f32>;

@group(0) @binding(8) var normalTextureSampler : sampler;

var<private> f_Texcoord : vec2<f32>;

@group(0) @binding(13) var shadowmapTexture : texture_2d<f32>;

@group(0) @binding(14) var shadowmapTextureSampler : sampler;

@group(0) @binding(3) var metallicRoughnessTexture : texture_2d<f32>;

@group(0) @binding(4) var metallicRoughnessTextureSampler : sampler;

@group(0) @binding(1) var baseColorTexture : texture_2d<f32>;

@group(0) @binding(2) var baseColorTextureSampler : sampler;

var<private> f_WorldPos : vec4<f32>;

@group(0) @binding(11) var cubemapTexture : texture_cube<f32>;

@group(0) @binding(12) var cubemapTextureSampler : sampler;

@group(0) @binding(9) var occlusionTexture : texture_2d<f32>;

@group(0) @binding(10) var occlusionTextureSampler : sampler;

@group(0) @binding(5) var emissiveTexture : texture_2d<f32>;

@group(0) @binding(6) var emissiveTextureSampler : sampler;

var<private> f_LightSpacePos : vec4<f32>;

var<private> outColor : vec4<f32>;

fn getNormal_() -> vec3<f32> {
  var nomral_1 : vec3<f32>;
  var t : vec3<f32>;
  var b : vec3<f32>;
  var n : vec3<f32>;
  var tbn : mat3x3<f32>;
  nomral_1 = vec3<f32>(0.0f, 0.0f, 0.0f);
  let x_173 : i32 = ubo.useNormalTexture;
  if ((x_173 != 0i)) {
    let x_181 : vec3<f32> = f_WorldTangent;
    t = normalize(x_181);
    let x_185 : vec3<f32> = f_WorldBioTangent;
    b = normalize(x_185);
    let x_189 : vec3<f32> = f_WorldNormal;
    n = normalize(x_189);
    let x_194 : vec3<f32> = t;
    let x_195 : vec3<f32> = b;
    let x_196 : vec3<f32> = n;
    tbn = mat3x3<f32>(vec3<f32>(x_194.x, x_194.y, x_194.z), vec3<f32>(x_195.x, x_195.y, x_195.z), vec3<f32>(x_196.x, x_196.y, x_196.z));
    let x_222 : vec2<f32> = f_Texcoord;
    let x_223 : vec4<f32> = textureSample(normalTexture, normalTextureSampler, x_222);
    nomral_1 = vec3<f32>(x_223.x, x_223.y, x_223.z);
    let x_225 : mat3x3<f32> = tbn;
    let x_226 : vec3<f32> = nomral_1;
    let x_233 : f32 = ubo.normalMapScale;
    let x_235 : f32 = ubo.normalMapScale;
    nomral_1 = normalize((x_225 * (((x_226 * 2.0f) - vec3<f32>(1.0f, 1.0f, 1.0f)) * vec3<f32>(x_233, x_235, 1.0f))));
  } else {
    let x_241 : vec3<f32> = f_WorldNormal;
    nomral_1 = x_241;
  }
  let x_242 : vec3<f32> = nomral_1;
  return x_242;
}

fn CalcMicrofacet_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(param : ptr<function, PBRParam>) -> f32 {
  var roughness2 : f32;
  var f : f32;
  let x_54 : f32 = (*(param)).alphaRoughness;
  let x_56 : f32 = (*(param)).alphaRoughness;
  roughness2 = (x_54 * x_56);
  let x_61 : f32 = (*(param)).NdotH;
  let x_62 : f32 = roughness2;
  let x_65 : f32 = (*(param)).NdotH;
  let x_68 : f32 = (*(param)).NdotH;
  f = ((((x_61 * x_62) - x_65) * x_68) + 1.0f);
  let x_72 : f32 = roughness2;
  let x_74 : f32 = f;
  let x_76 : f32 = f;
  return (x_72 / ((3.14159274101257324219f * x_74) * x_76));
}

fn CalcGeometricOcculusion_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(param_1 : ptr<function, PBRParam>) -> f32 {
  var NdotL : f32;
  var NdotV : f32;
  var r : f32;
  var attenuationL : f32;
  var attenuationV : f32;
  let x_84 : f32 = (*(param_1)).NdotL;
  NdotL = x_84;
  let x_88 : f32 = (*(param_1)).NdotV;
  NdotV = x_88;
  let x_91 : f32 = (*(param_1)).alphaRoughness;
  r = x_91;
  let x_94 : f32 = NdotL;
  let x_96 : f32 = NdotL;
  let x_97 : f32 = r;
  let x_98 : f32 = r;
  let x_100 : f32 = r;
  let x_101 : f32 = r;
  let x_104 : f32 = NdotL;
  let x_105 : f32 = NdotL;
  attenuationL = ((2.0f * x_94) / (x_96 + sqrt(((x_97 * x_98) + ((1.0f - (x_100 * x_101)) * (x_104 * x_105))))));
  let x_113 : f32 = NdotV;
  let x_115 : f32 = NdotV;
  let x_116 : f32 = r;
  let x_117 : f32 = r;
  let x_119 : f32 = r;
  let x_120 : f32 = r;
  let x_123 : f32 = NdotV;
  let x_124 : f32 = NdotV;
  attenuationV = ((2.0f * x_113) / (x_115 + sqrt(((x_116 * x_117) + ((1.0f - (x_119 * x_120)) * (x_123 * x_124))))));
  let x_131 : f32 = attenuationL;
  let x_132 : f32 = attenuationV;
  return (x_131 * x_132);
}

fn CalcFrenelReflection_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(param_2 : ptr<function, PBRParam>) -> vec3<f32> {
  let x_138 : vec3<f32> = (*(param_2)).reflectance0;
  let x_141 : vec3<f32> = (*(param_2)).reflectance90;
  let x_143 : vec3<f32> = (*(param_2)).reflectance0;
  let x_147 : f32 = (*(param_2)).VdotH;
  return (x_138 + ((x_141 - x_143) * pow(clamp((1.0f - x_147), 0.0f, 1.0f), 5.0f)));
}

fn CalcDiffuseBRDF_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(param_3 : ptr<function, PBRParam>) -> vec3<f32> {
  let x_159 : vec3<f32> = (*(param_3)).diffuseColor;
  return (x_159 / vec3<f32>(3.14159274101257324219f, 3.14159274101257324219f, 3.14159274101257324219f));
}

const x_263 = vec3<f32>(0.45454546809196472168f, 0.45454546809196472168f, 0.45454546809196472168f);

fn LINEARtoSRGB_vf4_(srgbIn_1 : ptr<function, vec4<f32>>) -> vec4<f32> {
  let x_260 : vec4<f32> = *(srgbIn_1);
  let x_264 : vec3<f32> = pow(vec3<f32>(x_260.x, x_260.y, x_260.z), x_263);
  let x_266 : f32 = (*(srgbIn_1)).w;
  return vec4<f32>(x_264.x, x_264.y, x_264.z, x_266);
}

fn SRGBtoLINEAR_vf4_(srgbIn : ptr<function, vec4<f32>>) -> vec4<f32> {
  let x_245 : vec4<f32> = *(srgbIn);
  let x_249 : vec3<f32> = pow(vec3<f32>(x_245.x, x_245.y, x_245.z), vec3<f32>(2.20000004768371582031f, 2.20000004768371582031f, 2.20000004768371582031f));
  let x_253 : f32 = (*(srgbIn)).w;
  return vec4<f32>(x_249.x, x_249.y, x_249.z, x_253);
}

fn ComputePCF_vf2_(uv : ptr<function, vec2<f32>>) -> vec2<f32> {
  var moments : vec2<f32>;
  var texelSize : vec2<f32>;
  var x : f32;
  var y : f32;
  moments = vec2<f32>(0.0f, 0.0f);
  let x_278 : f32 = ubo.ShadowMapX;
  let x_282 : f32 = ubo.ShadowMapY;
  texelSize = vec2<f32>((1.0f / x_278), (1.0f / x_282));
  x = -1.0f;
  loop {
    let x_292 : f32 = x;
    if ((x_292 <= 1.0f)) {
    } else {
      break;
    }
    y = -1.0f;
    loop {
      let x_300 : f32 = y;
      if ((x_300 <= 1.0f)) {
      } else {
        break;
      }
      let x_307 : vec2<f32> = *(uv);
      let x_308 : f32 = x;
      let x_309 : f32 = y;
      let x_311 : vec2<f32> = texelSize;
      let x_314 : vec4<f32> = textureSample(shadowmapTexture, shadowmapTextureSampler, (x_307 + (vec2<f32>(x_308, x_309) * x_311)));
      let x_316 : vec2<f32> = moments;
      moments = (x_316 + vec2<f32>(x_314.x, x_314.y));

      continuing {
        let x_318 : f32 = y;
        y = (x_318 + 1.0f);
      }
    }

    continuing {
      let x_320 : f32 = x;
      x = (x_320 + 1.0f);
    }
  }
  let x_323 : vec2<f32> = moments;
  moments = (x_323 / vec2<f32>(9.0f, 9.0f));
  let x_326 : vec2<f32> = moments;
  return x_326;
}

fn CalcShadow_vf3_vf3_vf3_(lsp : ptr<function, vec3<f32>>, nomral : ptr<function, vec3<f32>>, lightDir : ptr<function, vec3<f32>>) -> f32 {
  var moments_1 : vec2<f32>;
  var param_4 : vec2<f32>;
  var ShadowBias : f32;
  var distance : f32;
  var variance : f32;
  var d : f32;
  var p_max : f32;
  let x_331 : vec3<f32> = *(lsp);
  param_4 = vec2<f32>(x_331.x, x_331.y);
  let x_333 : vec2<f32> = ComputePCF_vf2_(&(param_4));
  moments_1 = x_333;
  let x_337 : vec3<f32> = *(nomral);
  let x_338 : vec3<f32> = *(lightDir);
  ShadowBias = max(0.00499999988824129105f, (0.05000000074505805969f * (1.0f - dot(x_337, x_338))));
  let x_346 : f32 = (*(lsp)).z;
  let x_347 : f32 = ShadowBias;
  distance = (x_346 - x_347);
  let x_349 : f32 = distance;
  let x_352 : f32 = moments_1.x;
  if ((x_349 <= x_352)) {
    return 1.0f;
  }
  let x_360 : f32 = moments_1.y;
  let x_362 : f32 = moments_1.x;
  let x_364 : f32 = moments_1.x;
  variance = (x_360 - (x_362 * x_364));
  let x_367 : f32 = variance;
  variance = max(0.00499999988824129105f, x_367);
  let x_370 : f32 = distance;
  let x_372 : f32 = moments_1.x;
  d = (x_370 - x_372);
  let x_375 : f32 = variance;
  let x_376 : f32 = variance;
  let x_377 : f32 = d;
  let x_378 : f32 = d;
  p_max = (x_375 / (x_376 + (x_377 * x_378)));
  let x_382 : f32 = p_max;
  return x_382;
}

const x_448 = vec3<f32>(1.0f, 1.0f, 1.0f);

fn main_1() {
  var col : vec4<f32>;
  var perceptualRoughness : f32;
  var metallic : f32;
  var metallicRoughnessColor : vec4<f32>;
  var alphaRoughness : f32;
  var baseColor : vec4<f32>;
  var f0 : vec3<f32>;
  var diffuseColor : vec3<f32>;
  var specularColor : vec3<f32>;
  var reflectance : f32;
  var reflectance90 : f32;
  var specularEnvironmentR0 : vec3<f32>;
  var specularEnvironmentR90 : vec3<f32>;
  var n_1 : vec3<f32>;
  var v : vec3<f32>;
  var l : vec3<f32>;
  var h : vec3<f32>;
  var reflection : vec3<f32>;
  var NdotL_1 : f32;
  var NdotV_1 : f32;
  var NdotH : f32;
  var LdotH : f32;
  var VdotH : f32;
  var pbrParam : PBRParam;
  var D : f32;
  var param_5 : PBRParam;
  var G : f32;
  var param_6 : PBRParam;
  var F : vec3<f32>;
  var param_7 : PBRParam;
  var specularBRDF : vec3<f32>;
  var diffuseBRDF : vec3<f32>;
  var param_8 : PBRParam;
  var mipCount : f32;
  var lod : f32;
  var reflectColor : vec3<f32>;
  var param_9 : vec4<f32>;
  var ao : f32;
  var emissive : vec3<f32>;
  var param_10 : vec4<f32>;
  var lsp_1 : vec3<f32>;
  var shadowCol : f32;
  var outSide : bool;
  var param_11 : vec3<f32>;
  var param_12 : vec3<f32>;
  var param_13 : vec3<f32>;
  var x_713 : bool;
  var x_714 : bool;
  var x_727 : bool;
  var x_728 : bool;
  col = vec4<f32>(1.0f, 1.0f, 1.0f, 1.0f);
  let x_390 : f32 = ubo.roughnessFactor;
  perceptualRoughness = x_390;
  let x_393 : f32 = ubo.metallicFactor;
  metallic = x_393;
  let x_396 : i32 = ubo.useMetallicRoughnessTexture;
  if ((x_396 != 0i)) {
    let x_406 : vec2<f32> = f_Texcoord;
    let x_407 : vec4<f32> = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, x_406);
    metallicRoughnessColor = x_407;
    let x_408 : f32 = perceptualRoughness;
    let x_410 : f32 = metallicRoughnessColor.y;
    perceptualRoughness = (x_408 * x_410);
    let x_412 : f32 = metallic;
    let x_414 : f32 = metallicRoughnessColor.z;
    metallic = (x_412 * x_414);
  }
  let x_416 : f32 = perceptualRoughness;
  perceptualRoughness = clamp(x_416, 0.03999999910593032837f, 1.0f);
  let x_419 : f32 = metallic;
  metallic = clamp(x_419, 0.0f, 1.0f);
  let x_422 : f32 = perceptualRoughness;
  let x_423 : f32 = perceptualRoughness;
  alphaRoughness = (x_422 * x_423);
  let x_427 : i32 = ubo.useBaseColorTexture;
  if ((x_427 != 0i)) {
    let x_437 : vec2<f32> = f_Texcoord;
    let x_438 : vec4<f32> = textureSample(baseColorTexture, baseColorTextureSampler, x_437);
    baseColor = x_438;
  } else {
    let x_442 : vec4<f32> = ubo.baseColorFactor;
    baseColor = x_442;
  }
  f0 = vec3<f32>(0.03999999910593032837f, 0.03999999910593032837f, 0.03999999910593032837f);
  let x_446 : vec4<f32> = baseColor;
  let x_449 : vec3<f32> = f0;
  diffuseColor = (vec3<f32>(x_446.x, x_446.y, x_446.z) * (x_448 - x_449));
  let x_452 : f32 = metallic;
  let x_454 : vec3<f32> = diffuseColor;
  diffuseColor = (x_454 * (1.0f - x_452));
  let x_457 : vec3<f32> = f0;
  let x_458 : vec4<f32> = baseColor;
  let x_460 : f32 = metallic;
  specularColor = mix(x_457, vec3<f32>(x_458.x, x_458.y, x_458.z), vec3<f32>(x_460, x_460, x_460));
  let x_465 : f32 = specularColor.x;
  let x_467 : f32 = specularColor.y;
  let x_470 : f32 = specularColor.z;
  reflectance = max(max(x_465, x_467), x_470);
  let x_473 : f32 = reflectance;
  reflectance90 = clamp((x_473 * 25.0f), 0.0f, 1.0f);
  let x_478 : vec3<f32> = specularColor;
  specularEnvironmentR0 = x_478;
  let x_480 : f32 = reflectance90;
  specularEnvironmentR90 = (x_448 * x_480);
  let x_483 : vec3<f32> = getNormal_();
  n_1 = x_483;
  let x_487 : vec4<f32> = ubo.cameraPos;
  let x_491 : vec4<f32> = f_WorldPos;
  v = normalize((vec3<f32>(x_487.x, x_487.y, x_487.z) - vec3<f32>(x_491.x, x_491.y, x_491.z)));
  let x_497 : vec4<f32> = ubo.lightDir;
  l = normalize(vec3<f32>(x_497.x, x_497.y, x_497.z));
  let x_501 : vec3<f32> = v;
  let x_502 : vec3<f32> = l;
  h = normalize((x_501 + x_502));
  let x_506 : vec3<f32> = v;
  let x_507 : vec3<f32> = n_1;
  reflection = -(normalize(reflect(x_506, x_507)));
  let x_512 : vec3<f32> = n_1;
  let x_513 : vec3<f32> = l;
  NdotL_1 = clamp(dot(x_512, x_513), 0.00100000004749745131f, 1.0f);
  let x_518 : vec3<f32> = n_1;
  let x_519 : vec3<f32> = v;
  NdotV_1 = clamp(abs(dot(x_518, x_519)), 0.00100000004749745131f, 1.0f);
  let x_524 : vec3<f32> = n_1;
  let x_525 : vec3<f32> = h;
  NdotH = clamp(dot(x_524, x_525), 0.0f, 1.0f);
  let x_529 : vec3<f32> = l;
  let x_530 : vec3<f32> = h;
  LdotH = clamp(dot(x_529, x_530), 0.0f, 1.0f);
  let x_534 : vec3<f32> = v;
  let x_535 : vec3<f32> = h;
  VdotH = clamp(dot(x_534, x_535), 0.0f, 1.0f);
  let x_539 : f32 = NdotL_1;
  let x_540 : f32 = NdotV_1;
  let x_541 : f32 = NdotH;
  let x_542 : f32 = LdotH;
  let x_543 : f32 = VdotH;
  let x_544 : f32 = perceptualRoughness;
  let x_545 : f32 = metallic;
  let x_546 : vec3<f32> = specularEnvironmentR0;
  let x_547 : vec3<f32> = specularEnvironmentR90;
  let x_548 : f32 = alphaRoughness;
  let x_549 : vec3<f32> = diffuseColor;
  let x_550 : vec3<f32> = specularColor;
  pbrParam = PBRParam(x_539, x_540, x_541, x_542, x_543, x_544, x_545, x_546, x_547, x_548, x_549, x_550);
  let x_554 : PBRParam = pbrParam;
  param_5 = x_554;
  let x_555 : f32 = CalcMicrofacet_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(&(param_5));
  D = x_555;
  let x_558 : PBRParam = pbrParam;
  param_6 = x_558;
  let x_559 : f32 = CalcGeometricOcculusion_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(&(param_6));
  G = x_559;
  let x_562 : PBRParam = pbrParam;
  param_7 = x_562;
  let x_563 : vec3<f32> = CalcFrenelReflection_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(&(param_7));
  F = x_563;
  let x_565 : f32 = D;
  let x_566 : f32 = G;
  let x_568 : vec3<f32> = F;
  let x_571 : f32 = NdotL_1;
  let x_573 : f32 = NdotV_1;
  let x_574 : f32 = ((4.0f * x_571) * x_573);
  specularBRDF = ((x_568 * (x_565 * x_566)) / vec3<f32>(x_574, x_574, x_574));
  let x_578 : vec3<f32> = F;
  let x_582 : PBRParam = pbrParam;
  param_8 = x_582;
  let x_583 : vec3<f32> = CalcDiffuseBRDF_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(&(param_8));
  diffuseBRDF = ((vec3<f32>(1.0f, 1.0f, 1.0f) - x_578) * x_583);
  let x_588 : f32 = ubo.mipCount;
  mipCount = x_588;
  let x_590 : f32 = mipCount;
  let x_591 : f32 = perceptualRoughness;
  lod = (x_590 * x_591);
  let x_602 : vec3<f32> = v;
  let x_603 : vec3<f32> = n_1;
  let x_605 : f32 = lod;
  let x_606 : vec4<f32> = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(x_602, x_603), x_605);
  param_9 = x_606;
  let x_608 : vec4<f32> = LINEARtoSRGB_vf4_(&(param_9));
  reflectColor = vec3<f32>(x_608.x, x_608.y, x_608.z);
  let x_610 : f32 = NdotL_1;
  let x_613 : vec4<f32> = ubo.lightColor;
  let x_616 : vec3<f32> = specularBRDF;
  let x_617 : vec3<f32> = diffuseBRDF;
  let x_620 : vec3<f32> = reflectColor;
  let x_621 : vec3<f32> = specularColor;
  let x_623 : vec3<f32> = (((vec3<f32>(x_613.x, x_613.y, x_613.z) * x_610) * (x_616 + x_617)) + (x_620 * x_621));
  let x_624 : vec4<f32> = col;
  col = vec4<f32>(x_623.x, x_623.y, x_623.z, x_624.w);
  let x_628 : i32 = ubo.useOcclusionTexture;
  if ((x_628 != 0i)) {
    let x_638 : vec2<f32> = f_Texcoord;
    let x_639 : vec4<f32> = textureSample(occlusionTexture, occlusionTextureSampler, x_638);
    ao = x_639.x;
    let x_641 : vec4<f32> = col;
    let x_643 : vec4<f32> = col;
    let x_645 : f32 = ao;
    let x_649 : f32 = ubo.occlusionStrength;
    let x_651 : vec3<f32> = mix(vec3<f32>(x_641.x, x_641.y, x_641.z), (vec3<f32>(x_643.x, x_643.y, x_643.z) * x_645), vec3<f32>(x_649, x_649, x_649));
    let x_652 : vec4<f32> = col;
    col = vec4<f32>(x_651.x, x_651.y, x_651.z, x_652.w);
  }
  let x_656 : i32 = ubo.useEmissiveTexture;
  if ((x_656 != 0i)) {
    let x_666 : vec2<f32> = f_Texcoord;
    let x_667 : vec4<f32> = textureSample(emissiveTexture, emissiveTextureSampler, x_666);
    param_10 = x_667;
    let x_669 : vec4<f32> = SRGBtoLINEAR_vf4_(&(param_10));
    let x_672 : vec4<f32> = ubo.emissiveFactor;
    emissive = (vec3<f32>(x_669.x, x_669.y, x_669.z) * vec3<f32>(x_672.x, x_672.y, x_672.z));
    let x_675 : vec3<f32> = emissive;
    let x_676 : vec4<f32> = col;
    let x_678 : vec3<f32> = (vec3<f32>(x_676.x, x_676.y, x_676.z) + x_675);
    let x_679 : vec4<f32> = col;
    col = vec4<f32>(x_678.x, x_678.y, x_678.z, x_679.w);
  }
  var x_712 : bool;
  let x_683 : vec4<f32> = f_LightSpacePos;
  let x_687 : f32 = f_LightSpacePos.w;
  lsp_1 = (vec3<f32>(x_683.x, x_683.y, x_683.z) / vec3<f32>(x_687, x_687, x_687));
  let x_690 : vec3<f32> = lsp_1;
  lsp_1 = ((x_690 * 0.5f) + vec3<f32>(0.5f, 0.5f, 0.5f));
  shadowCol = 1.0f;
  let x_699 : f32 = f_LightSpacePos.z;
  let x_700 : bool = (x_699 <= 0.0f);
  x_714 = x_700;
  if (!(x_700)) {
    let x_705 : f32 = lsp_1.x;
    let x_706 : bool = (x_705 < 0.0f);
    x_713 = x_706;
    if (!(x_706)) {
      let x_711 : f32 = lsp_1.y;
      x_712 = (x_711 < 0.0f);
      x_713 = x_712;
    }
    x_714 = x_713;
  }
  var x_726 : bool;
  x_728 = x_714;
  if (!(x_714)) {
    let x_719 : f32 = lsp_1.x;
    let x_720 : bool = (x_719 > 1.0f);
    x_727 = x_720;
    if (!(x_720)) {
      let x_725 : f32 = lsp_1.y;
      x_726 = (x_725 > 1.0f);
      x_727 = x_726;
    }
    x_728 = x_727;
  }
  outSide = x_728;
  let x_729 : bool = outSide;
  if (!(x_729)) {
    let x_734 : vec3<f32> = lsp_1;
    param_11 = x_734;
    let x_736 : vec3<f32> = n_1;
    param_12 = x_736;
    let x_738 : vec3<f32> = l;
    param_13 = x_738;
    let x_739 : f32 = CalcShadow_vf3_vf3_vf3_(&(param_11), &(param_12), &(param_13));
    shadowCol = x_739;
  }
  let x_740 : f32 = shadowCol;
  let x_741 : vec4<f32> = col;
  let x_743 : vec3<f32> = (vec3<f32>(x_741.x, x_741.y, x_741.z) * x_740);
  let x_744 : vec4<f32> = col;
  col = vec4<f32>(x_743.x, x_743.y, x_743.z, x_744.w);
  let x_746 : vec4<f32> = col;
  let x_748 : vec3<f32> = pow(vec3<f32>(x_746.x, x_746.y, x_746.z), x_263);
  let x_749 : vec4<f32> = col;
  col = vec4<f32>(x_748.x, x_748.y, x_748.z, x_749.w);
  let x_752 : f32 = baseColor.w;
  col.w = x_752;
  let x_756 : vec4<f32> = col;
  outColor = x_756;
  return;
}

struct main_out {
  @location(0)
  outColor_1 : vec4<f32>,
}

@fragment
fn main(@location(3) f_WorldTangent_param : vec3<f32>, @location(4) f_WorldBioTangent_param : vec3<f32>, @location(0) f_WorldNormal_param : vec3<f32>, @location(1) f_Texcoord_param : vec2<f32>, @location(2) f_WorldPos_param : vec4<f32>, @location(5) f_LightSpacePos_param : vec4<f32>) -> main_out {
  f_WorldTangent = f_WorldTangent_param;
  f_WorldBioTangent = f_WorldBioTangent_param;
  f_WorldNormal = f_WorldNormal_param;
  f_Texcoord = f_Texcoord_param;
  f_WorldPos = f_WorldPos_param;
  f_LightSpacePos = f_LightSpacePos_param;
  main_1();
  return main_out(outColor);
}
