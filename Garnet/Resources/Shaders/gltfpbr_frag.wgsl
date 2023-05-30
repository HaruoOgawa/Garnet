struct UniformBufferObject {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    emissiveFactor: vec4<f32>,
    time: f32,
    padding0_: f32,
    padding1_: f32,
    padding2_: f32,
}

@group(0) @binding(1) 
var baseColorTexture: texture_2d<f32>;
@group(0) @binding(2) 
var baseColorTextureSampler: sampler;
var<private> f_Texcoord_1: vec2<f32>;
@group(0) @binding(3) 
var metallicRoughnessTexture: texture_2d<f32>;
@group(0) @binding(4) 
var metallicRoughnessTextureSampler: sampler;
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
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> outColor: vec4<f32>;
var<private> f_WorldNormal_1: vec3<f32>;

fn main_1() {
    var col: vec4<f32>;
    var baseColor: vec4<f32>;
    var metallicRoughnessColor: vec4<f32>;
    var emissiveColor: vec4<f32>;
    var normalColor: vec4<f32>;
    var occlusionColor: vec4<f32>;
    var loopTime: f32;

    col = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let _e37 = f_Texcoord_1;
    let _e38 = textureSample(baseColorTexture, baseColorTextureSampler, _e37);
    baseColor = _e38;
    let _e39 = f_Texcoord_1;
    let _e40 = textureSample(metallicRoughnessTexture, metallicRoughnessTextureSampler, _e39);
    metallicRoughnessColor = _e40;
    let _e41 = f_Texcoord_1;
    let _e42 = textureSample(emissiveTexture, emissiveTextureSampler, _e41);
    emissiveColor = _e42;
    let _e43 = f_Texcoord_1;
    let _e44 = textureSample(normalTexture, normalTextureSampler, _e43);
    normalColor = _e44;
    let _e45 = f_Texcoord_1;
    let _e46 = textureSample(occlusionTexture, occlusionTextureSampler, _e45);
    occlusionColor = _e46;
    let _e48 = ubo.time;
    let _e49 = (_e48 * 0.10000000149011612);
    loopTime = (_e49 - (floor((_e49 / 1.0)) * 1.0));
    let _e54 = loopTime;
    let _e56 = loopTime;
    if ((_e54 >= 0.0) && (_e56 < 0.20000000298023224)) {
        let _e59 = baseColor;
        let _e60 = _e59.xyz;
        col[0u] = _e60.x;
        col[1u] = _e60.y;
        col[2u] = _e60.z;
    } else {
        let _e67 = loopTime;
        let _e69 = loopTime;
        if ((_e67 >= 0.20000000298023224) && (_e69 < 0.4000000059604645)) {
            let _e72 = metallicRoughnessColor;
            let _e73 = _e72.xyz;
            col[0u] = _e73.x;
            col[1u] = _e73.y;
            col[2u] = _e73.z;
        } else {
            let _e80 = loopTime;
            let _e82 = loopTime;
            if ((_e80 >= 0.4000000059604645) && (_e82 < 0.6000000238418579)) {
                let _e85 = emissiveColor;
                let _e86 = _e85.xyz;
                col[0u] = _e86.x;
                col[1u] = _e86.y;
                col[2u] = _e86.z;
            } else {
                let _e93 = loopTime;
                let _e95 = loopTime;
                if ((_e93 >= 0.6000000238418579) && (_e95 < 0.800000011920929)) {
                    let _e98 = normalColor;
                    let _e99 = _e98.xyz;
                    col[0u] = _e99.x;
                    col[1u] = _e99.y;
                    col[2u] = _e99.z;
                } else {
                    let _e106 = loopTime;
                    let _e108 = loopTime;
                    if ((_e106 >= 0.800000011920929) && (_e108 < 1.0)) {
                        let _e111 = occlusionColor;
                        let _e112 = _e111.xyz;
                        col[0u] = _e112.x;
                        col[1u] = _e112.y;
                        col[2u] = _e112.z;
                    }
                }
            }
        }
    }
    let _e119 = col;
    outColor = _e119;
    return;
}

@fragment 
fn main(@location(1) f_Texcoord: vec2<f32>, @location(0) f_WorldNormal: vec3<f32>) -> @location(0) vec4<f32> {
    f_Texcoord_1 = f_Texcoord;
    f_WorldNormal_1 = f_WorldNormal;
    main_1();
    let _e5 = outColor;
    return _e5;
}
