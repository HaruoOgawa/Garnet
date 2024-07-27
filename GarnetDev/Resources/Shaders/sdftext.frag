#version 450

layout(location = 0) in vec3 fWolrdNormal;
layout(location = 1) in vec2 fUV;
layout(location = 2) in vec3 fViewDir;

layout(location = 0) out vec4 outCol;

#ifdef USE_OPENGL
layout(binding = 1) uniform sampler2D MainTexture;
#else
layout(binding = 1) uniform texture2D MainTexture;
layout(binding = 2) uniform sampler MainTextureSampler;
#endif

void main()
{
    vec4 col = vec4(0.0f, 0.0, 0.0,1.0);
    
    #ifdef USE_OPENGL
    float dist = texture(MainTexture, vec2(fUV.x, 1.0 - fUV.y)).r;
    #else
    float dist = texture(sampler2D(MainTexture, MainTextureSampler), vec2(fUV.x, 1.0 - fUV.y)).r;
    #endif
    
    float t = 0.5;
    float alpha = smoothstep(t - 0.01, t + 0.01, dist);

    if(alpha > 0.5)
    {
        col.rgb = vec3(1.0);
    }
    else
    {
        discard;
    }

    outCol = col;
}