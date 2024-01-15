#pragma once

#ifdef USE_MMD

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace mmd
{
	enum class EPmxSphereMode
	{
		None = 0, // 無効
		Sph = 1,  // 乗算
		Spa = 2,  // 加算
		SubTexture = 3, // 追加UV1のx,yをUV参照して通常テクスチャ描画を行う
	};

	class CPmxMaterial
	{
		std::pair<std::string, std::wstring> m_MaterialName;
		std::pair<std::string, std::wstring> m_MaterialName_EN;

		glm::vec4 m_Diffuse;

		glm::vec3 m_Specular;
		float m_SpecularCoef;

		glm::vec3 m_Ambient;

		// 描画フラグ(DrawBitFlag)
		bool m_DrawDoubleSlided;
		bool m_DrawGroundShadow;
		bool m_DrawSelfShadowMap;
		bool m_DrawSelfShadow;
		bool m_DrawEdge;

		// エッジカラー
		glm::vec4 m_EdgeColor;

		// エッジサイズ
		float m_EdgeSize;

		// メインテクスチャの参照インデックス
		int m_MainTexIndex;

		// スフィアテクスチャの参照インデックス
		int m_SphereTexIndex;

		// スフィアモード
		EPmxSphereMode m_SphereMode;

		// トゥーンテクスチャ
		int m_ToonTexIndex;
		int m_SharedToonTexIndex;

		// メモ : 自由欄／スクリプト記述／エフェクトへのパラメータ配置など
		std::pair<std::string, std::wstring> m_MaterialDescription;

		// 材質に対応する面(頂点)数 (必ず3の倍数になる)
		int m_MatRefIndiceOffset;

	private:
		void AnalyseDrawBitFlag(unsigned char DrawBitFlag);
	public:
		CPmxMaterial(std::pair<std::string, std::wstring> MaterialName, std::pair<std::string, std::wstring> MaterialName_EN, glm::vec4 Diffuse, glm::vec3 Specular, float SpecularCoef, glm::vec3 Ambient, 
			unsigned char DrawBitFlag, glm::vec4 EdgeColor, float EdgeSize, int MainTexIndex, int SphereTexIndex, EPmxSphereMode SphereMode, int ToonTexIndex, int SharedToonTexIndex, 
			std::pair<std::string, std::wstring> MaterialDescription, int MatRefIndiceOffset);
		virtual ~CPmxMaterial() = default;
	};
}
#endif