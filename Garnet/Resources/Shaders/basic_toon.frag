#version 450

layout(location = 0) in vec3 f_WorldNormal;
layout(location = 1) in vec2 f_Texcoord;
layout(location = 2) in vec4 f_WorldPos;
layout(location = 3) in vec3 f_WorldTangent;
layout(location = 4) in vec3 f_WorldBioTangent;
layout(location = 5) in vec4 f_LightSpacePos;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
    mat4 view;
    mat4 proj;
	mat4 lightVPMat;

	vec4 lightDir;
	vec4 lightColor;
	vec4 cameraPos;
	vec4 diffuseFactor;

    int useSkinMeshAnimation;
    int JointIndexOffset;
    int UseMainTexture;
    int UseToonTexture;
} ubo;

#ifdef USE_OPENGL
layout(binding = 2) uniform sampler2D MainTexture;
layout(binding = 4) uniform sampler2D ToonTexture;
#else
layout(binding = 2) uniform texture2D MainTexture;
layout(binding = 3) uniform sampler MainTextureSampler;
layout(binding = 4) uniform texture2D ToonTexture;
layout(binding = 5) uniform sampler ToonTextureSampler;
#endif

void main(){
	vec3 col = vec3(1.0);
	float alpha = 1.0;

	// Diffuse
	vec4 diffuseColor = ubo.diffuseFactor;

	if(ubo.UseMainTexture != 0)
	{
		#ifdef USE_OPENGL
		vec4 MainColor = texture(MainTexture, f_Texcoord);
		#else
		vec4 MainColor = texture(sampler2D(MainTexture, MainTextureSampler), f_Texcoord);
		#endif
	
		diffuseColor.rgb *= MainColor.rgb;
	}

	// Lighting Param
	float NdotL = max(0.0, dot(f_WorldNormal, ubo.lightDir.xyz));

	// Toon
	if(ubo.UseToonTexture != 0)
	{
		#ifdef USE_OPENGL
		float ToonFactor = texture(ToonTexture, f_Texcoord).r;
		#else
		float ToonFactor = texture(sampler2D(ToonTexture, ToonTextureSampler), f_Texcoord).r;
		#endif 

		//diffuseColor.rgb *= ToonFactor;
	}

	// Combine Color
	col = diffuseColor.rgb;

	outColor = vec4(col, alpha);
}