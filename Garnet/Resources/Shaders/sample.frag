#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

/*layout(binding = 2) uniform texture2D u_texture;
layout(binding = 3) uniform sampler u_sampler;*/

layout(location = 0) out vec4 outColor;

void main() {
    vec3 col = vec3(1.0);
    col *= fragColor;

    /*vec4 texCol = texture(sampler2D(u_texture, u_sampler), fragTexCoord);
    outColor = vec4(fragColor.rgb * texCol.rgb, 1.0);*/
    outColor = vec4(col, 1.0);
}