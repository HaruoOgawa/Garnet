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
    useCubeMap: i32,
    useShadowMap: i32,
    useIBL: i32,
    useSkinMeshAnimation: i32,
    useDirCubemap: i32,
    pad1_: i32,
    pad2_: i32,
}

struct FragmentOutput {
    @location(0) member: vec4<f32>,
    @location(1) member_1: vec4<f32>,
    @location(2) member_2: vec4<f32>,
}

@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> f_WorldTangent_1: vec3<f32>;
var<private> f_WorldBioTangent_1: vec3<f32>;
var<private> f_WorldNormal_1: vec3<f32>;
@group(0) @binding(8) 
var normalTexture: texture_2d<f32>;
@group(0) @binding(9) 
var normalTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
@group(0) @binding(4) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(5) 
var metallicRoughnessTextureSampler: sampler;
@group(0) @binding(2) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(3) 
var baseColorTextureSampler: sampler;
var<private> gPosition: vec4<f32>;
var<private> f_WorldPos_1: vec4<f32>;
var<private> gNormal: vec4<f32>;
var<private> gAlbedo: vec4<f32>;
var<private> f_LightSpacePos_1: vec4<f32>;
@group(0) @binding(6) 
var emissiveTexture: texture_2d<f32>;
@group(0) @binding(7) 
var emissiveTextureSampler: sampler;
@group(0) @binding(10) 
var occlusionTexture: texture_2d<f32>;
@group(0) @binding(11) 
var occlusionTextureSampler: sampler;
@group(0) @binding(12) 
var cubemapTexture: texture_cube<f32>;
@group(0) @binding(13) 
var cubemapTextureSampler: sampler;
@group(0) @binding(14) 
var shadowmapTexture: texture_2d<f32>;
@group(0) @binding(15) 
var shadowmapTextureSampler: sampler;
@group(0) @binding(16) 
var IBL_Diffuse_Texture: texture_2d<f32>;
@group(0) @binding(17) 
var IBL_Diffuse_TextureSampler: sampler;
@group(0) @binding(18) 
var IBL_Specular_Texture: texture_2d<f32>;
@group(0) @binding(19) 
var IBL_Specular_TextureSampler: sampler;
@group(0) @binding(20) 
var IBL_GGXLUT_Texture: texture_2d<f32>;
@group(0) @binding(21) 
var IBL_GGXLUT_TextureSampler: sampler;
@group(0) @binding(22) 
var cubeMap2DTexture: texture_2d<f32>;
@group(0) @binding(23) 
var cubeMap2DTextureSampler: sampler;

fn getNormal() -> vec3<f32> {
    var nomral: vec3<f32>;
    var t: vec3<f32>;
    var b: vec3<f32>;
    var n: vec3<f32>;
    var tbn: mat3x3<f32>;

    nomral = vec3<f32>(0.0, 0.0, 0.0);
    let _e59 = ubo.useNormalTexture;
    if (_e59 != 0) {
        let _e61 = f_WorldTangent_1;
        t = normalize(_e61);
        let _e63 = f_WorldBioTangent_1;
        b = normalize(_e63);
        let _e65 = f_WorldNormal_1;
        n = normalize(_e65);
        let _e67 = t;
        let _e68 = b;
        let _e69 = n;
        tbn = mat3x3<f32>(vec3<f32>(_e67.x, _e67.y, _e67.z), vec3<f32>(_e68.x, _e68.y, _e68.z), vec3<f32>(_e69.x, _e69.y, _e69.z));
        let _e83 = f_Texcoord_1;
        let _e84 = textureSample(normalTexture, normalTextureSampler, _e83);
        nomral = _e84.xyz;
        let _e86 = tbn;
        let _e87 = nomral;
        let _e92 = ubo.normalMapScale;
        let _e94 = ubo.normalMapScale;
        nomral = normalize((_e86 * (((_e87 * 2.0) - vec3<f32>(1.0)) * vec3<f32>(_e92, _e94, 1.0))));
    } else {
        let _e99 = f_WorldNormal_1;
        nomral = _e99;
    }
    let _e100 = nomral;
    return _e100;
}

fn main_1() {
    var col: vec4<f32>;
    var perceptualRoughness: f32;
    var metallic: f32;
    var metallicRoughnessColor: vec4<f32>;
    var baseColor: vec4<f32>;
    var n_1: vec3<f32>;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e60 = ubo.roughnessFactor;
    perceptualRoughness = _e60;
    let _e62 = ubo.metallicFactor;
    metallic = _e62;
    let _e64 = ubo.useMetallicRoughnessTexture;
    if (_e64 != 0) {
        let _e66 = f_Texcoord_1;
        let _e67 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e66);
        metallicRoughnessColor = _e67;
        let _e68 = perceptualRoughness;
        let _e70 = metallicRoughnessColor[1u];
        perceptualRoughness = (_e68 * _e70);
        let _e72 = metallic;
        let _e74 = metallicRoughnessColor[2u];
        metallic = (_e72 * _e74);
    }
    let _e76 = perceptualRoughness;
    perceptualRoughness = clamp(_e76, 0.03999999910593033, 1.0);
    let _e78 = metallic;
    metallic = clamp(_e78, 0.0, 1.0);
    let _e81 = ubo.useBaseColorTexture;
    if (_e81 != 0) {
        let _e83 = f_Texcoord_1;
        let _e84 = textureSample(baseColorTexture, baseColorTextureSampler, _e83);
        baseColor = _e84;
    } else {
        let _e86 = ubo.baseColorFactor;
        baseColor = _e86;
    }
    let _e87 = getNormal();
    n_1 = _e87;
    let _e88 = f_WorldPos_1;
    gPosition = _e88;
    let _e89 = n_1;
    gNormal = vec4<f32>(_e89.x, _e89.y, _e89.z, 1.0);
    let _e94 = baseColor;
    gAlbedo = _e94;
    return;
}

@fragment 
fn main(@location(3) f_WorldTangent: vec3<f32>, @location(4) f_WorldBioTangent: vec3<f32>, @location(0) f_WorldNormal: vec3<f32>, @location(1) f_Texcoord: vec2<f32>, @location(2) f_WorldPos: vec4<f32>, @location(5) f_LightSpacePos: vec4<f32>) -> FragmentOutput {
    f_WorldTangent_1 = f_WorldTangent;
    f_WorldBioTangent_1 = f_WorldBioTangent;
    f_WorldNormal_1 = f_WorldNormal;
    f_Texcoord_1 = f_Texcoord;
    f_WorldPos_1 = f_WorldPos;
    f_LightSpacePos_1 = f_LightSpacePos;
    main_1();
    let _e15 = gPosition;
    let _e16 = gNormal;
    let _e17 = gAlbedo;
    return FragmentOutput(_e15, _e16, _e17);
}
