#version 450

layout(location = 0) in vec3 f_WorldNormal;
layout(location = 1) in vec2 f_Texcoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
    mat4 view;
    mat4 proj;
	vec4 emissiveFactor;
    float time;
    float padding0;
    float padding1;
    float padding2;
} ubo;

layout(binding = 1) uniform texture2D baseColorTexture;
layout(binding = 2) uniform sampler baseColorTextureSampler;

layout(binding = 3) uniform texture2D metallicRoughnessTexture;
layout(binding = 4) uniform sampler metallicRoughnessTextureSampler;

layout(binding = 5) uniform texture2D emissiveTexture;
layout(binding = 6) uniform sampler emissiveTextureSampler;

layout(binding = 7) uniform texture2D normalTexture;
layout(binding = 8) uniform sampler normalTextureSampler;

layout(binding = 9) uniform texture2D occlusionTexture;
layout(binding = 10) uniform sampler occlusionTextureSampler;

void main(){
	vec4 col = vec4(1.0);

	vec4 baseColor = texture(sampler2D(baseColorTexture, baseColorTextureSampler), f_Texcoord);
	vec4 metallicRoughnessColor = texture(sampler2D(metallicRoughnessTexture, metallicRoughnessTextureSampler), f_Texcoord);
	vec4 emissiveColor = texture(sampler2D(emissiveTexture, emissiveTextureSampler), f_Texcoord);
	vec4 normalColor = texture(sampler2D(normalTexture, normalTextureSampler), f_Texcoord);
	vec4 occlusionColor = texture(sampler2D(occlusionTexture, occlusionTextureSampler), f_Texcoord);

	float loopTime = mod(ubo.time * 0.1, 1.0);
	if(loopTime >= 0.0 && loopTime < 0.2)
	{
		col.rgb = baseColor.rgb;
	}
	else if(loopTime >= 0.2 && loopTime < 0.4)
	{
		col.rgb = metallicRoughnessColor.rgb;
	}
	else if(loopTime >= 0.4 && loopTime < 0.6)
	{
		col.rgb = emissiveColor.rgb;
	}
	else if(loopTime >= 0.6 && loopTime < 0.8)
	{
		col.rgb = normalColor.rgb;
	}
	else if(loopTime >= 0.8 && loopTime < 1.0)
	{
		col.rgb = occlusionColor.rgb;
	}

	outColor = col;
}