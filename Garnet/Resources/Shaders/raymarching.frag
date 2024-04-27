#version 450

layout(location = 0) in vec2 v2f_UV;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform FragUniformBufferObject{
	mat4 mPad0;
	mat4 mPad1;
	mat4 mPad2;
	mat4 mPad3;
} fragUbo;

void main()
{
	vec3 col = vec3(0.0);
	#ifdef USE_OPENGL
	vec2 uv = v2f_UV;
	#else
	vec2 uv = vec2(v2f_UV.x, 1.0 - v2f_UV.y);
	#endif

	col.rg = uv;

	outColor = vec4(col, 1.0);
}