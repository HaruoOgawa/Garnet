struct FragUniformBufferObject {
    lightDir: vec4<f32>,
    lightColor: vec4<f32>,
    cameraPos: vec4<f32>,
    diffuseFactor: vec4<f32>,
    ambientFactor: vec4<f32>,
    specularFactor: vec4<f32>,
    edgeColor: vec4<f32>,
    specularIntensity: f32,
    f_pad0_: f32,
    f_pad1_: f32,
    f_pad2_: f32,
    UseMainTexture: i32,
    UseToonTexture: i32,
    UseSphereTexture: i32,
    SphereMode: i32,
    drawPathIndex: i32,
    iPad0_: i32,
    iPad1_: i32,
    iPad2_: i32,
    mPad0_: mat4x4<f32>,
    mPad1_: mat4x4<f32>,
    mPad2_: mat4x4<f32>,
    mPad3_: mat4x4<f32>,
}

@group(0) @binding(2) 
var<uniform> fragUbo: FragUniformBufferObject;
var<private> f_WorldNormal_1: vec3<f32>;
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
    let _e54 = fragUbo.drawPathIndex;
    if (_e54 == 1) {
        let _e56 = f_WorldNormal_1;
        let _e58 = fragUbo.lightDir;
        NdotL = max(0.0, dot(_e56, -(_e58.xyz)));
        let _e64 = fragUbo.cameraPos;
        let _e66 = f_WorldPos_1;
        v = normalize((_e64.xyz - _e66.xyz));
        let _e71 = fragUbo.lightDir;
        l = (_e71.xyz * -1.0);
        let _e74 = v;
        let _e75 = l;
        HalfVector = normalize((_e74 + _e75));
        let _e79 = fragUbo.diffuseFactor;
        diffuseColor = _e79;
        let _e81 = fragUbo.UseToonTexture;
        if (_e81 == 0) {
            let _e84 = fragUbo.ambientFactor;
            let _e87 = fragUbo.lightDir;
            let _e90 = f_WorldNormal_1;
            let _e94 = diffuseColor;
            let _e96 = (_e94.xyz + (_e84.xyz * max(dot(-(_e87.xyz), _e90), 0.0)));
            diffuseColor[0u] = _e96.x;
            diffuseColor[1u] = _e96.y;
            diffuseColor[2u] = _e96.z;
        }
        let _e104 = fragUbo.UseMainTexture;
        if (_e104 != 0) {
            let _e106 = f_Texcoord_1;
            let _e107 = textureSample(MainTexture, MainTextureSampler, _e106);
            MainColor = _e107;
            let _e108 = MainColor;
            let _e110 = diffuseColor;
            let _e112 = (_e110.xyz * _e108.xyz);
            diffuseColor[0u] = _e112.x;
            diffuseColor[1u] = _e112.y;
            diffuseColor[2u] = _e112.z;
        }
        let _e119 = diffuseColor;
        col = _e119.xyz;
        let _e122 = diffuseColor[3u];
        alpha = _e122;
        let _e124 = fragUbo.UseSphereTexture;
        if (_e124 != 0) {
            let _e126 = f_SphereUV_1;
            let _e127 = textureSample(SphereTexture, SphereTextureSampler, _e126);
            SphereColor = _e127.xyz;
            let _e130 = fragUbo.SphereMode;
            if (_e130 == 1) {
                let _e132 = SphereColor;
                let _e133 = col;
                col = (_e133 * _e132);
            } else {
                let _e136 = fragUbo.SphereMode;
                if (_e136 == 2) {
                    let _e138 = SphereColor;
                    let _e139 = col;
                    col = (_e139 + _e138);
                }
            }
        }
        let _e142 = fragUbo.UseToonTexture;
        if (_e142 != 0) {
            let _e144 = NdotL;
            let _e146 = textureSample(ToonTexture, ToonTextureSampler, vec2<f32>(0.0, _e144));
            ToonColor = _e146.xyz;
            let _e148 = ToonColor;
            let _e149 = NdotL;
            let _e155 = col;
            col = (_e155 * mix(_e148, vec3<f32>(1.0, 1.0, 1.0), vec3<f32>(clamp(((_e149 * 16.0) + 0.5), 0.0, 1.0))));
        }
        let _e158 = fragUbo.specularFactor;
        let _e160 = HalfVector;
        let _e161 = f_WorldNormal_1;
        let _e164 = fragUbo.specularIntensity;
        specularColor = (_e158.xyz * max(0.0, pow(dot(_e160, _e161), _e164)));
        let _e168 = specularColor;
        let _e169 = col;
        col = (_e169 + _e168);
    } else {
        let _e172 = fragUbo.drawPathIndex;
        if (_e172 == 2) {
            let _e175 = fragUbo.edgeColor;
            col = _e175.xyz;
        }
    }
    let _e177 = col;
    let _e178 = alpha;
    outColor = vec4<f32>(_e177.x, _e177.y, _e177.z, _e178);
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
