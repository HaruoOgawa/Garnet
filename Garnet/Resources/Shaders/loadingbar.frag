#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	vec2 uv = fragTexCoord;

	vec2  pad = vec2(0.2, 0.475); // 縦横の隙間・パディング. uvなので0.0 ~ 0.5の値
	uv = (uv - pad) * vec2(1.666, 20.0);  // uvの pad ~ (1.0 - pad) までの区間を 0.0 ~ 1.0 に補完する => ローディングバーの領域

	vec3 col = vec3(0.0);

	if(uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0)
	{
		col.rg = uv;
	}

	outColor = vec4(col, 1.0);
}