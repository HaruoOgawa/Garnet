#ifdef USE_MMD
#include "CPmxMaterial.h"

namespace mmd
{
	CPmxMaterial::CPmxMaterial(std::pair<std::string, std::wstring> MaterialName, std::pair<std::string, std::wstring> MaterialName_EN, glm::vec4 Diffuse, glm::vec4 Specular, float SpecularCoef, glm::vec4 Ambient,
		unsigned char DrawBitFlag, glm::vec4 EdgeColor, float EdgeSize, int MainTexIndex, int SphereTexIndex, EPmxSphereMode SphereMode, int ToonTexIndex, int SharedToonTexIndex,
		std::pair<std::string, std::wstring> MaterialDescription, int MatRefIndiceCount) :
		m_MaterialName(MaterialName),
		m_MaterialName_EN(MaterialName_EN),
		m_Diffuse(Diffuse),
		m_Specular(Specular),
		m_SpecularCoef(SpecularCoef),
		m_Ambient(Ambient),
		m_DrawDoubleSlided(false),
		m_DrawGroundShadow(false),
		m_DrawSelfShadowMap(false),
		m_DrawSelfShadow(false),
		m_DrawEdge(false),
		m_EdgeColor(EdgeColor),
		m_EdgeSize(EdgeSize),
		m_MainTexIndex(MainTexIndex),
		m_SphereTexIndex(SphereTexIndex),
		m_SphereMode(SphereMode),
		m_ToonTexIndex(ToonTexIndex),
		m_SharedToonTexIndex(SharedToonTexIndex),
		m_MaterialDescription(MaterialDescription),
		m_MatRefIndiceCount(MatRefIndiceCount)
	{
		AnalyseDrawBitFlag(DrawBitFlag);
	}

	void CPmxMaterial::AnalyseDrawBitFlag(unsigned char DrawBitFlag)
	{
		m_DrawDoubleSlided = (DrawBitFlag & 0x01);
		m_DrawGroundShadow = (DrawBitFlag & 0x02);
		m_DrawSelfShadowMap = (DrawBitFlag & 0x04);
		m_DrawSelfShadow = (DrawBitFlag & 0x08);
		m_DrawEdge = (DrawBitFlag & 0x10);
	}

	const std::pair<std::string, std::wstring>& CPmxMaterial::GetMaterialName() const
	{
		return m_MaterialName;
	}

	const std::pair<std::string, std::wstring>& CPmxMaterial::GetMaterialName_EN() const
	{
		return m_MaterialName_EN;
	}

	const glm::vec4& CPmxMaterial::GetDiffuse() const
	{
		return m_Diffuse;
	}

	const glm::vec4& CPmxMaterial::GetSpecular() const
	{
		return m_Specular;
	}

	float CPmxMaterial::GetSpecularCoef() const
	{
		return m_SpecularCoef;
	}

	const glm::vec4& CPmxMaterial::GetAmbient() const
	{
		return m_Ambient;
	}

	// 描画フラグ(DrawBitFlag)
	bool CPmxMaterial::IsDrawDoubleSlided() const
	{
		return m_DrawDoubleSlided;
	}

	bool CPmxMaterial::IsDrawGroundShadow() const
	{
		return m_DrawGroundShadow;
	}

	bool CPmxMaterial::IsDrawSelfShadowMap() const
	{
		return m_DrawSelfShadowMap;
	}

	bool CPmxMaterial::IsDrawSelfShadow() const
	{
		return m_DrawSelfShadow;
	}

	bool CPmxMaterial::IsDrawEdge() const
	{
		return m_DrawEdge;
	}

	// エッジカラー
	const glm::vec4& CPmxMaterial::GetEdgeColor() const
	{
		return m_EdgeColor;
	}

	// エッジサイズ
	float CPmxMaterial::GetEdgeSize() const
	{
		return m_EdgeSize;
	}

	// メインテクスチャの参照インデックス
	int CPmxMaterial::GetMainTexIndex() const
	{
		return m_MainTexIndex;
	}

	// スフィアテクスチャの参照インデックス
	int CPmxMaterial::GetSphereTexIndex() const
	{
		return m_SphereTexIndex;
	}

	// スフィアモード
	EPmxSphereMode CPmxMaterial::GetSphereMode() const
	{
		return m_SphereMode;
	}

	// トゥーンテクスチャ
	int CPmxMaterial::GetToonTexIndex() const
	{
		return m_ToonTexIndex;
	}

	int CPmxMaterial::GetSharedToonTexIndex() const
	{
		return m_SharedToonTexIndex;
	}

	// メモ : 自由欄／スクリプト記述／エフェクトへのパラメータ配置など
	const std::pair<std::string, std::wstring>& CPmxMaterial::GetMaterialDescription() const
	{
		return m_MaterialDescription;
	}

	// 材質に対応する面(頂点)数 (必ず3の倍数になる)
	int CPmxMaterial::GetMatRefIndiceCount() const
	{
		return m_MatRefIndiceCount;
	}
}
#endif