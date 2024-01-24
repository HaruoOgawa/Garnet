struct FragUniformBufferObject {
    lightDir: vec4<f32>,
    lightColor: vec4<f32>,
    cameraPos: vec4<f32>,
    diffuseFactor: vec4<f32>,
    ambientFactor: vec4<f32>,
    specularFactor: vec4<f32>,
    specularIntensity: f32,
    f_pad0_: f32,
    f_pad1_: f32,
    f_pad2_: f32,
    UseMainTexture: i32,
    UseToonTexture: i32,
    UseSphereTexture: i32,
    SphereMode: i32,
    mPad0_: mat4x4<f32>,
    mPad1_: mat4x4<f32>,
    mPad2_: mat4x4<f32>,
    mPad3_: mat4x4<f32>,
}

var<private> f_WorldNormal_1: vec3<f32>;
@group(0) @binding(2) 
var<uniform> fragUbo: FragUniformBufferObject;
var<private> f_WorldPos_1: vec4<f32>;
@group(0) @binding(3) 
var MainTexture: texture_2d<f32>;
@group(0) @binding(4) 
var MainTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
@group(0) @binding(7) 
var SphereTexture: texture_2d<f32>;
@group(0) @binding(8) 
var SphereTextureSampler: sampler;
var<private> f_SphereUV_1: vec2<f32>;
@group(0) @binding(5) 
var ToonTexture: texture_2d<f32>;
@group(0) @binding(6) 
var ToonTextureSampler: sampler;
var<private> outColor: vec4<f32>;
var<private> f_WorldTangent_1: vec3<f32>;
var<private> f_WorldBioTangent_1: vec3<f32>;
var<private> f_LightSpacePos_1: vec4<f32>;

fn main_1() {
    var col: vec3<f32>;
    var alpha: f32;
    var NdotL: f32;
    var v: vec3<f32>;
    var l: vec3<f32>;
    var HalfVector: vec3<f32>;
    var diffuseColor: vec4<f32>;
    var MainColor: vec4<f32>;
    var SphereColor: vec3<f32>;
    var ToonColor: vec3<f32>;
    var specularColor: vec3<f32>;

    col = vec3<f32>(1.0, 1.0, 1.0);
    alpha = 1.0;
    let _e51 = f_WorldNormal_1;
    let _e53 = fragUbo.lightDir;
    NdotL = max(0.0, dot(_e51, -(_e53.xyz)));
    let _e59 = fragUbo.cameraPos;
    let _e61 = f_WorldPos_1;
    v = normalize((_e59.xyz - _e61.xyz));
    let _e66 = fragUbo.lightDir;
    l = (_e66.xyz * -1.0);
    let _e69 = v;
    let _e70 = l;
    HalfVector = normalize((_e69 + _e70));
    let _e74 = fragUbo.diffuseFactor;
    diffuseColor = _e74;
    let _e76 = fragUbo.UseToonTexture;
    if (_e76 == 0) {
        let _e79 = fragUbo.ambientFactor;
        let _e82 = fragUbo.lightDir;
        let _e85 = f_WorldNormal_1;
        let _e89 = diffuseColor;
        let _e91 = (_e89.xyz + (_e79.xyz * max(dot(-(_e82.xyz), _e85), 0.0)));
        diffuseColor[0u] = _e91.x;
        diffuseColor[1u] = _e91.y;
        diffuseColor[2u] = _e91.z;
    }
    let _e99 = fragUbo.UseMainTexture;
    if (_e99 != 0) {
        let _e101 = f_Texcoord_1;
        let _e102 = textureSample(MainTexture, MainTextureSampler, _e101);
        MainColor = _e102;
        let _e103 = MainColor;
        let _e105 = diffuseColor;
        let _e107 = (_e105.xyz * _e103.xyz);
        diffuseColor[0u] = _e107.x;
        diffuseColor[1u] = _e107.y;
        diffuseColor[2u] = _e107.z;
    }
    let _e114 = diffuseColor;
    col = _e114.xyz;
    let _e117 = diffuseColor[3u];
    alpha = _e117;
    let _e119 = fragUbo.UseSphereTexture;
    if (_e119 != 0) {
        let _e121 = f_SphereUV_1;
        let _e122 = textureSample(SphereTexture, SphereTextureSampler, _e121);
        SphereColor = _e122.xyz;
        let _e125 = fragUbo.SphereMode;
        if (_e125 == 1) {
            let _e127 = SphereColor;
            let _e128 = col;
            col = (_e128 * _e127);
        } else {
            let _e131 = fragUbo.SphereMode;
            if (_e131 == 2) {
                let _e133 = SphereColor;
                let _e134 = col;
                col = (_e134 + _e133);
            }
        }
    }
    let _e137 = fragUbo.UseToonTexture;
    if (_e137 != 0) {
        let _e139 = NdotL;
        let _e141 = textureSample(ToonTexture, ToonTextureSampler, vec2<f32>(0.0, _e139));
        ToonColor = _e141.xyz;
        let _e143 = ToonColor;
        let _e144 = NdotL;
        let _e150 = col;
        col = (_e150 * mix(_e143, vec3<f32>(1.0, 1.0, 1.0), vec3<f32>(clamp(((_e144 * 16.0) + 0.5), 0.0, 1.0))));
    }
    let _e153 = fragUbo.specularFactor;
    let _e155 = HalfVector;
    let _e156 = f_WorldNormal_1;
    let _e159 = fragUbo.specularIntensity;
    specularColor = (_e153.xyz * max(0.0, pow(dot(_e155, _e156), _e159)));
    let _e163 = specularColor;
    let _e164 = col;
    col = (_e164 + _e163);
    let _e166 = col;
    let _e167 = alpha;
    outColor = vec4<f32>(_e166.x, _e166.y, _e166.z, _e167);
    return;
}

@fragment 
fn main(@location(0) f_WorldNormal: vec3<f32>, @location(2) f_WorldPos: vec4<f32>, @location(1) f_Texcoord: vec2<f32>, @location(6) f_SphereUV: vec2<f32>, @location(3) f_WorldTangent: vec3<f32>, @location(4) f_WorldBioTangent: vec3<f32>, @location(5) f_LightSpacePos: vec4<f32>) -> @location(0) vec4<f32> {
    f_WorldNormal_1 = f_WorldNormal;
    f_WorldPos_1 = f_WorldPos;
    f_Texcoord_1 = f_Texcoord;
    f_SphereUV_1 = f_SphereUV;
    f_WorldTangent_1 = f_WorldTangent;
    f_WorldBioTangent_1 = f_WorldBioTangent;
    f_LightSpacePos_1 = f_LightSpacePos;
    main_1();
    let _e15 = outColor;
    return _e15;
}
