#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
    mat4 view;
    mat4 proj;
    float time;
    float padding0;
    float padding1;
    float padding2;
} ubo;

layout(location = 0) out vec3 f_WorldNormal;
layout(location = 1) out vec2 f_Texcoord;

void main(){
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    f_WorldNormal = (ubo.model * vec4(inNormal, 0.0)).xyz;
    f_Texcoord = inTexcoord;
}