#version 450

layout(location = 0) in vec2 f_UV;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject{
    float Kernal[32];

    int UseBlur;
    int KernelSize;
    vec2 Direction;
} ubo;

// カーネルサイズを可変にしたいときは大きめの値(例えば33とか)を事前に確保しておいて、それよりも小さい時は空いたとことを0詰めして使用する
// そうすれば配列サイズをいちいち弄らなくてもよくなる
// もしかしたら2のn乗しかだめかも？
//layout(binding = 1) uniform GaussianKernelBuffer{
  //  float Kernal[32];
//} kernel;

layout(binding = 1) uniform texture2D SrcTex;
layout(binding = 2) uniform sampler SamplerSrcTex;

void main() {
    vec3 col = vec3(0.0);
    vec2 dir = ubo.Direction;

    int halfSize = (ubo.KernelSize - 1) / 2;

    if(ubo.UseBlur != 0)
    {
        for(int i = 0; i < ubo.KernelSize; i++)
        {
            col += texture(sampler2D(SrcTex, SamplerSrcTex), f_UV + dir * float(i - halfSize)).rgb * ubo.Kernal[i];
        }
    }
    else
    {
        col = texture(sampler2D(SrcTex, SamplerSrcTex), f_UV).rgb;
    }
    
    outColor = vec4(col, 1.0);
}