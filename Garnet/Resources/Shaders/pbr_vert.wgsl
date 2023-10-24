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
var<private> inJoint0_1: vec4<i32>;
var<private> inWeights0_1: vec4<f32>;

fn main_1() {
    var pos: vec4<f32>;
    var BioTangent: vec3<f32>;

    let _e27 = inPosition_1;
    pos = vec4<f32>(_e27.x, _e27.y, _e27.z, 1.0);
    let _e32 = inNormal_1;
    let _e33 = inTangent_1;
    BioTangent = cross(_e32, _e33.xyz);
    let _e37 = ubo.proj;
    let _e39 = ubo.view;
    let _e42 = ubo.model;
    let _e44 = pos;
    perVertexStruct.gl_Position = (((_e37 * _e39) * _e42) * _e44);
    let _e48 = ubo.model;
    let _e49 = inNormal_1;
    f_WorldNormal = normalize((_e48 * vec4<f32>(_e49.x, _e49.y, _e49.z, 0.0)).xyz);
    let _e57 = inTexcoord_1;
    f_Texcoord = _e57;
    let _e59 = ubo.model;
    let _e60 = inPosition_1;
    f_WorldPos = (_e59 * vec4<f32>(_e60.x, _e60.y, _e60.z, 1.0));
    let _e67 = ubo.model;
    let _e68 = inTangent_1;
    f_WorldTangent = normalize((_e67 * _e68).xyz);
    let _e73 = ubo.model;
    let _e74 = BioTangent;
    f_WorldBioTangent = normalize((_e73 * vec4<f32>(_e74.x, _e74.y, _e74.z, 0.0)).xyz);
    let _e83 = ubo.lightVPMat;
    let _e85 = ubo.model;
    let _e87 = pos;
    f_LightSpacePos = ((_e83 * _e85) * _e87);
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>, @location(2) inTexcoord: vec2<f32>, @location(4) inJoint0_: vec4<i32>, @location(5) inWeights0_: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inTexcoord_1 = inTexcoord;
    inJoint0_1 = inJoint0_;
    inWeights0_1 = inWeights0_;
    main_1();
    let _e21 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e21);
    let _e23 = perVertexStruct.gl_Position;
    let _e24 = f_WorldNormal;
    let _e25 = f_Texcoord;
    let _e26 = f_WorldPos;
    let _e27 = f_WorldTangent;
    let _e28 = f_WorldBioTangent;
    let _e29 = f_LightSpacePos;
    return VertexOutput(_e23, _e24, _e25, _e26, _e27, _e28, _e29);
}
