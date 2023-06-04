#version 450

// ひとまずバージョンとプリプロセッサはこちらで仮で入力しておく(別glTFを使用するにはここを対応する必要がある)
#define USE_BASECOLOR_MAP
#define USE_METALLICROUGHNESS_MAP

layout(location = 0) in vec3 f_WorldNormal;
layout(location = 1) in vec2 f_Texcoord;
layout(location = 2) in vec4 f_WorldPos;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
    mat4 view;
    mat4 proj;

	vec4 lightDir;
	vec4 cameraPos;

	vec4 baseColorFactor;
	vec4 emissiveFactor;

    float time;
    float metallicFactor;
    float roughnessFactor;
    float padding2;
} ubo;

#ifdef USE_BASECOLOR_MAP
layout(binding = 1) uniform texture2D baseColorTexture;
layout(binding = 2) uniform sampler baseColorTextureSampler;
#endif

#ifdef USE_METALLICROUGHNESS_MAP
layout(binding = 3) uniform texture2D metallicRoughnessTexture;
layout(binding = 4) uniform sampler metallicRoughnessTextureSampler;
#endif

layout(binding = 5) uniform texture2D emissiveTexture;
layout(binding = 6) uniform sampler emissiveTextureSampler;

layout(binding = 7) uniform texture2D normalTexture;
layout(binding = 8) uniform sampler normalTextureSampler;

layout(binding = 9) uniform texture2D occlusionTexture;
layout(binding = 10) uniform sampler occlusionTextureSampler;

// なんかUnityPBRでもみた値だなぁ
const float MIN_ROUGHNESS = 0.04;
const float PI = 3.14159265;

struct PBRParam
{
	float NdotL;
	float NdotV;
	float NdotH;
	float LdotH;
	float VdotH;
	float perceptualRoughness;
	float metallic;
	vec3 reflectance0;
	vec3 reflectance90;
	float alphaRoughness;
	vec3 diffuseColor;
	vec3 specularColor;
};

// マイクロファセット(微小面法線分布関数)(Microfacet Distribution). Distributionは分布に意味
// 分布関数なので統計学的に求められた関数(数式)
// 物体表面の無数のミクロレベルの各微小平面の法線が確率分布としておおよそどの方向を向いているかの傾向を求める関数
// この傾向から一つの法線を定める
// https://learnopengl.com/PBR/Theory#:~:text=GGX%20for%20G.-,Normal%20distribution%20function,-The%20normal%20distribution
// (統計学、あんまやってないので導出よくわからぬ・・・)
float CalcMicrofacet(PBRParam param)
{
	// roughnessは面の粗さなので値が大きいほど微小平面が多くなるということを表す
	float roughness2 = param.alphaRoughness * param.alphaRoughness; // グラフの勾配を高くする
	
	//
	float f = (param.NdotH * roughness2 - param.NdotH) * param.NdotH + 1.0;
	// = ( param.NdotH * (roughness2 - 1.0) ) * param.NdotH + 1.0
	// = pow(param.NdotH, 2.0) * (roughness2 - 1.0) + 1.0
	// 数式と同じ形になる. (n・h)^2 * (a^2 - 1) + 1
	
	//
	return roughness2 / (PI * f * f);
}

// 幾何減衰項(Geometric Occlusion)
// マイクロファセットの微小平面が光の経路を遮断することにより失われてしまう光の減衰量を計算する関数
float CalcGeometricOcculusion(PBRParam param)
{
	float NdotL = param.NdotL;
	float NdotV = param.NdotV;
	// 表面が荒いほど、微小平面が増えて光が隠蔽されやすくなる
	float r = param.alphaRoughness;

	// 詳しい数式(https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg))
	// シャドウイングの項を計算(入射光が他の微小平面に遮られて影になり光が減衰する分)
	float attenuationL = 2.0 * NdotL / ( NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)) );
	// = 2.0 * NdotL / ( NdotL * () )
	// マスキングの項を計算(反射光が他の微小平面に遮られてその光が目に届かないことで減衰する分)
	float attenuationV = 2.0 * NdotV / ( NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)) );

	// 幾何減衰項は上記の乗算結果
	return attenuationL * attenuationV;
}

// フレネル反射(フレネル項). 
// フレネル反射とはView方向に応じて反射率が変化する物理現象のことである 
// ここでのGGX項でのフレネル反射はオブジェクトの端であるほど反射率が高い(反射色が明るい)ことを示している
// https://marmoset.co/posts/basic-theory-of-physically-based-rendering/
// この画像がわかりやすい --> https://marmoset.co/wp-content/uploads/2016/11/pbr_theory_fresnel.png
// GGXのフレネル項の式は、よく光学の分野で見聞きするようなフレネルの式の近似式である(https://ja.wikipedia.org/wiki/%E3%83%95%E3%83%AC%E3%83%8D%E3%83%AB%E3%81%AE%E5%BC%8F)
// https://learnopengl.com/PBR/Theory#:~:text=return%20ggx1%20*%20ggx2%3B%0A%7D-,Fresnel%20equation,-The%20Fresnel%20equation
vec3 CalcFrenelReflection(PBRParam param)
{
	// 基本の反射率: reflectance0
	// それに対して視野方向による反射率の変化分を加算している
	// 割と数式だとreflectance90は1.0なので今はあんまり深く考えなくてもいいかも？
	return param.reflectance0 + (param.reflectance90 - param.reflectance0) * pow(clamp(1.0 - param.VdotH, 0.0, 1.0), 5.0);
}

// ディフューズのBRDFを計算
// https://google.github.io/filament/Filament.md.html#materialsystem/diffusebrdf
// この記事によると拡散色のBRDFは近似的に『1.0 / PI』と定まるとのこと
vec3 CalcDiffuseBRDF(PBRParam param)
{
	return param.diffuseColor / PI;
}

void main(){
	vec4 col = vec4(1.0);

	// ラフネスとメタリックを取得。テクスチャにパッキングされていることもある
	float perceptualRoughness = ubo.roughnessFactor;
	float metallic = ubo.metallicFactor;

	#ifdef USE_METALLICROUGHNESS_MAP
	// G Channel: Roughness Map, B Channel: Metallic Map 
	vec4 metallicRoughnessColor = texture(sampler2D(metallicRoughnessTexture, metallicRoughnessTextureSampler), f_Texcoord);
	perceptualRoughness = perceptualRoughness * metallicRoughnessColor.g;
	metallic  = metallic  * metallicRoughnessColor.b;
	#endif

	perceptualRoughness = clamp(perceptualRoughness, MIN_ROUGHNESS, 1.0);
	metallic  = clamp(metallic, 0.0, 1.0);

	// 実際に計算に使用するラフネスかな
	float alphaRoughness = perceptualRoughness * perceptualRoughness;

	// ベースカラーの取得. ベースカラーは単純な表面色
	#ifdef USE_BASECOLOR_MAP
	vec4 baseColor = texture(sampler2D(baseColorTexture, baseColorTextureSampler), f_Texcoord);
	#else
	vec4 baseColor = ubo.baseColorFactor;
	#endif
	
	// 
	vec3 f0 = vec3(0.04);
	// diffuseColor. 訳すと拡散色. ライトで照らされたときにその物体が反射する色. 光学的には反射して目に入る色
	// https://help.autodesk.com/view/3DSMAX/2023/ENU/?guid=GUID-3CEBFC6A-7A8F-4E78-9CF2-F78BA9CFCDC3
	// たぶんこの0.04という数値は経験から得られた値で物理学者がいい感じにチューニングして得た綺麗な描画結果を出すのにちょうどいい値ということだと思う
	// → さらに調べてみるとこの0.04は入射反射率4%という意味らしく、たぶんどんな物体でも最低でも4%は反射するということなのかもしれない
	vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0); // 0.04だけ減衰させる. たぶん光エネルギーが色以外のとこで減衰した分を考慮している(?)
	diffuseColor *= (1.0 - metallic); // metallicが1.0ならdiffuseColorは0になる。完全な金属の表面色は周りの映り込み色だけになることを表している
	// specularColor. 意味は鏡面色. サーフェイス上のハイライトの色らしい.
	// https://help.autodesk.com/view/3DSMAX/2023/ENU/?guid=GUID-90065A74-C223-474C-8D85-7596D70E5004
	// 金属であるほどハイライト色がベースカラーに近づく.
	// とはいえ確かに金属はきらっとしてて逆にマットなオブジェクトは全く光ってないようなイメージがある
	// metallic : 0.0(マット) <--> 1.0(金属) って意味だと思う
	vec3 specularColor = mix(f0, baseColor.rgb, metallic); // specularColor

	// 反射率(reflectance)の計算
	// ハイライトカラーはRGBが一番大きいやつの色になるので、どれくらい反射するかもその値(色)を基準にするはずである
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

	// フレネル項で必要なパラメーター
	// reflectanceに対して0.04(4%) * 25.0 = 1.0(100%)といった計算を行い、0.04(4%)以上は強制的に1.0(100%)に、それより下の値で0.0 ~ 1.0の分布を作っている。つまりグラフを急勾配にしている
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specularColor.rgb; // 通常の反射率
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90; // 急勾配ば反射率

	// PBRに使うベクトル系のパラメーターを計算する
	vec3 n = f_WorldNormal;
	vec3 v = normalize(ubo.cameraPos.xyz - f_WorldPos.xyz);
	vec3 l = normalize(ubo.lightDir.xyz);
	// ハーフベクトルはvとlの中間ベクトル
	// 光源の方向ベクトルはCGの慣例として光源方向に向けた方がいいのかも？
	// https://qiita.com/emadurandal/items/76348ad118c36317ec5c#:~:text=%E3%81%97%E3%81%A6%E3%81%84%E3%81%BE%E3%81%99%E3%80%82-,h,%E3%81%AF%E3%83%8F%E3%83%BC%E3%83%95%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%E3%81%A8%E3%81%84%E3%81%84,-%E3%80%81%E3%83%A9%E3%82%A4%E3%83%88%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%E3%81%A8
	vec3 h = normalize(v + l);
	vec3 reflection = -normalize(reflect(v, n));

	// 考え方としては内積は二つのベクトルの角度がどれだけ水平・垂直に近いかを示す値としてみることができる
	float NdotL = clamp(dot(n, l), 0.001, 1.0);
	float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
	float NdotH = clamp(dot(n, h), 0.0, 1.0);
	float LdotH = clamp(dot(l, h), 0.0, 1.0);
	float VdotH = clamp(dot(v, h), 0.0, 1.0);

	//
	PBRParam pbrParam = PBRParam(
		NdotL,
		NdotV,
		NdotH,
		LdotH,
		VdotH,
		perceptualRoughness,
		metallic,
		specularEnvironmentR0,
		specularEnvironmentR90,
		alphaRoughness,
		diffuseColor,
		specularColor
	);

	// クックトランスモデルによるスペキュラーのGGXを計算する
	float D = CalcMicrofacet(pbrParam); // マイクロファセット(微小面法線分布関数)
	float G = CalcGeometricOcculusion(pbrParam); // 幾何減衰項
	vec3 F = CalcFrenelReflection(pbrParam); // フレネル項
	
	// スペキュラーBRDFを構築
	vec3 specularBRDF = D * G * F / (4.0 * NdotL * NdotV);

	// ディフューズBRDFを計算
	vec3 diffuseBRDF = (1.0 - F) * CalcDiffuseBRDF(pbrParam);

	// レンダリング方程式を構築
	col.rgb = NdotL * vec3(1.0) * (specularBRDF + diffuseBRDF);

	// カラースペースをリニアにする
	col.rgb = pow(col.rgb, vec3(1.0/2.2));

	// アルファを指定
	col.a = baseColor.a;

	//
	//vec4 emissiveColor = texture(sampler2D(emissiveTexture, emissiveTextureSampler), f_Texcoord);
	//vec4 normalColor = texture(sampler2D(normalTexture, normalTextureSampler), f_Texcoord);
	//vec4 occlusionColor = texture(sampler2D(occlusionTexture, occlusionTextureSampler), f_Texcoord);

	/*float loopTime = mod(ubo.time * 0.1, 1.0);
	if(loopTime >= 0.0 && loopTime < 0.2)
	{
		col.rgb = baseColor.rgb;
	}
	else if(loopTime >= 0.2 && loopTime < 0.4)
	{
		col.rgb = metallicRoughnessColor.rgb;
	}
	else if(loopTime >= 0.4 && loopTime < 0.6)
	{
		col.rgb = emissiveColor.rgb;
	}
	else if(loopTime >= 0.6 && loopTime < 0.8)
	{
		col.rgb = normalColor.rgb;
	}
	else if(loopTime >= 0.8 && loopTime < 1.0)
	{
		col.rgb = occlusionColor.rgb;
	}*/

	outColor = col;
}