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
  var x : i32;
  var y : i32;
  moments = vec2<f32>(0.0f, 0.0f);
  let x_278 : f32 = ubo.ShadowMapX;
  let x_282 : f32 = ubo.ShadowMapY;
  texelSize = vec2<f32>((1.0f / x_278), (1.0f / x_282));
  x = -1i;
  loop {
    let x_293 : i32 = x;
    if ((x_293 <= 1i)) {
    } else {
      break;
    }
    y = -1i;
    loop {
      let x_301 : i32 = y;
      if ((x_301 <= 1i)) {
      } else {
        break;
      }
      let x_308 : vec2<f32> = *(uv);
      let x_309 : i32 = x;
      let x_311 : i32 = y;
      let x_314 : vec2<f32> = texelSize;
      let x_317 : vec4<f32> = textureSample(shadowmapTexture, shadowmapTextureSampler, (x_308 + (vec2<f32>(f32(x_309), f32(x_311)) * x_314)));
      let x_319 : vec2<f32> = moments;
      moments = (x_319 + vec2<f32>(x_317.x, x_317.y));

      continuing {
        let x_321 : i32 = y;
        y = (x_321 + 1i);
      }
    }

    continuing {
      let x_323 : i32 = x;
      x = (x_323 + 1i);
    }
  }
  let x_326 : vec2<f32> = moments;
  moments = (x_326 / vec2<f32>(9.0f, 9.0f));
  let x_329 : vec2<f32> = moments;
  return x_329;
}

fn CalcShadow_vf3_vf3_vf3_(lsp : ptr<function, vec3<f32>>, nomral : ptr<function, vec3<f32>>, lightDir : ptr<function, vec3<f32>>) -> f32 {
  var moments_1 : vec2<f32>;
  var param_4 : vec2<f32>;
  var ShadowBias : f32;
  var distance : f32;
  var variance : f32;
  var d : f32;
  var p_max : f32;
  let x_334 : vec3<f32> = *(lsp);
  param_4 = vec2<f32>(x_334.x, x_334.y);
  let x_336 : vec2<f32> = ComputePCF_vf2_(&(param_4));
  moments_1 = x_336;
  let x_340 : vec3<f32> = *(nomral);
  let x_341 : vec3<f32> = *(lightDir);
  ShadowBias = max(0.00499999988824129105f, (0.05000000074505805969f * (1.0f - dot(x_340, x_341))));
  let x_349 : f32 = (*(lsp)).z;
  let x_350 : f32 = ShadowBias;
  distance = (x_349 - x_350);
  let x_352 : f32 = distance;
  let x_355 : f32 = moments_1.x;
  if ((x_352 <= x_355)) {
    return 1.0f;
  }
  let x_363 : f32 = moments_1.y;
  let x_365 : f32 = moments_1.x;
  let x_367 : f32 = moments_1.x;
  variance = (x_363 - (x_365 * x_367));
  let x_370 : f32 = variance;
  variance = max(0.00499999988824129105f, x_370);
  let x_373 : f32 = distance;
  let x_375 : f32 = moments_1.x;
  d = (x_373 - x_375);
  let x_378 : f32 = variance;
  let x_379 : f32 = variance;
  let x_380 : f32 = d;
  let x_381 : f32 = d;
  p_max = (x_378 / (x_379 + (x_380 * x_381)));
  let x_385 : f32 = p_max;
  return x_385;
}

const x_451 = vec3<f32>(1.0f, 1.0f, 1.0f);

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
  var x_716 : bool;
  var x_717 : bool;
  var x_730 : bool;
  var x_731 : bool;
  col = vec4<f32>(1.0f, 1.0f, 1.0f, 1.0f);
  let x_393 : f32 = ubo.roughnessFactor;
  perceptualRoughness = x_393;
  let x_396 : f32 = ubo.metallicFactor;
  metallic = x_396;
  let x_399 : i32 = ubo.useMetallicRoughnessTexture;
  if ((x_399 != 0i)) {
    let x_409 : vec2<f32> = f_Texcoord;
    let x_410 : vec4<f32> = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, x_409);
    metallicRoughnessColor = x_410;
    let x_411 : f32 = perceptualRoughness;
    let x_413 : f32 = metallicRoughnessColor.y;
    perceptualRoughness = (x_411 * x_413);
    let x_415 : f32 = metallic;
    let x_417 : f32 = metallicRoughnessColor.z;
    metallic = (x_415 * x_417);
  }
  let x_419 : f32 = perceptualRoughness;
  perceptualRoughness = clamp(x_419, 0.03999999910593032837f, 1.0f);
  let x_422 : f32 = metallic;
  metallic = clamp(x_422, 0.0f, 1.0f);
  let x_425 : f32 = perceptualRoughness;
  let x_426 : f32 = perceptualRoughness;
  alphaRoughness = (x_425 * x_426);
  let x_430 : i32 = ubo.useBaseColorTexture;
  if ((x_430 != 0i)) {
    let x_440 : vec2<f32> = f_Texcoord;
    let x_441 : vec4<f32> = textureSample(baseColorTexture, baseColorTextureSampler, x_440);
    baseColor = x_441;
  } else {
    let x_445 : vec4<f32> = ubo.baseColorFactor;
    baseColor = x_445;
  }
  f0 = vec3<f32>(0.03999999910593032837f, 0.03999999910593032837f, 0.03999999910593032837f);
  let x_449 : vec4<f32> = baseColor;
  let x_452 : vec3<f32> = f0;
  diffuseColor = (vec3<f32>(x_449.x, x_449.y, x_449.z) * (x_451 - x_452));
  let x_455 : f32 = metallic;
  let x_457 : vec3<f32> = diffuseColor;
  diffuseColor = (x_457 * (1.0f - x_455));
  let x_460 : vec3<f32> = f0;
  let x_461 : vec4<f32> = baseColor;
  let x_463 : f32 = metallic;
  specularColor = mix(x_460, vec3<f32>(x_461.x, x_461.y, x_461.z), vec3<f32>(x_463, x_463, x_463));
  let x_468 : f32 = specularColor.x;
  let x_470 : f32 = specularColor.y;
  let x_473 : f32 = specularColor.z;
  reflectance = max(max(x_468, x_470), x_473);
  let x_476 : f32 = reflectance;
  reflectance90 = clamp((x_476 * 25.0f), 0.0f, 1.0f);
  let x_481 : vec3<f32> = specularColor;
  specularEnvironmentR0 = x_481;
  let x_483 : f32 = reflectance90;
  specularEnvironmentR90 = (x_451 * x_483);
  let x_486 : vec3<f32> = getNormal_();
  n_1 = x_486;
  let x_490 : vec4<f32> = ubo.cameraPos;
  let x_494 : vec4<f32> = f_WorldPos;
  v = normalize((vec3<f32>(x_490.x, x_490.y, x_490.z) - vec3<f32>(x_494.x, x_494.y, x_494.z)));
  let x_500 : vec4<f32> = ubo.lightDir;
  l = normalize(vec3<f32>(x_500.x, x_500.y, x_500.z));
  let x_504 : vec3<f32> = v;
  let x_505 : vec3<f32> = l;
  h = normalize((x_504 + x_505));
  let x_509 : vec3<f32> = v;
  let x_510 : vec3<f32> = n_1;
  reflection = -(normalize(reflect(x_509, x_510)));
  let x_515 : vec3<f32> = n_1;
  let x_516 : vec3<f32> = l;
  NdotL_1 = clamp(dot(x_515, x_516), 0.00100000004749745131f, 1.0f);
  let x_521 : vec3<f32> = n_1;
  let x_522 : vec3<f32> = v;
  NdotV_1 = clamp(abs(dot(x_521, x_522)), 0.00100000004749745131f, 1.0f);
  let x_527 : vec3<f32> = n_1;
  let x_528 : vec3<f32> = h;
  NdotH = clamp(dot(x_527, x_528), 0.0f, 1.0f);
  let x_532 : vec3<f32> = l;
  let x_533 : vec3<f32> = h;
  LdotH = clamp(dot(x_532, x_533), 0.0f, 1.0f);
  let x_537 : vec3<f32> = v;
  let x_538 : vec3<f32> = h;
  VdotH = clamp(dot(x_537, x_538), 0.0f, 1.0f);
  let x_542 : f32 = NdotL_1;
  let x_543 : f32 = NdotV_1;
  let x_544 : f32 = NdotH;
  let x_545 : f32 = LdotH;
  let x_546 : f32 = VdotH;
  let x_547 : f32 = perceptualRoughness;
  let x_548 : f32 = metallic;
  let x_549 : vec3<f32> = specularEnvironmentR0;
  let x_550 : vec3<f32> = specularEnvironmentR90;
  let x_551 : f32 = alphaRoughness;
  let x_552 : vec3<f32> = diffuseColor;
  let x_553 : vec3<f32> = specularColor;
  pbrParam = PBRParam(x_542, x_543, x_544, x_545, x_546, x_547, x_548, x_549, x_550, x_551, x_552, x_553);
  let x_557 : PBRParam = pbrParam;
  param_5 = x_557;
  let x_558 : f32 = CalcMicrofacet_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(&(param_5));
  D = x_558;
  let x_561 : PBRParam = pbrParam;
  param_6 = x_561;
  let x_562 : f32 = CalcGeometricOcculusion_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(&(param_6));
  G = x_562;
  let x_565 : PBRParam = pbrParam;
  param_7 = x_565;
  let x_566 : vec3<f32> = CalcFrenelReflection_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(&(param_7));
  F = x_566;
  let x_568 : f32 = D;
  let x_569 : f32 = G;
  let x_571 : vec3<f32> = F;
  let x_574 : f32 = NdotL_1;
  let x_576 : f32 = NdotV_1;
  let x_577 : f32 = ((4.0f * x_574) * x_576);
  specularBRDF = ((x_571 * (x_568 * x_569)) / vec3<f32>(x_577, x_577, x_577));
  let x_581 : vec3<f32> = F;
  let x_585 : PBRParam = pbrParam;
  param_8 = x_585;
  let x_586 : vec3<f32> = CalcDiffuseBRDF_struct_PBRParam_f1_f1_f1_f1_f1_f1_f1_vf3_vf3_f1_vf3_vf31_(&(param_8));
  diffuseBRDF = ((vec3<f32>(1.0f, 1.0f, 1.0f) - x_581) * x_586);
  let x_591 : f32 = ubo.mipCount;
  mipCount = x_591;
  let x_593 : f32 = mipCount;
  let x_594 : f32 = perceptualRoughness;
  lod = (x_593 * x_594);
  let x_605 : vec3<f32> = v;
  let x_606 : vec3<f32> = n_1;
  let x_608 : f32 = lod;
  let x_609 : vec4<f32> = textureSampleLevel(cubemapTexture, cubemapTextureSampler, reflect(x_605, x_606), x_608);
  param_9 = x_609;
  let x_611 : vec4<f32> = LINEARtoSRGB_vf4_(&(param_9));
  reflectColor = vec3<f32>(x_611.x, x_611.y, x_611.z);
  let x_613 : f32 = NdotL_1;
  let x_616 : vec4<f32> = ubo.lightColor;
  let x_619 : vec3<f32> = specularBRDF;
  let x_620 : vec3<f32> = diffuseBRDF;
  let x_623 : vec3<f32> = reflectColor;
  let x_624 : vec3<f32> = specularColor;
  let x_626 : vec3<f32> = (((vec3<f32>(x_616.x, x_616.y, x_616.z) * x_613) * (x_619 + x_620)) + (x_623 * x_624));
  let x_627 : vec4<f32> = col;
  col = vec4<f32>(x_626.x, x_626.y, x_626.z, x_627.w);
  let x_631 : i32 = ubo.useOcclusionTexture;
  if ((x_631 != 0i)) {
    let x_641 : vec2<f32> = f_Texcoord;
    let x_642 : vec4<f32> = textureSample(occlusionTexture, occlusionTextureSampler, x_641);
    ao = x_642.x;
    let x_644 : vec4<f32> = col;
    let x_646 : vec4<f32> = col;
    let x_648 : f32 = ao;
    let x_652 : f32 = ubo.occlusionStrength;
    let x_654 : vec3<f32> = mix(vec3<f32>(x_644.x, x_644.y, x_644.z), (vec3<f32>(x_646.x, x_646.y, x_646.z) * x_648), vec3<f32>(x_652, x_652, x_652));
    let x_655 : vec4<f32> = col;
    col = vec4<f32>(x_654.x, x_654.y, x_654.z, x_655.w);
  }
  let x_659 : i32 = ubo.useEmissiveTexture;
  if ((x_659 != 0i)) {
    let x_669 : vec2<f32> = f_Texcoord;
    let x_670 : vec4<f32> = textureSample(emissiveTexture, emissiveTextureSampler, x_669);
    param_10 = x_670;
    let x_672 : vec4<f32> = SRGBtoLINEAR_vf4_(&(param_10));
    let x_675 : vec4<f32> = ubo.emissiveFactor;
    emissive = (vec3<f32>(x_672.x, x_672.y, x_672.z) * vec3<f32>(x_675.x, x_675.y, x_675.z));
    let x_678 : vec3<f32> = emissive;
    let x_679 : vec4<f32> = col;
    let x_681 : vec3<f32> = (vec3<f32>(x_679.x, x_679.y, x_679.z) + x_678);
    let x_682 : vec4<f32> = col;
    col = vec4<f32>(x_681.x, x_681.y, x_681.z, x_682.w);
  }
  var x_715 : bool;
  let x_686 : vec4<f32> = f_LightSpacePos;
  let x_690 : f32 = f_LightSpacePos.w;
  lsp_1 = (vec3<f32>(x_686.x, x_686.y, x_686.z) / vec3<f32>(x_690, x_690, x_690));
  let x_693 : vec3<f32> = lsp_1;
  lsp_1 = ((x_693 * 0.5f) + vec3<f32>(0.5f, 0.5f, 0.5f));
  shadowCol = 1.0f;
  let x_702 : f32 = f_LightSpacePos.z;
  let x_703 : bool = (x_702 <= 0.0f);
  x_717 = x_703;
  if (!(x_703)) {
    let x_708 : f32 = lsp_1.x;
    let x_709 : bool = (x_708 < 0.0f);
    x_716 = x_709;
    if (!(x_709)) {
      let x_714 : f32 = lsp_1.y;
      x_715 = (x_714 < 0.0f);
      x_716 = x_715;
    }
    x_717 = x_716;
  }
  var x_729 : bool;
  x_731 = x_717;
  if (!(x_717)) {
    let x_722 : f32 = lsp_1.x;
    let x_723 : bool = (x_722 > 1.0f);
    x_730 = x_723;
    if (!(x_723)) {
      let x_728 : f32 = lsp_1.y;
      x_729 = (x_728 > 1.0f);
      x_730 = x_729;
    }
    x_731 = x_730;
  }
  outSide = x_731;
  let x_732 : bool = outSide;
  if (!(x_732)) {
    let x_737 : vec3<f32> = lsp_1;
    param_11 = x_737;
    let x_739 : vec3<f32> = n_1;
    param_12 = x_739;
    let x_741 : vec3<f32> = l;
    param_13 = x_741;
    let x_742 : f32 = CalcShadow_vf3_vf3_vf3_(&(param_11), &(param_12), &(param_13));
    shadowCol = x_742;
  }
  let x_743 : f32 = shadowCol;
  let x_744 : vec4<f32> = col;
  let x_746 : vec3<f32> = (vec3<f32>(x_744.x, x_744.y, x_744.z) * x_743);
  let x_747 : vec4<f32> = col;
  col = vec4<f32>(x_746.x, x_746.y, x_746.z, x_747.w);
  let x_749 : vec4<f32> = col;
  let x_751 : vec3<f32> = pow(vec3<f32>(x_749.x, x_749.y, x_749.z), x_263);
  let x_752 : vec4<f32> = col;
  col = vec4<f32>(x_751.x, x_751.y, x_751.z, x_752.w);
  let x_755 : f32 = baseColor.w;
  col.w = x_755;
  let x_759 : vec4<f32> = col;
  outColor = x_759;
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
