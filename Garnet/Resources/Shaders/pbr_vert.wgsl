struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32,1u>,
    gl_CullDistance: array<f32,1u>,
}

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
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
    @location(1) member_1: vec2<f32>,
    @location(2) member_2: vec4<f32>,
    @location(3) member_3: vec3<f32>,
    @location(4) member_4: vec3<f32>,
    @location(5) member_5: vec4<f32>,
}

var<private> inPosition_1: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;
var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> f_WorldNormal: vec3<f32>;
var<private> f_Texcoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> f_WorldPos: vec4<f32>;
var<private> f_WorldTangent: vec3<f32>;
var<private> f_WorldBioTangent: vec3<f32>;
var<private> f_LightSpacePos: vec4<f32>;

fn main_1() {
    var pos: vec4<f32>;
    var BioTangent: vec3<f32>;

    let _e25 = inPosition_1;
    pos = vec4<f32>(_e25.x, _e25.y, _e25.z, 1.0);
    let _e30 = inNormal_1;
    let _e31 = inTangent_1;
    BioTangent = cross(_e30, _e31.xyz);
    let _e35 = ubo.proj;
    let _e37 = ubo.view;
    let _e40 = ubo.model;
    let _e42 = pos;
    perVertexStruct.gl_Position = (((_e35 * _e37) * _e40) * _e42);
    let _e46 = ubo.model;
    let _e47 = inNormal_1;
    f_WorldNormal = normalize((_e46 * vec4<f32>(_e47.x, _e47.y, _e47.z, 0.0)).xyz);
    let _e55 = inTexcoord_1;
    f_Texcoord = _e55;
    let _e57 = ubo.model;
    let _e58 = inPosition_1;
    f_WorldPos = (_e57 * vec4<f32>(_e58.x, _e58.y, _e58.z, 1.0));
    let _e65 = ubo.model;
    let _e66 = inTangent_1;
    f_WorldTangent = normalize((_e65 * _e66).xyz);
    let _e71 = ubo.model;
    let _e72 = BioTangent;
    f_WorldBioTangent = normalize((_e71 * vec4<f32>(_e72.x, _e72.y, _e72.z, 0.0)).xyz);
    let _e81 = ubo.lightVPMat;
    let _e83 = ubo.model;
    let _e85 = pos;
    f_LightSpacePos = ((_e81 * _e83) * _e85);
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(2) inTexcoord: vec2<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inTexcoord_1 = inTexcoord;
    main_1();
    let _e17 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e17);
    let _e19 = perVertexStruct.gl_Position;
    let _e20 = f_WorldNormal;
    let _e21 = f_Texcoord;
    let _e22 = f_WorldPos;
    let _e23 = f_WorldTangent;
    let _e24 = f_WorldBioTangent;
    let _e25 = f_LightSpacePos;
    return VertexOutput(_e19, _e20, _e21, _e22, _e23, _e24, _e25);
}
