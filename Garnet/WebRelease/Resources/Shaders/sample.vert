#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;
layout(location = 3) in vec4 inTangent;
layout(location = 4) in vec4 inBioTangent;

layout(binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightVPMat;
} ubo;

layout(location = 0) out vec2 fragTexCoord;

float rand(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
#ifdef USE_OPENGL
    int id = gl_InstanceID;
#else
    int id = gl_InstanceIndex;
#endif

    float f_id = float(id);
    float w = 10.0;

    vec3 offset = vec3(
        w * (rand(vec2(f_id, 55.5)) * 2.0 - 1.0),
        w * (rand(vec2(943.22, f_id)) * 2.0 - 1.0),
        w * (rand(vec2(f_id + 11.111, f_id + 456.123)) * 2.0 - 1.0)
    );

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition + offset, 1.0);
    fragTexCoord = inTexcoord;
}