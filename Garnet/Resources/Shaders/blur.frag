#version 450

layout(location = 0) in vec2 f_UV;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightVPMat;

    int IsXBlur;
    int KernelSize;
    int pad1;
    int pad2;

    float Kernal[32];
} ubo;

// カーネルサイズを可変にしたいときは大きめの値(例えば33とか)を事前に確保しておいて、それよりも小さい時は空いたとことを0詰めして使用する
// そうすれば配列サイズをいちいち弄らなくてもよくなる
// もしかしたら2のn乗しかだめかも？
//layout(binding = 1) uniform GaussianKernelBuffer{
  //  float Kernal[32];
//} kernel;

layout(binding = 2) uniform texture2D SrcTex;
layout(binding = 3) uniform sampler SamplerSrcTex;

void main() {
    vec3 col = texture(sampler2D(SrcTex, SamplerSrcTex), f_UV).rgb;
    outColor = vec4(col, 1.0);
}