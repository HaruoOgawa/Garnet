#version 450

layout(location = 0) in vec4 v2f_ObjectPos;

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;

layout(binding = 1) uniform FragmentUniformBuffer{
	mat4 invModel;
    mat4 mPad0;
    mat4 mPad1;
    mat4 mPad2;

	vec4 cameraPos;
    vec4 v4Pad0;
    vec4 v4Pad1;
    vec4 v4Pad2;

    float time;
    float fPad0;
    float fPad1;
    float fPad2;
} f_ubo;

float map(vec3 p)
{
    return length(p) - 0.5;
}

vec3 gn(vec3 p)
{
    vec2 e = vec2(0.0001, 0.0);
    
    return normalize(vec3(
        map(p + e.xyy) - map(p - e.xyy),
        map(p + e.yxy) - map(p - e.yxy),
        map(p + e.yyx) - map(p - e.yyx)
    ));
}

void main()
{
    vec3 ro = (f_ubo.invModel * f_ubo.cameraPos).xyz;
    vec3 rd = normalize(v2f_ObjectPos.xyz - ro);

    float d = 1.0, t = 0.0;

    for(int i = 0; i < 64; i++)
    {
        d = map(ro + rd * t);
        if(d < 0.0001) break;
        t += d;
    }

    if(d < 0.0001 && t < 100.0)
    {
        vec3 p = ro + rd * t;
        vec3 n = gn(p);

        gPosition = vec4(p, 1.0);
	    gNormal = vec4(n, 1.0);
	    gAlbedo = vec4(1.0);
    }
    else
    {
        discard;
    }
}