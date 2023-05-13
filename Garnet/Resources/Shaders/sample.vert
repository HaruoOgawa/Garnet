#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 test;
} ubo;

layout(binding = 1) uniform TestBuffer{
    vec4 MulColor;
    vec4 val0;
    vec4 val1;
    vec4 val2;
} testUBO;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor * testUBO.MulColor.rgb;
    fragTexCoord = inTexCoord;
}