#version 450

layout(location = 0) in vec3 fWolrdNormal;
layout(location = 1) in vec2 fUV;
layout(location = 2) in vec3 fViewDir;

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
    mat4 view;
    mat4 proj;
	mat4 lightVPMat;

	vec4 cameraPos;

	int useDirSampling;
	float time;
	int pad1;
	int pad2;
} ubo;

#ifdef USE_OPENGL
layout(binding = 1) uniform sampler2D texImage;
#else
layout(binding = 1) uniform texture2D texImage;
layout(binding = 2) uniform sampler texSampler;
#endif

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 col = vec3(0.0); 
	vec2 st = fUV;

	if(ubo.useDirSampling != 0)
	{
		float pi = 3.1415;

		float theta = acos(fViewDir.y);
		float phi = atan(fViewDir.z, fViewDir.x);

		st = vec2(phi / (2.0 * pi), theta / pi);
	}

	#ifdef USE_OPENGL
	col = texture(texImage, st).rgb;
	#else
	col = texture(sampler2D(texImage, texSampler), st).rgb;
	#endif

	outColor = vec4(col, 1.0);
}