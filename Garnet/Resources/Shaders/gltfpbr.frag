#version 450

layout(location = 0) in vec3 f_WorldNormal;
layout(location = 1) in vec2 f_Texcoord;

layout(location = 0) out vec4 outColor;

void main(){
	outColor = vec4(f_WorldNormal * 0.5 + 0.5, 1.0);
}