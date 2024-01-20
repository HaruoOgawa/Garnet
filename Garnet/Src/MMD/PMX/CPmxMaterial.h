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

		glm::vec4 m_Specular;
		float m_SpecularCoef;

		glm::vec4 m_Ambient;

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
		int m_MatRefIndiceCount;

	private:
		void AnalyseDrawBitFlag(unsigned char DrawBitFlag);
	public:
		CPmxMaterial(std::pair<std::string, std::wstring> MaterialName, std::pair<std::string, std::wstring> MaterialName_EN, glm::vec4 Diffuse, glm::vec4 Specular, float SpecularCoef, glm::vec4 Ambient,
			unsigned char DrawBitFlag, glm::vec4 EdgeColor, float EdgeSize, int MainTexIndex, int SphereTexIndex, EPmxSphereMode SphereMode, int ToonTexIndex, int SharedToonTexIndex, 
			std::pair<std::string, std::wstring> MaterialDescription, int MatRefIndiceCount);
		virtual ~CPmxMaterial() = default;

		const std::pair<std::string, std::wstring>& GetMaterialName() const;
		const std::pair<std::string, std::wstring>& GetMaterialName_EN() const;

		const glm::vec4& GetDiffuse() const;

		const glm::vec4& GetSpecular() const;
		float GetSpecularCoef() const;

		const glm::vec4& GetAmbient() const;

		// 描画フラグ(DrawBitFlag)
		bool IsDrawDoubleSlided() const;
		bool IsDrawGroundShadow() const;
		bool IsDrawSelfShadowMap() const;
		bool IsDrawSelfShadow() const;
		bool IsDrawEdge() const;

		// エッジカラー
		const glm::vec4& GetEdgeColor() const;

		// エッジサイズ
		float GetEdgeSize() const;

		// メインテクスチャの参照インデックス
		int GetMainTexIndex() const;

		// スフィアテクスチャの参照インデックス
		int GetSphereTexIndex() const;

		// スフィアモード
		EPmxSphereMode GetSphereMode() const;

		// トゥーンテクスチャ
		int GetToonTexIndex() const;
		int GetSharedToonTexIndex() const;

		// メモ : 自由欄／スクリプト記述／エフェクトへのパラメータ配置など
		const std::pair<std::string, std::wstring>& GetMaterialDescription() const;

		// 材質に対応する面(頂点)数 (必ず3の倍数になる)
		int GetMatRefIndiceCount() const;
	};
}
#endif